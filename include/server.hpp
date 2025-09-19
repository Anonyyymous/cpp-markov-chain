#ifndef SERVER_H
#define SERVER_H

#include<boost/asio.hpp>
#include<iostream>
using namespace boost::asio;

class Server {
    public:
        Server(int port_, bool debug = false);
        int port;

        int StartServer();

    protected:
        bool debug;
};

#endif