/**********************************************************************
 *
 * Filename:    Socket.cpp
 *
 * Description: Definition of videoServer class
 *
 * Notes:
 *
 *
 * Copyright (c) 2018 by Kelly Klein.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#include "videoserver.h"

using namespace videoStreamer;

VideoServer::VideoServer(std::string configFile){
    gst_init(NULL,NULL);
        // I was trying to make something that is super general because thats kinda my thing. It would be great if we could reuse this code with very little fandangling.
        //Just check out soro2 and soro-research if you want to know what I mean. It took me an entire year to understand what was happening in that code.
        //Then it turns out, you cannot change shit. It is all dependent on everything else and if you delete something like the mbed code (used for the driving system)
        //Then the video streamer does work. Here, there is none of that. Here, you can change shit and not break unrelated shit. Youre welcome.
        /*
        incase you are wondering, this is here so you can enter stuff into a file like this
        frontCamera=video0
        backCamera=video1
        clawCamera=video2
        topCamera=video3
        and it will be parsed and return the device needed.

    This project was origionally made for the Sooner Rover Team and I think my struggles as I try to figure out how to use gstreamer.
    Its difficult to use but I got something working. Then I tried to make it more general and applicable for any project not just the rover.
         */
    ConfigReader reader(configFile);
    if(reader.exists()){
        devices = reader.getDeviceList();
        std::cout << "got list" << std::endl;
        for(int i = 0; i < devices.size(); i++){
            if(devices[i].deviceType == VIDEO){
                int ret = system(std::string("ls " + devices[i].source).c_str());
                    if(ret == 0){
                        std::cout << LOG_TAG << "Good news, everybody! I found the " << devices[i].name << " camera!" << std::endl;
                    devices[i].exists = true;
                    }else{
                        std::cout << LOG_TAG << "Sweet File-not-found of Puget Sound! Cant find the " << devices[i].name << " device. You should probably check to see if it is plugged in." << std::endl;
                    devices[i].exists = false;
                }
            }
        }
        }else{
            int ret = system(std::string("ls /dev/video0").c_str());
                    if(ret == 0){
                        std::cout << LOG_TAG << "Good news, everybody! I found the default camera!" << std::endl;
                videoStreamer::Device device;
                device.name = "default";
                device.source = "/dev/video0";
                device.port = CAMERA_PORT_BASE;
                device.exists = true;
                device.playing = false;
                devices.push_back(device);
                    }else{
                        std::cout << LOG_TAG << "Sweet File-not-found of Puget Sound! Cant find the default device. You should probably check to see if it is plugged in." << std::endl;
                    }
            std::cout << "missig config file! Please put one at " + configFile << std::endl;
    }

    for(int i = 0; i < devices.size(); i++){
        std::cout << LOG_TAG << "using " + devices[i].source + " as device " + std::to_string(i) << std::endl;
    }

    connected = false;
    control = new Socket(CONTROL_PORT);

    std::cout << LOG_TAG << "setup sockets" << std::endl;
}

