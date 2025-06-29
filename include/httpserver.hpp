#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include<server.hpp>
#include<httprequest.hpp>
#include<httpresponse.hpp>


class HTTPServer : public Server {
    public:
        HTTPServer(int port, HTTPResponse (*consumer)(HTTPRequest, bool), bool debug=false);
        int StartServer();
    private:
        HTTPResponse (*consumer)(HTTPRequest request, bool debug);
};

#endif