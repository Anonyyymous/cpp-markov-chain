#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<ctime>
#include<map>
//#include"word.h"
#include<utils.hpp>
#include<echoserver.hpp>

using namespace std;

int main(int argc, char** argv) {
    //TwoChain* chain = new TwoChain();

    if(argc > 1) {
        const int port = 6678;
        switch (argv[1][0]) {
            case 's': {
                Server server(port);
                return server.StartServer();
            }
            case 'c': {
                Client client;
                return client.Connect("", port);
            }
            case 'e': {
                EchoServer server(port);
                return server.StartServer();
            }
        }
    }
    return RunTerminalModel(argc, argv);
}