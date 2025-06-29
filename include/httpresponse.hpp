#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include<string>
#include<map>
#include<httprequest.hpp>

class HTTPResponse {
    public:
        std::string contents;
        HTTPResponse(int status_code, std::map<std::string, std::string> headers, std::string result);
    private:
        std::string GetStatusString(int status_code);

};

#endif