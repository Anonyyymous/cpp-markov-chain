#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include<string>
#include<map>

// TODO replace with enum
const int POST = 0;
const int GET = 1;

class HTTPRequest {
    public:
        // lot of this is for metadata, if needed
        std::string requestLine, resource, body;
        int requestType, size;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;

        HTTPRequest(int type, const char* resource);
        HTTPRequest(const char* contents);

        std::string FormatToSend();
    
    private:
        int ParseRequestType(char first_char);
        void ParseHeadersAndBody(std::string contents, int start);
        void ParseParams(int param_start);
};

HTTPRequest* format_request_for(std::string destination, std::string resource);

#endif