#include<httpserver.hpp>

/// @brief Creates a new HTTPServer
/// @param port The port to open the server on later
/// @param consumer A function that takes in a HTTRequest and returns a HTTPResponse
/// @param quiet Whether or not the display information to the terminal
HTTPServer::HTTPServer(int port, HTTPResponse (*consumer)(HTTPRequest, bool), bool quiet) : Server(port, quiet), consumer(consumer) {
    std::cout << "initialising http server with port" << port << std::endl;
}

/// @brief Starts the HTTPServer, and once it receives a request, passes it into 'consumer'
/// @return 0 if the server closes successfully, -1 otherwise
int HTTPServer::StartServer() {
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
    
    if(!quiet)
        std::cout << "bound" << std::endl;
    bind(serverSocket, (sockaddr*)&server_address, sizeof(server_address));

    if(!quiet)
        std::cout << "listening" << std::endl;
    listen(serverSocket, 256); // make space for 256 requestss


    try {
        while(true) {
            char buffer[4096] = {0};
            if(!quiet)
                std::cout << "waiting for connections" << std::endl;
            int client_socket = accept(serverSocket, nullptr, nullptr);

            if(client_socket < 0) {
                std::cout << "bad accept" << std::endl;
                close(client_socket);
                continue;
            }

            int res = read(client_socket, buffer, sizeof(buffer));
            if(client_socket < 0) {
                close(client_socket);
                std::cout << "bad read: " << res << " -> " << buffer << std::endl;
                continue;
            }
            
            if(!quiet)
                std::cout << buffer << std::endl;

            HTTPRequest req(buffer);
            if(!quiet)
                std::cout << "request constructed" << std::endl;
            HTTPResponse response = consumer(req, quiet);
            if(!quiet)
                std::cout << "response constructed" << std::endl; 
            
            res = send(client_socket, response.contents.c_str(), response.contents.size(), 0);

            if(!quiet)
                std::cout << "result of sending: " << res << std::endl;
            if(res < 0) {
                std::cout << "sent bad message: " << res << ":" << buffer << std::endl;
                close(client_socket);
                continue;
            }

            close(client_socket);
            if(!quiet)
                std::cout << "client closed" << std::endl;

        }
    } catch (...) {
        std::cout << "An error occured while the server was receiving messages" << std::endl;
    }
    close(serverSocket);

    return 0;
}