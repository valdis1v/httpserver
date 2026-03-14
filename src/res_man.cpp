#include "res_man.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace fs = std::filesystem;

Ressource_Manager::Ressource_Manager(Ressource_Manager_Config conf)
    : config(conf)
{
    init_match_all();
    init_error_routes();
}

std::string_view Ressource_Manager::request_ressource(const std::string& path)
{
    std::shared_lock lock(lock_ressources);

    auto it = ressource_map.find(path);
    if(it != ressource_map.end())
        return it->second;

    return {};
}

std::string_view Ressource_Manager::request_error_page(const std::string& code)
{
    std::shared_lock lock(lock_error);

    auto it = error_map.find(code + ".html");
    if(it != error_map.end())
        return it->second;

    return {};
}

std::string_view Ressource_Manager::request_or_fallback(const std::string& path)
{
    std::shared_lock lock(lock_ressources);

    auto it = ressource_map.find(path);
    if(it != ressource_map.end())
        return it->second;

    auto f_it = ressource_map.find("404.html");
    if(f_it != ressource_map.end())
        return f_it->second;

    return {};
}

void Ressource_Manager::init_match_all()
{
    fs::path site_path = config.serve_path;

    if (site_path.filename() != "site") {
        throw std::runtime_error("Invalid serve_path: must point to /site directory");
    }

    fs::path browser_path = site_path / "browser";

    if (!fs::exists(browser_path) || !fs::is_directory(browser_path)) {
        throw std::runtime_error("/site/browser directory missing");
    }

    for (const auto& ent : fs::recursive_directory_iterator(browser_path))
    {
        if (!ent.is_regular_file())
            continue;

        std::ifstream file(ent.path(), std::ios::binary);
        if (!file)
            continue;

        std::string resource(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );

        fs::path rel = fs::relative(ent.path(), browser_path);
        std::string path = "/" + rel.generic_string();
        std::string ext = ent.path().extension().string();

        if (ext == ".html" || ext == ".js" || ext == ".css" || ext == ".map")
        {
            ressource_map[path] = resource;
        }
        else if (ext == ".png" || ext == ".jpg" || ext == ".svg")
        {
            images_map[path] = resource;
        }
        else if (ent.file_size() / 1024 / 1024 < 100)
        {
            media_map[path] = resource;
        }
    }
}

void Ressource_Manager::init_error_routes()
{
    fs::path site_path = config.serve_path;

    if (site_path.filename() != "site") {
        throw std::runtime_error("Invalid serve_path: must point to /site directory");
    }

    fs::path error_path = site_path / "error";

    if (!fs::exists(error_path) || !fs::is_directory(error_path)) {
        return;
    }

    for (const auto& ent : fs::recursive_directory_iterator(error_path))
    {
        if (!ent.is_regular_file())
            continue;

        std::ifstream file(ent.path(), std::ios::binary);
        if (!file)
            continue;

        std::string resource(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );

        std::string filename = ent.path().filename().string();

        if (ent.path().extension() == ".html")
        {
            error_map[filename] = resource;
        }
    }
}
