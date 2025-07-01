#include<client.hpp>


/// @brief Creates a new client
Client::Client() {
    std::cout << "initialised client" << std::endl;
}

/// @brief Tries to connect to 'destination' on port 'port', and sends 'greeting'
/// @param destination 
/// @param greeting 
/// @param port 
/// @return An int error code
int Client::Connect(std::string destination, std::string greeting, int port) {
    // same as server
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_address;

    if(client_socket < 0) {
        std::cout << "couldnt reserve client socket" << std::endl;
        close(client_socket);
        return  client_socket;
    }

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(port); // server port to connect to
    client_address.sin_addr.s_addr = inet_addr(destination.c_str());

    if(connect(client_socket, (sockaddr*) &client_address, sizeof(client_address)) < 0) {
        std::cout << "couldnt connect" << std::endl;
        return -1;
    }

    // send takes a file descriptor, a const char*, and the size of the char array, + some flags
    // just used write, which works on any file descriptor
    if(write(client_socket, greeting.c_str(), greeting.size()) < 0) {
        std::cout << "client sent bad message: " << greeting << std::endl;
    }
    std::cout << "message sent: '" << greeting << "'" << std::endl;
    char buffer[1028] = {0};

    if(read(client_socket, buffer, sizeof(buffer)) < 0) {
        std::cout << "client received bad message: " << greeting << std::endl;
    }
    std::cout << "message received: '" << buffer << "'" << std::endl;

    close(client_socket);

    return 0;
}