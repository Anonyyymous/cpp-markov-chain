#include<filesystem>
#include<iostream>
#include<nchain.hpp>
#include<httpserver.hpp>
#include<cstdlib>
#include<signal.h>


using namespace std;

int main() {
    // delete file if it exists, then train a new model on the input
    std::cout << "starting tests" << std::endl;

    // make a way to train/save the model in the command line
    // do that
    NChain* chain = new NChain(2, 5, 15);
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

    string regurgitation = chain->Regurgitate("Tell him");
    if(regurgitation == "Tell him to go eat shit, Johnny." || regurgitation == "Tell him yourself.")  
        std::cout << "Initial test complete" << std::endl;
    else {
        std::cout << "'" << regurgitation << "' - test failed" << std::endl;
        return -1;
    }

    std::cout << std::endl << "--Base tests complete--" << std::endl << std::endl;


    // run the server
    int result = -1;
    int server_pid = 0;
    pid_t pid = fork();
    if (pid == 0) {
        const int result = std::system("./run-server -c ../tests/server.conf q");
    } else {
        const int success = 5;
        cout << "server running on pid: " << pid << std::endl;
        const int python_result = std::system("python3.10 ../tests/testapi.py") >> 8;

        std::cout << "python test status code: " << to_string(python_result) << std::endl;

        if(python_result == success) {
            std::cout << "python tests successful" << std::endl;
            result = 1;
        } else {
            std::cout << "python tests unsuccessful" << std::endl;
            
        }
        kill(pid, SIGTERM);
        std::cout << "server killed..." << std::endl;
    }

    if(result == 1)
        std::cout << "--tests successful--" << std::endl;
    else
        std::cout << "--tests unsuccessful--" << std::endl;

    return result;
}