#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include<server.hpp>


// TODO remove at some point
class EchoServer : public Server {
    public:
        EchoServer(int port_) : Server(port_, true) {
            std::cout << "initialising echo server" << std::endl;
        }
        int StartServer() {
            // explanations will be in original server.StartServer()

            std::cout << "starting echo server" << std::endl;
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        
            struct sockaddr_in server_address;
        
            if(serverSocket < 0) {
                std::cout << "error occured trying to create the server socket" << std::endl;
                return serverSocket;
            }
        
            server_address.sin_family = AF_INET; 
            server_address.sin_port = htons(port);
            server_address.sin_addr.s_addr = INADDR_ANY;
            
            std::cout << "bound" << std::endl;
            bind(serverSocket, (sockaddr*)&server_address, sizeof(server_address));
        
            std::cout << "listening" << std::endl;
            listen(serverSocket, 5);
        
        
            for(int i = 0; i < 6; i++) {
                char buffer[256] = {0};
                std::cout << "waiting for connections" << std::endl;
                int client_socket = accept(serverSocket, nullptr, nullptr);

                std::cout << "accepted client: " << client_socket << std::endl;
                std::cout << "server sock/peer name: " << getsockname(serverSocket, (sockaddr*)&server_address, (socklen_t*) sizeof(server_address)) << ", " 
                    << getpeername(serverSocket, (sockaddr*)&server_address, (socklen_t*) sizeof(server_address)) << std::endl;
            
                if(client_socket < 0) {
                    std::cout << "an error occured trying to accept client connections" << std::endl;
                    return client_socket;
                }
                int res;
                // can use recv/send if we want to use flags, otherwise use unistd.read/write, as they all work based on file descriptors anyway
                res = read(client_socket, buffer, sizeof(buffer));
                if(res < 0) {
                    std::cout << "received bad message: " << res << ":" << buffer << std::endl;
                    close(client_socket);
                    continue;
                }
                std::cout << "client message: '" << buffer << "', " << sizeof(buffer) << std::endl;

                res = write(client_socket, buffer, sizeof(buffer));

                std::cout << "result of sending: " << res << std::endl;
                if(res < 0) {
                    std::cout << "sent bad message: " << res << ":" << buffer << std::endl;
                    close(client_socket);
                    continue;
                }
                std::cout << "reply sent (echo)" << std::endl;

                close(client_socket);
                std::cout << "client closed" << std::endl;
            }
        
            close(serverSocket);
        
            return 0;
        }
};

#endif