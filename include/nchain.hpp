#ifndef NCHAIN_H
#define NCHAIN_H

#include<vector>
#include<map>
#include<string>
#include<word.hpp>

using namespace std;

class NChain {
    public:
        int length, outputSize;
        NChain(int length, int outputSize, bool debug);
        NChain(int length, int outputSize);
        //NChain(string filepath);
        bool Train(string filepath);
        bool TrainDirectory(string path);
        bool HasWord(string word);
        string Regurgitate(string input);
        void DisplayDetails();

        bool SaveChain(string filepath);
        bool LoadChain(string filepath);
    private:
        bool debug_ = false;
        void Initialise(int length, int outputSize, bool debug);
        bool InitialiseWordBuffer(string input, vector<string>* word_buffer);
        Word* PickWord(string target);
        Word* AddWord(string context, string newWord);
        vector<Word*> words_;
        map<string, vector<Word*>> usedWords_;
};

#endif