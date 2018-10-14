/**********************************************************************
 *
 * Filename:    Socket.cpp
 *
 * Description: Definition of videoClient class
 *
 * Notes:
 *
 *
 * Copyright (c) 2018 by Kelly Klein.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#ifndef VIDEOCLIENT_H
#define VIDEOCLIENT_H

#include <thread>
#include <iostream>
#include <gst/gstpipeline.h>
#include <Socket.h>
#include <vector>
#include <useful.h>

#include "configreader.h"
#include "videostreamer_global.h"
#include <core/soro_global.h>

/*
    Probably isnt the best way to do this, but you need a config file that has only one
    variable. The ip address of the video server. So just have a file, and pass the absolute
    file location to the constructor and have it with one variable like this: server=192.168.1.183

    This is just a wrapper for gstreamer to make things a little simpler, because let me tell you...
    that shit aint simple and the creators enjoy seeing you struggle. Documentation is shit. Actually,
    Ive found most documentation is shit so good luck.
*/

const int fps = 0;
const int resolution = 1;
const bool NO_TERMINAL = true;

class VideoClient{
public:
    /*just starts up and loads the system.
    This was origionally made for the Sooner Rover team and some of us dont like
    terminal displays. *eye rolls* So I had to make it so you can control it from a gui
    that isnt appart of this system. It may be a little weird control but I think it works
    */
    VideoClient(std::string configFile);
    //I thought the best way would be to run this method in another thread because it is blocking.
    void run();
    //if you do not want to use the terminal input, this method returns weather or not there are videos you can stream from.
    //if there are, then it copies the list for you.
    bool requestList(std::vector<std::string> &list);
    //starts the video of the device index from the list that you requested. So if you want to start the third stream from the requestedList, you would call requestStart(3)
    //this does not block
    //returns if this is a valid index
    bool requestStart(int deviceIndex);
    //same as the startDevice except it kills the video stream
    //returns if this is a valid index
    bool requestStop(int deviceIndex);
    //updates the video settings. So check out the videoStreamer_global.h to see a full list but you could pick to change the resolution to 1080p for example
    bool updateSettings(int type,int typeIndex);
    //just returns if it is connected to the server
    bool isConnected();
    //if you wanted to add a text field to the video display, you can do that here! just give it the device index (that you got from the requestList) and the initial message you want displayed.
    //it returns the handler of the text device used on the display. This is only useful if you want to update its contents with the update method
    //this can only be used before startDevice is called. It adds a text element to the pipeline but it is only generated in the startDevice method.
    int addDisplay(int deviceIndex,std::string initMessage);
    //if it is something dynamic, like maybe the speed for a drone, then you can update it here. Just give it the index of the device and the index of the handler.
    void update(int deviceIndex,int handler,std::string message);

    void kill();
    ~VideoClient();

private:
    //void beatHeart();
    void startDevice(int deviceIndex);
    void stopDevice(int deviceIndex);
    void onMessage(Message packet);
    void processInput();
    void onHeartbeat();

    std::string server;
    Socket *control;
    std::thread *messageThread;
    std::vector<videoStreamer::ClientDevice> devices;
    bool connected;
    bool running;
    int framerate = 30;
    bool terminalInput = true;

    const bool DEBUG = true;
};

#endif
