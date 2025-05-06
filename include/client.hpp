#ifndef CLIENT_H
#define CLIENT_H

#include<networking.hpp>

class Client {
    public: 
        Client();
        int Connect(std::string ip, int port);
};

#endif