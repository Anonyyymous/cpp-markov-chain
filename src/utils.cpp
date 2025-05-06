#include<utils.hpp>

#include<fstream>
#include<iostream>
#include<filesystem>

using namespace std;

NChain* ParseChain() {
    cout << "parsing chain" << endl;
    int length, outputSize;
    cout << "enter context size: " << endl;
    cin >> length;
    if(length <= 0)
        length = 2;

    cout << "enter output size: " << endl;
    cin >> outputSize;
    if(outputSize <= 0)
        outputSize = 2;

    // clearing input buffer, idk how eles
    while(getchar() != '\n');

    return new NChain(length, outputSize);
}
NChain* LoadChain(string filepath) { // TODO remove?
    NChain* chain = new NChain(-1, -1);
    if(chain->LoadChain(filepath)) {
        chain->DisplayDetails();
        return chain;
    }
    return NULL;
}
int RunTerminalModel(int argc, char** argv) {
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