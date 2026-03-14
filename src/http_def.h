#ifndef HTTP_TYPES_H
#define HTTP_TYPES_H

#include <string>
#include <string_view>
#include <cstddef>

enum Method { Get, Post, Put, Delete, Unset };
enum ContentType { All, Html, Script, Json, Css };

Method method_from(std::string_view val);
std::string contenttype_into(ContentType type);

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
    );

    static HttpResponse NotFound(
        ContentType Type = Html,
        std::string content = "<html></html>"
    );

    static HttpResponse IntServerError(
        ContentType Type = Html,
        std::string content = "<html></html>"
    );

    std::string into_writable();
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

    HttpRequest();

    static HttpRequest from(const char* buffer);
};

#endif
