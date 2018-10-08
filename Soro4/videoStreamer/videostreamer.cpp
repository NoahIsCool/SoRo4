/**********************************************************************
 *
 * Filename:    videoClient.cpp
 *
 * Description: Implementations of videoClient class
 *
 * Notes:
 *
 *
 * Copyright (c) 2018 by Kelly Klein.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#include "videostreamer.h"

using namespace videoStreamer;

VideoClient::VideoClient(std::string configFile/*,bool input*/){
    gst_init(NULL,NULL);
    //terminalInput = input;
    ConfigReader reader(configFile);
    if(!reader.exists()){
            LOG_W(LOG_TAG,"no config file found using defaults");
            server = "192.168.1.183";

    }else{
            server = reader.find("server");
    }
    connected = false;
    running = true;
    LOG_I(LOG_TAG,"server address is " + server);
    control = new Socket(CONTROL_CLIENT_PORT);
    //heartbeat = new Socket(HEARTBEAT_CLIENT_PORT);
}

void VideoClient::run(){
    LOG_I(LOG_TAG,"connecting...");
    Message init;
    init.type = INIT;
    init.length = 0;

    //initally blocks until the client manages to connect to the server
    while(!connected){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> span = t2 - t1;
        control->write(&init,server,CONTROL_PORT);
        while(span.count() < 1000){
            if(control->available()){
                Message message;
                std::string sender;
                int port;
                control->getMessage(&message,sender,port);
                onMessage(message);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            t2 = std::chrono::high_resolution_clock::now();
            span = t2 - t1;
        }
    }

    //now launches this into the background and returns.
    messageThread = new std::thread([this](){
        while(running){
            /*while(!connected){
                std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> span = t2 - t1;
                control->write(&init,server,CONTROL_PORT);
                while(span.count() < 1000){
                    if(control->available()){
                        Message message;
                        std::string sender;
                        int port;
                        control->getMessage(&message,sender,port);
                        onMessage(message);
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    t2 = std::chrono::high_resolution_clock::now();
                    span = t2 - t1;
                }
            }*/

            std::chrono::high_resolution_clock::time_point lagT1 = std::chrono::high_resolution_clock::now();
            Message message;
            std::string sender;
            int port;
            while(connected){
                if(control->available()){
                    if(control->getMessage(&message,sender,port)){
                        onMessage(message);
                    }else{
                        std::cout << "nacking message " << message.type << std::endl;
                        std::cout << "length: " << message.length << std::endl;
                        std::string data = std::string((char *)message.data);
                        std::cout << data << std::endl;
                        Message nack;
                        nack.type = NACK;
                        nack.length = 0;
                        control->write(&nack,server,CONTROL_PORT);
                    }
                }
                std::chrono::high_resolution_clock::time_point lagT2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> time = lagT2 - lagT1;
                if(time.count() > 2000){
                    double lag = control->checkLag(10, server, CONTROL_PORT, CONTROL_CLIENT_PORT);
                    for(int i = 0; i < devices.size(); i++){
                        if(devices[i].playing){
                            GstClockTime time;
                            devices[i].lastTimestamp = time;
                            g_object_set(devices[i].overlay, "text", std::string("lag time: " + std::to_string(lag) + "ms").c_str(), NULL);
                        }
                    }
                    lagT1 = std::chrono::high_resolution_clock::now();
                }else{
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void VideoClient::onMessage(Message message){
    Message responce;
    switch(message.type){
    case INIT:{
        responce.type = (MessageType)STATUS;
        responce.length = 0;
        connected = true;
        LOG_I(LOG_TAG,"Connected to server",DEBUG);
    break;
    }
    case STATUS:{
        responce.type = ACK;
        responce.length = 0;
        //std::cout << "\n\navailable devices:" << std::endl;
        //std::cout << std::endl;
        for(int i = 0; i < message.length;){
            ClientDevice device;
            std::string tmp;
            tmp += message.data[i+1];
            tmp += message.data[i+2];
            tmp += message.data[i+3];
            tmp += message.data[i+4];
            device.port = std::stoi(tmp);
            tmp = message.data[i+5];
            device.deviceType = (DeviceType)std::stoi(tmp);
            tmp = message.data[i+6];
            tmp += message.data[i+7];
            int length = std::stoi(tmp);
            tmp = "";
            for(int j = 0; j < length; j++){
                device.name += message.data[j+8+i];
            }
            device.server = server;
            //std::cout << device.name + "\t\tenabled";
            /*if(device.deviceType == AUDIO){
                std::cout << "\t\taudio device" << std::endl;
            }else if(device.deviceType == VIDEO){
                std::cout << "\t\tvideo device" << std::endl;
            }else if(device.deviceType == RASPI){
                std::cout << "\t\traspberry pi device" << std::endl;
            }*/
            i += 8 + length;
            device.playing = false;
            device.lastTimestamp = 0;
            devices.push_back(device);
        }
    break;
    }
    case STARTED:{
        std::cout << devices[message.data[CAMERA_INDEX]].name << " stream started" << std::endl;
        startDevice(message.data[CAMERA_INDEX]);
        responce.type = ACK;
        responce.length = 0;
    break;
    }
    case STOPPED:{
        std::cout << devices[message.data[CAMERA_INDEX]].name << " stream stopped" << std::endl;
        stopDevice(message.data[CAMERA_INDEX]);
        responce.type = ACK;
        responce.length = 0;
    break;
    }
    };

    control->write(&responce,server,CONTROL_PORT);
}

/*
    So I had this in here because it was supposed to be a standalone program. But I changed that so
    it can be used on any project. Anyways, I left in the code as an example but a better one is in the main file
*/
/*void VideoClient::onCommand(std::string command){
    Message message;
    std::vector<std::string> commandsplit = useful::split(command,' ');
    if(commandsplit[0] == "start"){
        bool found = false;
        message.type = (MessageType)START;
        message.length = 1;
        for(int i = 0; i < devices.size(); i++){
            if(commandsplit[1] == devices[i].name){
                message.data[CAMERA_INDEX] = i;
                found = true;
            }
        }
        if(!found){
            std::cout << "could not find " << commandsplit[1] << std::endl;
            return;
        }
    }else if(commandsplit[0] == "stop"){
        bool found = false;
        message.type = (MessageType)STOP;
        message.length = 1;
        for(int i = 0; i < devices.size(); i++){
            if(commandsplit[1] == devices[i].name){
                std::cout << "stopping " << devices[i].name << " stream" << std::endl;
                message.data[CAMERA_INDEX] = i;
                stopDevice(i);
                found = true;
            }
        }
        if(!found){
            std::cout << "could not find " << commandsplit[1] << std::endl;
            return;
        }
    }else if(commandsplit[0] == "set"){
            message.type = (MessageType)SET;
            message.length = 2;
        if(commandsplit[1] == "resolution"){
            if(commandsplit[2] == "1080"){
                message.data[0] = videoStreamer::RESOLUTION;
                message.data[1] = HD;
                std::cout << "setting resolution to 1080p" << std::endl;
            }else if(commandsplit[2] == "720"){
                message.data[0] = videoStreamer::RESOLUTION;
                message.data[1] = HD720;
                std::cout << "setting resolution to 720p" << std::endl;
            }else if(commandsplit[2] == "480"){
                message.data[0] = videoStreamer::RESOLUTION;
                message.data[1] = SD;
                std::cout << "setting resolution to 480p" << std::endl;
            }else{
                std::cout << "not a resolution" << std::endl;
                return;
            }
        }else if(commandsplit[1] == "framerate"){
            if(commandsplit[2] == "10"){
                message.data[0] = FRAMERATE;
                message.data[1] = F10;
                framerate = 10;
                std::cout << "setting framerate to 10fps" << std::endl;
            }else if(commandsplit[2] == "20"){
                message.data[0] = FRAMERATE;
                message.data[1] = F20;
                framerate = 20;
                std::cout << "setting framerate to 20fps" << std::endl;
            }else if(commandsplit[2] == "30"){
                message.data[0] = FRAMERATE;
                message.data[1] = F30;
                framerate = 30;
                std::cout << "setting framerate to 30fps" << std::endl;
            }else{
                std::cout << "not a valid frame rate" << std::endl;
                return;
            }
        }else{
            std::cout << "nope, not an option" << std::endl;
            return;
        }
    }else if(commandsplit[0] == "status"){
        message.type = (MessageType)STATUS;
        message.length = 0;
        std::cout << "requesting status" << std::endl;
    }else if(commandsplit[0] == "exit"){
        message.type = (MessageType)QUIT;
        message.length = 0;
        control->write(&message,server,CONTROL_PORT);
        exit(0);
    }else{
        std::cout << "not recognized brah" << std::endl;
        return;
    }

    control->write(&message,server,CONTROL_PORT);
}*/

void VideoClient::startDevice(int deviceIndex){
    ClientDevice &device = devices[deviceIndex];
    if(device.deviceType == AUDIO){
        std::string binStr = "udpsrc port=" + std::to_string(device.port) + " caps=\"application/x-rtp\" ! queue ! rtppcmudepay ! mulawdec ! audioconvert ! alsasink";
                device.pipeline = gst_parse_launch(binStr.c_str(),NULL);
        device.bus = gst_element_get_bus(device.pipeline);
        gst_bus_add_signal_watch(device.bus);
                gst_element_set_state(device.pipeline,GST_STATE_PLAYING);
        device.playing = true;
        return;
    }

    /*std::string binStr = "udpsrc port=" + std::to_string(device.port) + " caps = \"application/x-rtp, media=(string)video, framerate=30/1, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! vaapidecodebin threads=8 ! textoverlay text=\"lag: 0ms\" valignment=top halignment=left shaded-background=true ! vaapisink"; //autovideosink
    device.pipeline = gst_parse_launch(binStr.c_str(),NULL);*/
    device.source = gst_element_factory_make("udpsrc",NULL);
    g_object_set(G_OBJECT(device.source),"port",device.port,NULL);
    device.caps = gst_caps_new_simple("application/x-rtp",
        "media",G_TYPE_STRING,"video",
        "framerate",GST_TYPE_FRACTION,framerate,1,
        "encoding-name",G_TYPE_STRING,"H264",
        "payload",G_TYPE_INT,"96",
        NULL);
    g_object_set(device.source,"caps",device.caps,NULL);
    //g_object_set(G_OBJECT(device.source),"caps","application/x-rtp, media=(string)video, framerate=30/1, encoding-name=(string)H264, payload=(int)96",NULL);
    device.depay = gst_element_factory_make("rtph264depay",NULL);
    device.decodeBin = gst_element_factory_make("vaapidecodebin",NULL);
    g_object_set(device.decodeBin,"max-size-buffers",100,NULL);
    device.overlay = gst_element_factory_make("textoverlay",NULL);
    g_object_set(device.overlay,"text","lag: 0ms","valignment",2,"halignment",2,"shaded-background",true,NULL);
    device.sink = gst_element_factory_make("vaapisink",NULL);

    if(!device.source || !device.depay || !device.decodeBin || !device.overlay || !device.sink){
        std::cout << "failed to create elements" << std::endl;
        exit(-1);
    }

    device.pipeline = gst_pipeline_new(std::string(device.name + "-pipeline").c_str());
    gst_bin_add_many(GST_BIN(device.pipeline), device.source, device.depay, device.decodeBin, device.overlay, device.sink, NULL);

    for(int i = 0; i < device.displays.size(); i++){
        if(!device.displays[i]){
            std::cout << "failed to create display at: " << i << std::endl;
        }
        gst_bin_add(GST_BIN(device.pipeline),device.displays[i]);
    }

    //FIXME: figure out how to link more items...
    if(!gst_element_link_many(device.source, device.depay, device.decodeBin, device.overlay, device.sink, NULL)){
        std::cout << "failed to link devices" << std::endl;
        exit(-1);
    }
    for(int i = 0; i < device.displays.size(); i++){
        if(!gst_element_link(device.source,device.displays[i])){
        std::cout << "failed to link device at: " << i << std::endl;
        exit(-1);
    }
    }

    device.bus = gst_element_get_bus(device.pipeline);
    gst_bus_add_signal_watch(device.bus);
    gst_element_set_state(device.pipeline,GST_STATE_PLAYING);
    device.playing = true;
}

void VideoClient::stopDevice(int deviceIndex){
    if(devices[deviceIndex].playing){
        ClientDevice &device = devices[deviceIndex];
        device.playing = false;
        gst_element_set_state(device.pipeline,GST_STATE_NULL);
        gst_object_unref(device.pipeline);
        gst_object_unref(device.bus);
        //gst_object_unref(device.source);
        //gst_object_unref(device.depay);
        //gst_object_unref(device.decodeBin);
        //gst_object_unref(device.overlay);
        //gst_object_unref(device.sink);
    }
}

bool VideoClient::requestStart(int index){
    if(index < devices.size()){
        Message message;
        message.type = (MessageType)START;
        message.length = 1;
        message.data[CAMERA_INDEX] = index;
        control->write(&message,server,CONTROL_PORT);
    }

    return false;
}

bool VideoClient::requestStop(int index){
    if(index < devices.size()){
        Message message;
        message.type = (MessageType)STOP;
        message.length = 1;
        message.data[CAMERA_INDEX] = index;
        control->write(&message,server,CONTROL_PORT);

        stopDevice(index);
    }

    return false;
}

bool VideoClient::requestList(std::vector<std::string> &list){
    if(devices.size() > 0){
        list.clear();
        for(int i = 0; i < devices.size(); i++){
            list.push_back(devices[i].name);
        }
        return true;
    }

    return false;
}

bool VideoClient::updateSettings(int type,int value){
    Message message;
    message.type = (MessageType)SET;
    message.length = 2;
    switch(type){
    case fps:
        message.data[0] = FRAMERATE;
        switch(value){
        case F10:
            framerate = value;
            message.data[1] = value;
        break;
        case F20:
            framerate = value;
            message.data[1] = value;
        break;
        case F30:
            framerate = value;
            message.data[1] = value;
        break;
        default:
            return false;
        break;
        };
    break;
    case resolution:
        message.data[0] = videoStreamer::RESOLUTION;
        switch(value){
        case HD:
            message.data[1] = HD;
        break;
        case HD720:
            message.data[1] = HD720;
        break;
        case SD:
            message.data[1] = SD;
        break;
        default:
            return false;
        break;
        };
    break;
    default:
        return false;
    break;
    };

    control->write(&message,server,CONTROL_PORT);
    return true;
}

int VideoClient::addDisplay(int deviceIndex,std::string initMessage){
    devices[deviceIndex].displays.push_back(gst_element_factory_make("textoverlay",NULL));
    g_object_set(devices[deviceIndex].displays.back(),"text",initMessage.c_str(),"valignment",2,"halignment",1,"shaded-background",true,NULL);

    return devices[deviceIndex].displays.size() - 1;
}

void VideoClient::update(int deviceIndex,int handler,std::string message){
    g_object_set(devices[deviceIndex].displays[handler], "text", message.c_str(), NULL);
}

bool VideoClient::isConnected(){
    return connected;
}

/*
    I used to have the heartbeat within this class but decided it would be better to have the controlling
    project take care of this. However, I left in the code if you wanted to see how I would handle it.
*/
/*void VideoClient::onHeartbeat(){
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    Message beat;
    beat.type = HEARTBEAT;
    beat.length = 0;
    heartbeat->write(&beat,server,HEARTBEAT_PORT);
    while(connected){
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> span = t2 - t1;
        if(span.count() > 5000){
            connected = false;
            std::cout << "total time: " << span.count() << std::endl;
            LOG_W(LOG_TAG,"timeout");
            return;
        }

        if(heartbeat->available()){
            Message back;
            std::string sender;
            int port;
            if(heartbeat->getMessage(&back,sender,port)){
                heartbeat->write(&beat,server,HEARTBEAT_PORT);
            }else{
                std::cout << "nacking heartbeat" << std::endl;
                Message nack;
                nack.type = NACK;
                nack.length = 0;
                heartbeat->write(&nack,server,HEARTBEAT_PORT);
            }
            t1 = std::chrono::high_resolution_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}*/

void VideoClient::kill(){
    Message message;
    message.type = (MessageType)QUIT;
    message.length = 0;
    control->write(&message,server,CONTROL_PORT);
    connected = false;
    //messageThread->join();
}

VideoClient::~VideoClient(){
    connected = false;
    running = false;
    messageThread->join();
    //heartbeatThread->join();
    //commandThread->join();
}
