// https://www.elektronik-kompendium.de/sites/net/0902231.htm


#include <algorithm>
#include <cstdio>
#include <memory>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define LE "\r\n"

/***
 * Return sollte deleted werden
 */
char* http_ok_response(std::shared_ptr<char[]> resource, int resource_len, int &written)
{
    std::ostringstream stream_res;
    stream_res << "HTTP/1.1 200 OK" << LE;
    stream_res << "Content-Type: text/html; charset=UTF-8" << LE;
    stream_res << "Content-Length: " << resource_len << LE; // WICHTIG
    stream_res << LE; // HEADER UND BODY TRENNEN
    stream_res.write(resource.get(), resource_len);

    std::string res = stream_res.str();

    char* response_charbuf = new char[res.length() + 1];
    std::copy(res.begin(), res.end(), response_charbuf);
    response_charbuf[res.length()] = '\0';
    written = res.length();
    return response_charbuf;
}

/***
 * Return sollte deleted werden
 */
char* http_generic_error(int &written)
{
    std::ostringstream stream_res;
    stream_res << "HTTP/1.1 404 NOT FOUND" << LE;
    stream_res << "Content-Type: text/html; charset=UTF-8" << LE;
    stream_res << "Content-Length: 49" << LE;
    stream_res << LE;
    stream_res << "<html><body><h1>404 NOT FOUND</h1></body></html>";

    std::string res = stream_res.str();
    char* response_charbuf = new char[res.length() + 1];
    std::copy(res.begin(), res.end(), response_charbuf);
    response_charbuf[res.length()] = '\0';
    written = res.length();
    return response_charbuf;
}

struct HttpRequest
{
    std::string requested_resource;
    int response_socket_fd;
};

void respond(int socket_fd, char* response, int response_len)
{
    ssize_t sent = send(socket_fd, response, response_len, 0);
    if(sent < 0)
    {
        perror("Failed to send");
        close(socket_fd);
        return;
    }
    close(socket_fd);
    return;
}
