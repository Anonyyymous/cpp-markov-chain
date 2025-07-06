#include<filesystem>
#include<iostream>
#include<nchain.hpp>
#include<httpserver.hpp>
#include<unistd.h>
#include<signal.h>


/// @brief Checks whether the given port is taken
/// @param port The port to check for
/// @return True if the port is free, false otherwise
bool test_port(int port) {
    int file = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    if(file < 0) {
        std::cout << "error occured trying to create the test socket" << std::endl;
        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(file, (sockaddr*)&addr, sizeof(addr));
    close(file);
    return res >= 0;
}

int main() {
    std::cout << "Starting tests - please make sure the server.conf in the tests directory uses port 6678 for testing" << std::endl;

    NChain* chain = new NChain(2, 4, 5);
    std::cout << "Chain created..." << std::endl;

    if(!chain->Train("../tests/testing-data")) {
        std::cout << "Chain training failed..." << std::endl;
        return -1;
    }
    std::cout << "Chain trained..." << std::endl;

    if(!chain->SaveChain("../tests/test-mdl.jkc")) {
        std::cout << "Chain saving failed..." << std::endl;
        return -1;
    }
    std::cout << "Chain saved..." << std::endl;
    std::string prompt = "Tell him";
    bool res = chain->Regurgitate(&prompt);
    if(res && (prompt == "Tell him to go eat shit, Johnny." || prompt == "Tell him yourself."))  
        std::cout << "Initial test complete" << std::endl;
    else {
        std::cout << "'" << prompt << "' - test failed" << std::endl;
        return -1;
    }

    std::cout << std::endl << "--Base tests complete--" << std::endl << std::endl;
    delete chain;

    if(!test_port(6678)) {
        std::cout << "Couldnt get port 6678, another program may be using it. If you used that port recently, give it some time and try again." << std::endl;
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        std::cout << "concerting child fork to instance of run-server..." << std::endl;

        // converts this process into one of run-server, so it can be properly killed
        // (char*) shuts cmake up, as it doesn't like converting a string constant to char* implicitly
        const int result = execvp("./run-server", new char*[4]{(char*) "./run-server", (char*) "-c", (char*) "../tests/server.conf", NULL});
        return -1;
    }

    std::cout << "server running on pid: " << pid << std::endl;
    const int python_result = std::system("python3.10 ../tests/testapi.py") >> 8;

    if(python_result == 0) {
        std::cout << "--tests successful--" << std::endl;
    } else 
        std::cout << "--tests unsuccessful--" << std::endl;

    kill(pid, SIGKILL);
    std::cout << "server killed, exiting program" << std::endl;
    return python_result;
}