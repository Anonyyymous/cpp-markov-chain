#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include<string>
#include<map>

const int POST = 0;
const int GET = 1;

class HTTPRequest {
    public:
        // lot of this is for metadata, if needed
        std::string requestLine, body;
        int requestType, size;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;

        HTTPRequest(int type, const char* resource);
        HTTPRequest(const char* contents);
    
    private:
        // returns the index to start analysing the rest of the request from
        int ParseRequestType(char first_char);
        void ParseHeadersAndBody(std::string contents, int start);
        void ParseParams();
};

#endif