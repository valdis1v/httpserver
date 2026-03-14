#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

struct Ressource_Manager_Config
{
    std::string serve_path;
    std::vector<std::string> friendly_ext;
    std::vector<std::string> ignore_dir;
};

class Ressource_Manager
{
private:
Ressource_Manager_Config config;
std::unordered_map<std::string, std::string> ressource_map;
std::shared_mutex lock_ressources;

std::unordered_map<std::string, std::string> images_map;
std::shared_mutex lock_images;

std::unordered_map<std::string, std::string>  media_map;
std::shared_mutex lock_media;

std::map<std::string, std::string> error_map;
std::shared_mutex lock_error;

    void init_match_all();
    void init_error_routes();

public:
    explicit Ressource_Manager(Ressource_Manager_Config conf);

    std::string_view request_ressource(const std::string& path);
    std::string_view request_error_page(const std::string& code);
    std::string_view request_or_fallback(const std::string& path);
};

#endif
