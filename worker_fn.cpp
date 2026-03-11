
#include <algorithm>
#include <memory>
#include <sstream>

#define LE "\r\n"

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
