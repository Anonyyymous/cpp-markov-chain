#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<ctime>
#include<map>
//#include"word.h"
#include<utils.hpp>

using namespace std;

int main(int argc, char** argv) {
    //TwoChain* chain = new TwoChain();
    NChain* chain = new NChain(-1, -1);
    if(argc > 1) {
        cout << argv[1] << endl;
    }
    if(argc > 1 && chain->LoadChain(argv[1])) {
        cout << "model loaded successfully" << endl;
    } else {
        chain = ParseChain();
        chain->DisplayDetails();

        string training_filepath = "../training-data/";

        std::srand(std::time(NULL));

        //cout << "hello world2" << endl;
        if(!chain->TrainDirectory(training_filepath)) {
            cout << "womp womp, training failed" << endl;
            return -1;
        }
    }

    std::srand(std::time({}));

    string inp = "";
    cout << ">>";
    //cin >> inp;
    getline(cin, inp);

    while(inp != "stop") {
        if(inp.length() <= 1)
            cout << chain->Regurgitate(inp) << endl;
        else if(inp[0] == 's' && inp[1] == ' ')
            chain->SaveChain(inp.substr(2, inp.length()-1));
        else if(inp[0] == 'l' && inp[1] == ' ')
            chain = LoadChain(inp.substr(2, inp.length()-1));
        else
            cout << chain->Regurgitate(inp) << endl;
        cout << ">>";
        getline(cin, inp);
    }
    return 0;
}