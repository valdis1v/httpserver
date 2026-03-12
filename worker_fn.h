#ifndef WORKER_FN_H
#define WORKER_FN_H

#include <memory>
#include <string>

// HTTP-Antwort erstellen
char* http_ok_response(std::shared_ptr<char[]> resource, int resource_len, int &written);

// 404 Fehler-Antwort erstellen
char* http_generic_error(int &written);

// Struktur für HTTP-Anfragen
struct HttpRequest
{
    std::string requested_resource;  // Angeforderte Ressource (z.B. index.html)
    int response_socket_fd;          // Socket FD für Antwort
};

// Antwort senden
void respond(int socket_fd, char* response, int response_len);

#endif // WORKER_FN_H
