#ifndef SERVER_H
#define SERVER_H

#include<client.hpp>

class Server {
    public:
        Server(int port_, bool debug = false);
        int port;

        int StartServer();

    protected:
        int serverSocket;
        bool debug;
};

#endif