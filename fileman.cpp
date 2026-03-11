#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

struct Route
{
    std::string name;
    std::shared_ptr<char[]> ressource; // safe thred
    int res_len;
};

class FileManager
{
public:
    FileManager(const fs::path& init) : _path(init) {
        parse_dir();
    }

    std::shared_ptr<char[]> get_ressource(const std::string& path)
    {
        for (const auto& res : this->routes)
        {
            if (path.compare(res.name) == 0)
            {
                return res.ressource;
            }
        }
        return nullptr;
    }

    void parse_dir()
    {
        std::vector<std::thread> threads;

        for (const auto& entry : fs::directory_iterator(this->_path))
        {
            const auto path = entry.path();
            // TODO: Unterordner mappen.
            if (entry.is_directory()) continue;

            auto name = entry.path().string();

            Route route;
            route.name = name;

            auto read_file_charbuf = [&route]() {
                std::ifstream file(route.name, std::ios::binary | std::ios::ate);
                if (!file) {
                    route.ressource = nullptr;
                    route.res_len = 0;
                    return;
                }
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                route.ressource = std::shared_ptr<char[]>(new char[size + 1]);
                if (!file.read(route.ressource.get(), size))
                {
                    route.ressource = nullptr;
                    route.res_len = 0;
                    return;
                }

                route.ressource[size] = '\0';
                route.res_len = static_cast<int>(size);
            };

            threads.emplace_back(read_file_charbuf);
        }
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

private:
    std::vector<Route> routes;
    std::string _path;
};
