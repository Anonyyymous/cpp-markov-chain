#ifndef SERVER_H
#define SERVER_H

#include<client.hpp>

class Server {
    public:
        Server(int port_, bool quiet = false);
        int port;

        int StartServer();

    protected:
        int serverSocket;
};

#endif