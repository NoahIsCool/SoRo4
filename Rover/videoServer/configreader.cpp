#include "configReader.h"

using namespace logger;
using namespace videoStreamer;

ConfigReader::ConfigReader(std::string config){
    std::ifstream in;
    in.open(config,std::ios::in);
    if(in.fail()){
        doesExist = false;
        LOG_W(LOG_TAG,"config file " + config + " does not exist.");
        LOG_W(LOG_TAG,"WHATEVER! I DO WHAT I WANT!");
        return;
    }
    LOG_E(LOG_TAG,"File exists! Oh Boy!");
    doesExist = true;
    std::string line = "";
    LOG_I(LOG_TAG,line);
    while(std::getline(in,line)) {
        std::vector<std::string>  fields = useful::split(line,'=');

        StringPair pair;
        pair.tag = fields[0];
        pair.value = fields[1];
        list.push_back(pair);
    }
}

bool ConfigReader::exists(){
    return doesExist;
}

std::vector<videoStreamer::Device> ConfigReader::getDeviceList(){
    std::vector<videoStreamer::Device> returnList;
    returnList.resize(list.size());
    for(int i = 0; i < list.size(); i++){
        StringPair pair = list[i];
        videoStreamer::Device device;
        if(pair.tag == "raspi"){
            device.name = pair.value;
            device.source = "piCam";
            device.deviceType = RASPI;
        }else if(pair.tag == "audio"){
            device.name = pair.value;
            device.source = "hw1";
            device.deviceType = AUDIO;
        }else{
            device.name = pair.tag;
            std::size_t found = pair.value.find("/dev/");
            if(found != std::string::npos){
                device.source = pair.value;
            }else{
                device.source = "/dev/" + pair.value;
            }
            device.deviceType = VIDEO;
        }
        device.exists = true;
        device.playing = false;
        device.port = CAMERA_PORT_BASE + i;
        returnList[i] = device;
    }

    return returnList;
}

std::string ConfigReader::find(std::string tag){
    for(StringPair pair : list){
        if(pair.tag == tag){
            return pair.value;
        }
    }
    return "NOT_FOUND";
}
