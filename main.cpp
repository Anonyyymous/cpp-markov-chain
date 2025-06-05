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

// literally just ignores the first 2 characters of the string, so 's blah' becomes 'blah'
string parse_arg(string* str) {
    return str->substr(2, str->length()-1);
}

int main(int argc, char** argv) {
    //TwoChain* chain = new TwoChain();
    std::srand(std::time(NULL));
    
    NChain* chain = nullptr;
    if(argc > 1) {
        cout << ">" << argv[1] << endl;
    }
    if(argc > 1 && (chain = LoadChain(argv[1])) != nullptr)
        cout << "model loaded successfully" << endl;

    std::srand(std::time({}));

    string inp = "";

    do {
        cout << ">>";
        getline(cin, inp);

        // load
        if(inp.length() >= 3 && inp[0] == 'l' && inp[1] == ' ')
            chain = LoadChain(parse_arg(&inp));

        // make new chain
        else if(inp.length() >= 1 && inp[0] == 'n') {
            chain = ParseChain();

            // check if we have a filepath supplied, for some initial training data
            if(inp.length() >= 3 && inp[1] == ' ')
                chain->TrainDirectory(parse_arg(&inp));
        // quit
        } else if(inp.length() == 1 && inp[0] == 'q')
            break;
        else if(chain != nullptr) {

            // save model
            if(inp.length() >= 1 && inp[0] == 's') {
                if(inp.length() == 1)
                    chain->SaveChain("");
                else 
                    chain->SaveChain(parse_arg(&inp));

            // train model
            } else if(inp.length() >= 3 && inp[0] == 't' && inp[1] == ' ') {
                string training_filepath = parse_arg(&inp);
                cout << training_filepath << endl;
                chain->TrainDirectory(training_filepath);

            // display details about the current chain
            } else if(inp.length() == 1 && inp[0] == 'd')
                chain->DisplayDetails();

            // force regurgitate (ignores special characters)
            else if(inp.length() >= 1 && inp[0] == '>')
                cout << endl << chain->Regurgitate(inp.substr(1, inp.length()-1)) << endl;
            
            // change option
            else if (inp.length() >= 3 && inp[0] == 'c' && inp[1] == ' ')
                chain->ChangeOption(inp);
            
            // assume they wanted to regurgitate anyway
            else
                cout << endl << chain->Regurgitate(inp) << endl;

        } else {
            cout << "cannot perform command; chain is null" << endl;
        }
    } while(inp != "stop");
    return 0;

}