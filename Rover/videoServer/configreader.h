#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>
#include <fstream>
#include <vector>
#include <useful.h>

#include "videoStreamer_global.h"

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
    std::vector<StringPair> list;
    bool doesExist = true;
};

#endif
