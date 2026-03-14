#include "http_def.h"

#include <sstream>
#include <string_view>

Method method_from(std::string_view val)
{
    if(val.find("GET") != std::string_view::npos) return Get;
    if(val.find("POST") != std::string_view::npos) return Post;
    if(val.find("PUT") != std::string_view::npos) return Put;
    if(val.find("DELETE") != std::string_view::npos) return Delete;

    return Unset;
}

std::string contenttype_into(ContentType type)
{
    if(type == Html)   return "text/html";
    if(type == Script) return "text/javascript";
    if(type == Css)    return "text/css";
    if(type == Json)   return "application/json";

    return "*/*";
}

HttpResponse HttpResponse::OK(ContentType Type, std::string content)
{
    HttpResponse resp;
    resp.body = content;
    resp.content_length = content.size();
    resp.status = "HTTP/1.1 200 OK";
    resp.content_type = contenttype_into(Type);
    return resp;
}

HttpResponse HttpResponse::NotFound(ContentType Type, std::string content)
{
    HttpResponse resp;
    resp.body = content;
    resp.content_length = content.size();
    resp.status = "HTTP/1.1 404 NOT FOUND";
    resp.content_type = contenttype_into(Type);
    return resp;
}

HttpResponse HttpResponse::IntServerError(ContentType Type, std::string content)
{
    HttpResponse resp;
    resp.body = content;
    resp.content_length = content.size();
    resp.status = "HTTP/1.1 500 Internal Server Error";
    resp.content_type = contenttype_into(Type);
    return resp;
}

std::string HttpResponse::into_writable()
{
    std::ostringstream stream1;

    stream1 << status << "\r\n";
    stream1 << "Content-Length: " << content_length << "\r\n";
    stream1 << "Content-Type: " << content_type << "\r\n";
    stream1 << "\r\n";
    stream1 << body;

    return stream1.str();
}

HttpRequest::HttpRequest() {}

HttpRequest HttpRequest::from(const char* buffer)
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
