
#include <filesystem>
#include <fstream>
#include <iostream>
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
        std::cout << "Created Server directories\n";
    } catch (fs::filesystem_error& e)
    {
        std::cerr << "Failed to setup directory:\n " << e.what() << "\n";
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
        std::cout << "Created default configuration\n";
    }
}
