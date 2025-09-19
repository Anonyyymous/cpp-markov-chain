#include<server.hpp>


/// @brief Creates a new server 
/// @param port The port to run on later
/// @param debug Whether or not to print extra information to the terminal
Server::Server(int port, bool debug) : port(port), debug(debug) {
    if(debug)
        std::cout << "server created at port " << port << std::endl;
}

/// @brief Starts a server listening on the given port, that will simply return hello world to any TCP connections
/// @return An int error code
int Server::StartServer() {
    try {
        const ip::tcp ip = ip::tcp::v4();

        io_context io_context;
        ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip, port));

        while (true) {
            ip::tcp::socket socket(io_context);
            acceptor.accept(socket);

            const std::string msg = "hello world\n";

            boost::system::error_code err;
            write(socket, buffer(msg), err);

            if (err) 
                throw new boost::system::system_error(err);

            std::cout << "message written" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "exiting program" << std::endl;
    }
    return 0;
}