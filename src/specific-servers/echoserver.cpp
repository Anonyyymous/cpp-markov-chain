/* #ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include<server.hpp>


class EchoServer : public Server {
    public:
        ~EchoServer();
        int StartServer() {
            // explanations will be in original server.StartServer()
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        
            struct sockaddr_in server_address;
        
            if(serverSocket < 0) {
                std::cout << "error occured trying to create the server socket" << std::endl;
                return serverSocket;
            }
        
            server_address.sin_family = AF_INET; 
            server_address.sin_port = htons(port);
            server_address.sin_addr.s_addr = INADDR_ANY;
        
            bind(serverSocket, (sockaddr*)&server_address, sizeof(server_address));
        
            listen(serverSocket, 5);

            int buffer_size = 1024;
        
            char buffer[buffer_size] = {0};
        
            for(int i = 6; i > 0; i++) {

                int client_socket = accept(serverSocket, nullptr, nullptr);
            
                if(client_socket < 0) {
                    std::cout << "an error occured trying to accept client connections" << std::endl;
                    return client_socket;
                }
            
                recv(client_socket, buffer, sizeof(buffer), 0); // we can use unistd.read/write for these operations, since they both do the same thing
                std::cout << "client message: " << buffer << std::endl;
                send(serverSocket, buffer, buffer_size, 0);

                close(client_socket);
            }
        
            close(serverSocket);
        
            return 0;
        }
};

#endif */