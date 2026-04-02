

#include <fstream>
#include <functional>
#include <map>
#include "logger.h"
#include <string>
#include "config.h"

SERVERCONFIG parse_configfile() {
    SERVERCONFIG config;
    std::map< std::string, std::function<void(const std::string&)>> handler
    = {
        { "port",       [&](const std::string& v) { config.port = std::stoi(v); }},
        { "host",       [&](const std::string& v) { config.host = v; }},
        { "site_dir",   [&](const std::string& v) { config.site_dir = v; }},
    };
    std::ifstream file("site/.conf", std::ios::binary);
    if(!file) {
        write_log("Failed to find site/conf.\n Ensure that a config file exists or run this program with the -setup Flag", 3);
    };
    std::string line;
    while(std::getline(file , line))
    {
        if(line.empty() || line[0] == '#')
        {
            continue;
        }
        auto pos = line.find('=');
        if(pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if(handler.count(key)) {
                handler[key](value);
            }
        }
    }
    return config;

}
