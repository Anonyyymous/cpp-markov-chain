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
    NChain* chain = new NChain(2, 4, 5);
    chain->debug = true;
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

    pid_t pid = fork();
    if (pid == 0) {
        const int result = std::system("./run-server -c ../tests/server.conf q");
        std::cout << "server closed" << std::endl;
        return 0;
    }

    const int success = 5;
    cout << "server running on pid: " << pid << std::endl;
    const int python_result = std::system("python3.10 ../tests/testapi.py") >> 8;

    std::cout << "python test status code: " << to_string(python_result) << std::endl;

    if(python_result == success) {
        result = 1;
        std::cout << "--tests successful--" << std::endl;
    } else 
        std::cout << "--tests unsuccessful--" << std::endl;


    //kill(pid, SIGTERM);

    /* std::cout << "gpid: " << to_string((int) getpgid(getpid())) << std::endl;
    std::cout << "stuff killed: " << kill(pid, SIGKILL) << std::endl;
    waitpid(pid, nullptr, 0); */

    // using the pid of the server just didnt work for some reason
    kill(0, SIGKILL); // will also quit program

    return result;
}