#include<server.hpp>


Server::Server(int port, bool quiet) : port(port), quiet(quiet) {
    if(!quiet)
        std::cout << "server created at port " << port << std::endl;
}

int Server::StartServer() {
    // important to note that sockets (client_socket, serverSocket) are stored as file descriptors.
    // this is also why documentation refers to these as ..._fd
    // file descriptors are basically just keys in a dictionary of information about files, used to interact with them

    // AF_INET ensures we use IPv4, SOCK_STREAM uses TCP, SOCK_DGRAM would so UDP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // sockaddr_in is a sockaddr that is designed for IPv4
    // generally, these just hold useful information
    struct sockaddr_in server_address;

    if(serverSocket < 0) {
        std::cout << "error occured trying to create the server socket" << std::endl;
        return serverSocket;
    }

    server_address.sin_family = AF_INET;

    // Host TO Network
    // htonX converts the native byte order to whatever the network uses (e.g. little endian to big endian) 
    // X changes depending on the type needed, s for short, l for long.
    // also have ntohX (Network TO Host)
    server_address.sin_port = htons(port);
    // associated IP address, the actual value of which is 0, but means that any local ip can be used
    server_address.sin_addr.s_addr = INADDR_ANY;

    // maps the value of serverSocket to the contents of server_address
    bind(serverSocket, (sockaddr*)&server_address, sizeof(server_address));

    // accepts 5 connections before terminating, connections will be queued
    listen(serverSocket, 5);

    char buffer[256] = {0};

    // pops the head of the connection queue, and can take in a sockaddr for extra information (I don't need this extra information)
    // this will wait for an incoming connection if there are currently none, but only if O_NONBLOCK is set, which it is by default. 
    // O_NONBLOCK can be changed from socket(0)
    // the client sockaddr would be used if we wanted to only receive from that client, and we would use it's sockaddr
    int client_socket = accept(serverSocket, nullptr, nullptr);

    if(client_socket < 0) {
        std::cout << "an error occured trying to accept client connections" << std::endl;
        return client_socket;
    }

    // takes the client socket, a buffer, and flags (we can choose to peek data and not read it, or wait until the full message arrives)
    recv(client_socket, buffer, sizeof(buffer), 0); // we can use unistd.read/write for these operations, since they both do the same thing
    
    if(!quiet)
        std::cout << "client message: " << buffer << std::endl;


    close(serverSocket);
    close(client_socket);

    return 0;
}

/* int main() {
    Server server(6789);
    
    return 0;
} */