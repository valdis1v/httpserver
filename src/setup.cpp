
#include <filesystem>
#include <fstream>
#include "logger.h"
namespace fs = std::filesystem;


/**
*
*  /site
*      /browser
*          / static files.
*      /error
*          / error pages (404.html, 500.html)
*      /config.toml (Server Config)
*
*/

void setup_dir()
{
    try
    {
        fs::create_directories("site/browser");
        fs::create_directories("site/error");
        write_log("Created Server directories", 1);
    } catch (fs::filesystem_error& e)
    {
        std::string msg = std::string("Failed to setup directory:\t") + e.what();
        write_log(msg, 3);
    }
}

void setup_config()
{
    if(!fs::exists("site/config.toml"))
    {
        std::ofstream configfile("site/config.toml");
        configfile << "#SERVERCONFIG \n";
        configfile << "[server]\n";
        configfile << "host = localhost\n";
        configfile << "port = 4200\n";
        configfile.close();
        write_log("Created default configuration", 1);
    }
}
