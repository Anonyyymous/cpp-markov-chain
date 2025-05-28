#include<iostream>
#include<ctime>
#include<nchain.hpp>

using namespace std;

NChain* ParseChain() {
    cout << "parsing chain" << endl;
    int length;
    cout << "enter context size: " << endl;
    cin >> length;
    if(length <= 0)
        length = 2;

    // clearing input buffer, idk how eles
    while(getchar() != '\n');

    return new NChain(length);
}

int main(int argc, char** argv) {
    //TwoChain* chain = new TwoChain();
    std::srand(std::time(NULL));
    
    NChain* chain = nullptr;
    if(argc > 1) {
        cout << argv[1] << endl;
    }
    if(argc > 1 && chain->LoadChain(argv[1]))
        cout << "model loaded successfully" << endl;

    std::srand(std::time({}));

    string inp = "";
    cout << ">>";
    //cin >> inp;
    getline(cin, inp);

    while(inp != "stop") {
        if(inp.length() >= 3 && inp[0] == 'l' && inp[1] == ' ')
            chain = LoadChain(inp.substr(2, inp.length()-1));
        else if(inp.length() >= 1 && inp[0] == 'n') {
            chain = ParseChain();

            // check if we have a filepath supplied, for some initial training data
        } else if(inp.length() == 1 && inp[0] == 'q')
            break;
        else if(chain != nullptr) {
            if(inp.length() >= 3 && inp[0] == 's' && inp[1] == ' ')
                chain->SaveChain(inp.substr(2, inp.length()-1));
            else if(inp.length() >= 3 && inp[0] == 't' && inp[1] == ' ') {
                string training_filepath = inp.substr(2, inp.size()-1);
                cout << training_filepath << endl;
                chain->TrainDirectory(training_filepath);
            } else
                cout << endl << chain->Regurgitate(inp, 50, 100) << endl;
        } else {
            cout << "cannot perform command; chain is null" << endl;
        }
        cout << ">>";
        getline(cin, inp);
    }
    return 0;

}