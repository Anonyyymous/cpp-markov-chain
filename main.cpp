#include<iostream>
#include<ctime>
#include<nchain.hpp>


/// @brief Ignores the first two characters of a string. Useful for parsing instructions
/// @param str The string to slice
/// @return The sliced string
std::string parse_arg(std::string* str) {
    return str->substr(2, str->length()-1);
}

/// @brief Frees a chain if possible. Intended to be used when loading/making a new chain
/// @param chain A pointer to the chain to free
void free_chain(NChain* chain) {
    if(chain != nullptr) 
        free(chain);
}

int main(int argc, char** argv) {
    NChain* chain2 = LoadChain("../tests/test-mdl.jkc");
    std::cout << chain2->Regurgitate("") << std::endl;
    std::cout << chain2->Regurgitate("Tell") << std::endl;
    std::cout << chain2->Regurgitate("Tell him to go") << std::endl;


    return 0;

    std::srand(std::time(NULL));
    
    NChain* chain = nullptr;
    if(argc == 2 && (chain = LoadChain(argv[1])) != nullptr)
        std::cout << "model loaded successfully" << std::endl;

    std::srand(std::time({}));

    std::string inp = "";

    do {
        std::cout << ">>";
        std::getline(std::cin, inp);

        // load
        if(inp.length() >= 3 && inp[0] == 'l' && inp[1] == ' ') {
            free_chain(chain);
            chain = LoadChain(parse_arg(&inp));
        }

        // make new chain
        else if(inp.length() >= 1 && inp[0] == 'n') {
            free_chain(chain);

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
                std::string training_filepath = parse_arg(&inp);
                std::cout << training_filepath << std::endl;
                chain->TrainDirectory(training_filepath);

            // display details about the current chain
            } else if(inp.length() == 1 && inp[0] == 'd')
                chain->DisplayDetails();

            // force regurgitate (ignores special characters)
            else if(inp.length() >= 1 && inp[0] == '>')
                std::cout << std::endl << chain->Regurgitate(inp.substr(1, inp.length()-1)) << std::endl;
            
            // change option
            else if (inp.length() >= 3 && inp[0] == 'c' && inp[1] == ' ')
                chain->ChangeOption(inp);
            
            // assume they wanted to regurgitate anyway
            else
                std::cout << std::endl << chain->Regurgitate(inp) << std::endl;

        } else {
            std::cout << "cannot perform command; chain is null" << std::endl;
        }
    } while(inp != "stop");
    return 0;

}