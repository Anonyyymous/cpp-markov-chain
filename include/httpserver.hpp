#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include<server.hpp>
#include<httprequest.hpp>
#include<httpresponse.hpp>


class HTTPServer : public Server {
    public:
        HTTPServer(int port, HTTPResponse (*consumer)(HTTPRequest)) : Server(port, true), consumer(consumer) {
            std::cout << "initialising http server with port" << port << std::endl;
        }
        int StartServer() {
            // explanations will be in original server.StartServer()

            std::cout << "starting http server" << std::endl;
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
            listen(serverSocket, 2);
        
        
            for(int i = 0; i < 6; i++) {
                char buffer[4096] = {0};
                std::cout << "waiting for connections" << std::endl;
                int client_socket = accept(serverSocket, nullptr, nullptr);

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
                HTTPRequest req(buffer);
                HTTPResponse response = consumer(req);
                std::cout << std::endl; 
                
                res = send(client_socket, response.contents.c_str(), response.contents.size(), 0);

                std::cout << "result of sending: " << res << std::endl;
                if(res < 0) {
                    std::cout << "sent bad message: " << res << ":" << buffer << std::endl;
                    close(client_socket);
                    continue;
                }

                close(client_socket);
                std::cout << "client closed" << std::endl;
            }
        
            close(serverSocket);
        
            return 0;
        }
    private:
        HTTPResponse (*consumer)(HTTPRequest input);
};

#endif