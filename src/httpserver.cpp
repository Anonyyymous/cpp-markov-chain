#include<httpserver.hpp>

/// @brief Creates a new HTTPServer
/// @param port The port to open the server on later
/// @param consumer A function that takes in a HTTRequest and returns a HTTPResponse
/// @param debug Whether or not the display information to the terminal
HTTPServer::HTTPServer(int port, HTTPResponse (*consumer)(HTTPRequest*, bool), bool debug) : Server(port, debug), consumer(consumer) {
    std::cout << "initialising http server with port " << port << std::endl;
}

/// @brief Starts the HTTPServer, and once it receives a request, passes it into 'consumer'
/// @return 0 if the server closes successfully, -1 otherwise
int HTTPServer::StartServer() {
    std::cout << "starting http server, now using boost/asio" << std::endl;
    try {
        io_context context;
        ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), port));
        acceptor.set_option(socket_base::reuse_address(false)); // stops another program from taking the port while we're waiting for the next request

        while(true) {
            constexpr unsigned int read_size = 512;

            std::array<char, read_size> bfr;
            ip::tcp::socket socket(context);
            if(debug)
                std::cout << "waiting for connections" << std::endl;
            acceptor.accept(socket);

            std::string str;

            size_t byte_count = read_size;

            while (byte_count == read_size) {
                boost::system::error_code ec;
                byte_count = socket.read_some(buffer(bfr), ec);

                if (ec == error::eof)
                    break;
                else if (ec) 
                    throw boost::system::system_error(ec);
                str.append(bfr.data(), byte_count);
            }
            
            // if a client connects to and instantly leaves (like we do to test a port's validity)
            if (str.size() == 0)
                continue;

            if(debug)
                std::cout << str << std::endl;
            
            HTTPRequest* req = new HTTPRequest(str);
            if(debug)
                std::cout << "request constructed" << std::endl;
            HTTPResponse response = consumer(req, debug);
            if(debug)
                std::cout << "response constructed" << std::endl; 
            boost::system::error_code ec;
            
            write(socket, buffer(response.contents), ec);
            if(debug)
                std::cout << "client closed" << std::endl;

        }
    } catch (const boost::system::system_error& e) {
        if (e.code() == boost::asio::error::address_in_use)
            std::cout << "address already in use" << std::endl;
        else
            std::cout << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cout << "An error occured while the server was receiving messages: " << e.what() << std::endl;
    }

    return 0;
}