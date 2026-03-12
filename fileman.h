#ifndef FILEMAN_H
#define FILEMAN_H

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <thread>

namespace fs = std::filesystem;

struct Route
{
    std::string name;
    std::shared_ptr<char[]> ressource; // Thread-sichere Ressource
    int res_len;
};

class FileManager
{
public:
    FileManager(const fs::path& init);  // Konstruktor: Initialisierung mit einem Pfad
    std::shared_ptr<char[]> get_ressource(const std::string& path, int &written); // Ressource abrufen
    void parse_dir();  // Verzeichnisse und Dateien analysieren

private:
    std::vector<Route> routes;  // Speichert alle Routen
    std::string _path;  // Pfad, der das Verzeichnis angibt
};

#endif // FILEMAN_H
