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
                cout << argc << endl;
                if(argc < 3)
                    return client.Connect("hello world", port);
                string msg(argv[2]);
                return client.Connect(msg, port);
            }
            case 'e': {
                EchoServer server(port);
                return server.StartServer();
            } case 'h': {
                HTTPServer server(port, TestWebsite);
                return server.StartServer();
            } case 'j': {
                HTTPServer server(port, TestChain2);
                return server.StartServer();
            }
        }
    }
    return RunTerminalModel(argc, argv);
}