void VideoServer::run(){
    std::cout << LOG_TAG << "Listening for connection" << std::endl;

    Message message;
    std::string sender;
    int port;
    while(true){
        if(control->available()){
            if(control->getMessage(&message,sender,port)){
                std::cout << "message from " << sender << std::endl;
                onMessage(&message,sender);
            }else{
                std::cout << LOG_TAG << "GOT BAD MESSAGE" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void VideoServer::onMessage(Message *message,std::string sender){
    Message responce;
    switch(message->type){
    case INIT:{
        responce.type = INIT;
        responce.length = 0;
        std::cout << LOG_TAG << "Connected to " << sender << std::endl;
        connected = true;
    break;
    }
    case STATUS:{
        responce.type = (MessageType)STATUS;
        std::string data = "";
        for(int i = 0; i < devices.size(); i++){
            data += std::to_string(i) + std::to_string(devices[i].port) + std::to_string(devices[i].deviceType) + std::to_string(devices[i].name.length() / 10) + std::to_string(devices[i].name.length() % 10) + devices[i].name;
        }
        responce.length = data.length();
        memcpy(responce.data, data.c_str(), data.length());
        std::cout << "status" << std::endl;
    break;
    }
    case START:{
        int index = message->data[CAMERA_INDEX];
        if(!(devices[index].exists && devices[index].playing)){
            devices[index].client = sender;
            startDevice(&devices[index]);
            responce.type = (MessageType)STARTED;
            responce.length = 1;
            responce.data[CAMERA_INDEX] = index;
        }else{
            responce.type = (MessageType)CAMERA_IN_USE;
            responce.length = 0;
        }
        std::cout << "starting device " << devices[index].name << std::endl;
    break;
    }
    case STOP:{
        int index = message->data[CAMERA_INDEX];
        if(devices[index].playing){
            devices[index].client = sender;
            stopDevice(&devices[index]);
            std::cout << "stopping device" << std::endl;
        }else{
            std::cout << "device already stopped" << std::endl;
        }
        responce.type = ACK;
        responce.length = 0;
    break;
    }
    case SET:{
        switch(message->data[0]){
        case RESOLUTION:{
            switch(message->data[1]){
            case HD:
                height = 1080;
                width = 1920;
            break;
            case HD720:
                height = 720;
                width = 1280;
            break;
            case SD:
                height = 480;
                width = 640;
            break;
            };
        std::cout << "set resolution" << std::endl;
        break;
        }
        case FRAMERATE:{
            switch(message->data[1]){
            case F10:
                framerate = 10;
            break;
            case F20:
                framerate = 20;
            break;
            case F30:
                framerate = 30;
            break;
            };
        std::cout << "set framerate" << std::endl;
        break;
        }
        };
        return;
    break;
    }
    case ACK:
        std::cout << "got ack" << std::endl;
        return;
    break;
    case NACK:
        std::cout << "got nack" << std::endl;
        return;
    break;
    case QUIT:
        std::cout << "client disconnecting" << std::endl;
        killAllDevices();
    break;
    default:
        //std::cout << "nope, dont know that one" << std::endl;
        return;
    };
    //std::cout << "sending message type: " << responce.type << " with size of " << sizeof(message) << " with data size of " << responce.length << " to " << sender << " at port " << CONTROL_CLIENT_PORT << std::endl;
    control->write(&responce,sender,CONTROL_CLIENT_PORT);
}

void VideoServer::startDevice(videoStreamer::Device *device){
    std::string binStr;
    switch(device->deviceType){
    case RASPI:
        binStr = "rpicamsrc preview=false ! video/x-raw, width=" + std::to_string(width) + ", height=" + std::to_string(height) + ", framerate=" + std::to_string(framerate) + "/1 ! queue ! videoconvert ! omxh264enc ! rtph264pay pt=96 ! udpsink host=" + device->client + " port=" + std::to_string(device->port);
    break;
    case AUDIO:
        binStr = "autoaudiosrc ! mulawenc ! rtppcmupay ! udpsink host=" + device->client + "  port=" + std::to_string(device->port);
    break;
    case VIDEO:
        binStr = "v4l2src device=" + device->source + " ! videoscale method=0 ! videorate drop-only=true ! videoconvert ! video/x-raw,format=I420,width=" + std::to_string(width) + ",height=" + std::to_string(height) + ",framerate=" + std::to_string(framerate) + "/1 ! x264enc speed-preset=ultrafast tune=zerolatency bitrate=2048 ! rtph264pay config-interval=3 pt=96 ! udpsink host=" + device->client + " port=" + std::to_string(device->port);
    break;
    };
    device->pipeline = gst_parse_launch(binStr.c_str(),NULL);
    device->bus = gst_element_get_bus(device->pipeline);

    gst_bus_add_signal_watch(device->bus);

    gst_element_set_state(device->pipeline,GST_STATE_PLAYING);
    device->playing = true;
}

void VideoServer::stopDevice(videoStreamer::Device *device){
    if(device->playing == true){
        std::cout << "stopping device " << device->name << std::endl;
        gst_element_set_state(device->pipeline,GST_STATE_NULL);
        device->client = "";
        gst_object_unref(device->bus);
        gst_object_unref(device->pipeline);
        device->pipeline = gst_pipeline_new(device->name.c_str());
        device->playing = false;
    }
}

void VideoServer::killAllDevices(){
    for(int i = 0; i < devices.size(); i++){
        stopDevice(&devices[i]);
    }
}


/*
    I used to have the heartbeat within this class but decided it would be better to have the controlling
    project take care of this. However, I left in the code if you wanted to see how I would handle it.
*/
/*void VideoServer::onHeartbeat(){
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    Message beat;
    beat.type = HEARTBEAT;
    beat.length = 0;
    while(connected){
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> span = t2 - t1;
        if(span.count() > 5000){
            std::cout << span.count() << std::endl;
            connected = false;
            LOG_W(LOG_TAG,"timeout");
            killAllDevices();
        }

        if(connected){
            if(heartbeat->available()){
                Message back;
                std::string sender;
                int port;
                if(heartbeat->getMessage(&back,sender,port)){
                    heartbeat->write(&beat,sender,HEARTBEAT_CLIENT_PORT);
                }else{
                    std::cout << "nacking heartbeat" << std::endl;
                    Message nack;
                    nack.type = NACK;
                    nack.length = 0;
                    heartbeat->write(&nack,sender,HEARTBEAT_CLIENT_PORT);
                }
                t1 = std::chrono::high_resolution_clock::now();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}*/
