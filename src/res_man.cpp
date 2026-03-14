
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <map>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <cstddef>

enum Method { Get, Post, Put, Delete, Unset};
enum ContentType { All, Html, Script, Json, Css };


Method method_from(std::string_view val) {
    if(val.find("GET") != std::string_view::npos) return Get;
    if(val.find("POST")!= std::string_view::npos) return Post;
    if(val.find("PUT")!= std::string_view::npos) return Put;
    if(val.find("DELETE")!= std::string_view::npos) return Delete;
    else {
        return Unset;
    }
}

std::string contenttype_into(ContentType type)
{
    if(type == Html) {return "text/html"; }
    if(type == Script) {return "text/javascript"; }
    if(type == Css) {return "text/css"; }
    if(type == Json) {return "application/json"; }
    else {
        return "*/*";
    }
}

class HttpResponse
{
    public:

        std::string status;
        std::string content_type;
        size_t content_length;
        std::string body;


        static HttpResponse OK(
            ContentType Type,
            std::string content
        )
        {
            HttpResponse resp;
            resp.body = content;
            resp.content_length = content.length();
            resp.status = "HTTP/1.1 200 OK";
            resp.content_type = contenttype_into(Type);
            return resp;
        }

        static HttpResponse NotFound(
            ContentType Type = Html,
            std::string content = "<html></html>"
        )
        {
            HttpResponse resp;
            resp.body = content;
            resp.content_length = content.length();
            resp.status = "HTTP/1.1 404 NOT FOUND";
            resp.content_type = contenttype_into(Type);
            return resp;
        }

        static HttpResponse IntServerError(
            ContentType Type = Html,
            std::string content = "<html></html>"
        )
        {
            HttpResponse resp;
            resp.body = content;
            resp.content_length = content.length();
            resp.status = "HTTP/1.1 500";
            resp.content_type = contenttype_into(Type);
            return resp;
        }

        std::string into_writable()
        {
            std::ostringstream stream1;
            stream1 << status << "\r\n";
            stream1 << "Content-Length: " << content_length << "\r\n";
            stream1 << "Content-Type: " << content_type << "\r\n";
            stream1 << "\r\n";
            stream1 << body;
            return stream1.str();
        }
};

class HttpRequest
{
    public:

        Method method;
        std::string http_v;

        std::string path;
        std::string body;
        std::string host;
        std::string user_agent;

        size_t content_length;
        ContentType content_type;

        HttpRequest() {}

        static HttpRequest from(const char* buffer)
        {
            std::string_view view_buffer(buffer);
            HttpRequest request;

            // Request line
            std::string_view line1 = view_buffer.substr(0, view_buffer.find("\r\n"));

            auto method_i = line1.find(' ');
            auto path_i = line1.find(' ', method_i + 1);

            if(method_i == std::string_view::npos || path_i == std::string_view::npos)
                return request;

            std::string_view method  = line1.substr(0, method_i);
            std::string_view path    = line1.substr(method_i + 1, path_i - method_i - 1);
            std::string_view version = line1.substr(path_i + 1);

            request.method = method_from(method);
            request.path = std::string(path);
            request.http_v = std::string(version);

            size_t body_i = view_buffer.find("\r\n\r\n");
            if(body_i != std::string_view::npos)
            {
                request.body = std::string(view_buffer.substr(body_i + 4));
            }

            return request;
        }
};

namespace fs = std::filesystem;

void create_defaults(std::string path) {
    if(!fs::exists(path)) {
        throw std::runtime_error("Invalid path. Couldnt create defaults");
    }
    std::filesystem::create_directory(path + "/site");
    std::filesystem::create_directory(path + "/error");
}


struct Ressource_Manager_Config
{
    std::string serve_path;
    std::vector<std::string> friendly_ext;
    std::vector<std::string> ignore_dir;
};

class Ressource_Manager
{

    Ressource_Manager_Config config;
    std::map<std::string, std::string> ressource_map;
    std::shared_mutex lock_ressources;

    std::map<std::string, std::string> images_map;
    std::shared_mutex lock_images;

    std::map<std::string, std::string> media_map;
    std::shared_mutex lock_media;

    std::map<std::string, std::string> error_map;
    std::shared_mutex lock_error;

    public:

        Ressource_Manager(Ressource_Manager_Config conf): config(conf)
        {
            init_match_all();
            init_error_routes();
        }

        std::string_view request_ressource(const std::string& path)
        {
            std::shared_lock lock(lock_ressources);

            auto it = ressource_map.find(path);
            if(it != ressource_map.end())
                return it->second;

            return {};
        }

        std::string_view request_error_page(const std::string& code)
        {
            std::shared_lock lock(lock_error);
            auto it = error_map.find(code + ".html"); // e.g 500.html
            if(it != error_map.end())
                return it->second;
            return {};
        }

        std::string_view request_or_fallback(const std::string& path)
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


    private:

        void init_match_all()
        {
            fs::path site_path = config.serve_path;
            if (site_path.filename() != "site") {
                throw std::runtime_error("Invalid serve_path: must point to /site directory");
            }
            fs::path browser_path = site_path / "browser";
            if (!fs::exists(browser_path) ||
                !fs::is_directory(browser_path)) {
                throw std::runtime_error("/site/browser directory missing");
            }
            for (const auto& ent : fs::recursive_directory_iterator(browser_path)) {
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

                if (ext == ".html" || ext == ".js" || ext == ".css" || ext == ".map") {
                    ressource_map[path] = resource;
                }
                else if (ext == ".png" || ext == ".jpg" || ext == ".svg") {
                    images_map[path] = resource;
                }
                else if (ent.file_size() / 1024 / 1024 < 100) {
                    media_map[path] = resource;
                }
            }
        }

        void init_error_routes()
        {
            fs::path site_path = config.serve_path;
            if (site_path.filename() != "site") {
                throw std::runtime_error("Invalid serve_path: must point to /site directory");
            }
            fs::path browser_path = site_path / "error";
            if (!fs::exists(browser_path) ||
                !fs::is_directory(browser_path)) {
                    return;
            }
            for (const auto& ent : fs::recursive_directory_iterator(browser_path)) {
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

                if (ext == ".html") {
                    error_map[path] = resource;
                }
            }
        }
};
