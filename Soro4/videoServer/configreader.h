#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>
#include <fstream>
#include <vector>
#include <QDebug>
#include <core/soro_global.h>

#include "videoserver_global.h"

struct StringPair{
    std::string tag;
    std::string value;
};

class ConfigReader{
public:
    ConfigReader(std::string file);
    std::string find(std::string tag);
    std::vector<videoStreamer::Device> getDeviceList();
    bool exists();

private:
    std::vector<std::string> split(std::string &str,char dil);

    std::vector<StringPair> list;
    bool doesExist = true;
};

#endif
