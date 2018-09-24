/**********************************************************************
 *
 * Filename:    videoServer.h
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

#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

#pragma once
#include <string>
#include <gst/gst.h>
#include <glib.h>
#include <gst/gstbin.h>
#include <glib/gerror.h>
#include <gst/gstbus.h>
#include <gst/gstpipeline.h>
#include <gst/gstparse.h>
#include <gst/gstmessage.h>
#include <thread>
#include <vector>

#include "configReader.h"
#include "videoStreamer_global.h"

#define LOG_TAG "VideoServer"

/*
    This is a "simple" wrapper for gstreamer to stream as many videos as you want. Well kinda,
    Only supports one host right now. But you can stream several camera feeds. The only thing
    you need to do is provide it with the absolute path of the config file.
    The config file needs to have a list of the video streams you want. Like so:
    desktop=/dev/video0
    cam1=/dev/video1
    big boy=/dev/video2

    this also supports the raspberry pi! (Oh boy!) So if you build it on the pi, you need to add this
    line to the conf file:
    raspi=true

    This also means that you need to build the gst for raspberry pi. Check out BUILD for instructions

    You can also use an audio stream! YAY! Add this to your config and pray to your god.
    It has trouble with most devices. Once I figure out how to fix that issue, all will be right
    audio=true
*/

class VideoServer{
public:
    VideoServer(std::string configFile);
    void run();

private:
    std::vector<videoStreamer::Device> devices;
    Socket *control;
    std::string caps = "";
    bool connected = false;
    int width = 1920;
    int height = 1080;
    int framerate = 30;

    void killAllDevices();
    void startDevice(videoStreamer::Device *device);
    void stopDevice(videoStreamer::Device *device);
    void onMessage(Message *message,std::string sender);
};

#endif
