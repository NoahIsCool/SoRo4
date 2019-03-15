#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

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

#include "configreader.h"
#include "core/videostreamer_global.h"
#include "videoserver_global.h"

class VIDEOSERVERSHARED_EXPORT VideoServer
{

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

#endif // VIDEOSERVER_H
