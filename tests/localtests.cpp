#include<filesystem>
#include<iostream>
#include<nchain.hpp>
#include<httpserver.hpp>
#include<cstdlib>
#include<signal.h>

int main() {
    std::cout << "starting tests" << std::endl;

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

    std::string regurgitation = chain->Regurgitate("Tell him");
    if(regurgitation == "Tell him to go eat shit, Johnny." || regurgitation == "Tell him yourself.")  
        std::cout << "Initial test complete" << std::endl;
    else {
        std::cout << "'" << regurgitation << "' - test failed" << std::endl;
        return -1;
    }

    std::cout << std::endl << "--Base tests complete--" << std::endl << std::endl;

    pid_t pid = fork();
    if (pid == 0) {
        const int result = std::system("./run-server -c ../tests/server.conf");
        std::cout << "server closed" << std::endl;
        return -1;
    }

    const int success = 5;
    std::cout << "server running on pid: " << pid << std::endl;
    const int python_result = std::system("python3.10 ../tests/testapi.py") >> 8;

    std::cout << "python test status code: " << std::to_string(python_result) << std::endl;

    if(python_result == success) {
        std::cout << "--tests successful--" << std::endl;
    } else 
        std::cout << "--tests unsuccessful--" << std::endl;

    kill(pid, SIGTERM);
    return python_result;
}