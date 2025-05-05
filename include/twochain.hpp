#ifndef TWOCHAIN_H
#define TWOCHAIN_H

#include<vector>
#include<string>
#include<map>
#include<word.hpp>

using namespace std;

class TwoChain {
    public:
        TwoChain();
        TwoChain(string creation_message);
        bool Train(string filepath);
        string Regurgitate(string input);
    private:
        void Initialise();
        int length = 2;
        vector<Word*> words;
        map<string, Word*> used_words;
};

#endif