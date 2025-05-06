#include<client.hpp>


Client::Client() {
    std::cout << "initialised client" << std::endl;
}
int Client::Connect(std::string ip, int port) {
    std::cout << "trying to connect to port " << port << " and ip: " << ip << std::endl;
    // same as server
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_address;

    if(client_socket < 0) {
        std::cout << "couldnt reserver client socket" << std::endl;
        return  client_socket;
    }

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(port);
    client_address.sin_addr.s_addr = INADDR_ANY;

    connect(client_socket, (sockaddr*) &client_address, sizeof(client_address));

    std::cout << "connected to server with " << client_socket << std::endl;

    std::string greeting = "hello world";

    // send takes a file descriptor, a const char*, and the size of the char array, + some flags
    // just used write, which works on any file descriptor
    if(write(client_socket, greeting.c_str(), greeting.size()) < 0) {
        std::cout << "client sent bad message: " << greeting << std::endl;
    }
    std::cout << "message sent: " << greeting.c_str() << std::endl;
    char buffer[256] = {0};

    if(read(client_socket, &buffer, sizeof(buffer)) < 0) {
        std::cout << "client received bad message: " << greeting << std::endl;
    }
    std::cout << "message received: '" << buffer << "'" << std::endl;

    close(client_socket);

    return 0;
}

/* int main() {
    Client client();

    return 0;
} */