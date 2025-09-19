#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include<string>
#include<map>

// TODO replace with enum
enum RequestType {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

class HTTPRequest {
    public:
        // lot of this is for metadata, if needed
        std::string requestLine, resource, body;
        RequestType requestType;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;

        HTTPRequest(RequestType request_type, const char* request_line);
        HTTPRequest(const char* contents) : HTTPRequest(std::string(contents)) {};
        HTTPRequest(std::string contents);

        std::string FormatToSend();
    
    private:
        void ParseHeadersAndBody(std::string contents, int start);
        void ParseParams(int param_start);
};

HTTPRequest* format_get_for(std::string destination, std::string resource);

std::string request_type_to_string(RequestType type);

RequestType string_to_request_type(char fst, char snd);
#endif