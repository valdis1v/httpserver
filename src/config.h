

#ifndef CONFIG
#define CONFIG
#include <string>

struct SERVERCONFIG
{
    int port;
    std::string host;
    std::string site_dir;
};

SERVERCONFIG parse_configfile();


#endif
