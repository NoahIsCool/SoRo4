#ifndef SHAREDSTRUCTS_H
#define SHAREDSTRUCTS_H

#pragma once
#include <gst/gst.h>
#include <glib.h>
#include <gst/gstbin.h>
#include <glib/gerror.h>
//#include <glib/gconnect.h>
//#include <gst/ginit.h>
#include <gst/gstbus.h>
#include <gst/gstpipeline.h>
#include <gst/gstparse.h>
#include <gst/gstmessage.h>
#include <Socket.h>

namespace videoStreamer{

const int CONTROL_PORT		= 6969;
const int CONTROL_CLIENT_PORT	= 6970;
const int HEARTBEAT_PORT	= 6971;
const int HEARTBEAT_CLIENT_PORT = 6972;
const int CAMERA_PORT_BASE	= 7000;

enum ExtendedMessage : uint8_t{
    START = ERROR + 1,
    STOP,
    STARTED,
    STOPPED,
    STATUS,
    SET,
    CAMERA_IN_USE,
    QUIT,
};

enum DeviceType : uint8_t{
    VIDEO = 0,
    RASPI,
    AUDIO,
};

enum dataLocation : uint8_t{
    CAMERA_INDEX = 0,
    PORT,
    AUDIO_DEVICE,
    NAME_LENGTH,
    NAME,
};

enum SetType : uint8_t{
    RESOLUTION = 0,
    FRAMERATE,
};

enum Resolution : uint8_t{
    HD = 0,
    HD720,
    SD,
};

enum FPS : uint8_t{
    F10 = 0,
    F20,
    F30,
};

struct Device{
    GstElement *pipeline; //GstPipeline pipeline;
    GstBus *bus;
    std::string client;
    std::string name;
    std::string source;
    bool playing;
    bool exists;
    int port;
    DeviceType deviceType;
};

struct ClientDevice{
    GstElement *pipeline;
    GstBus *bus;
    GstElement *source;
    GstCaps *caps;
    GstElement *displayQueue;
    GstElement *depay;
    GstElement *decodeBin;
    GstElement *overlay;
    GstElement *sink;

    GstElement *tee;
    GstElement *recordQueue;
    GstElement *recordingDecodeBin;
    GstElement *recordingEncoder;
    GstElement *recordingMux;
    GstElement *recordingSink;
    std::vector<GstElement *> displays;
    unsigned int lastTimestamp;
    std::string server;
    std::string name;
    bool playing;
    bool recording;
    int port;
    DeviceType deviceType;
};

};

#endif
