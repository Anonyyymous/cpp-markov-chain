#ifndef CLIENT_H
#define CLIENT_H

#include<networking.hpp>
#include<arpa/inet.h>

class Client {
    public: 
        Client();
        int Connect(std::string destination, std::string ip, int port);
};

#endif