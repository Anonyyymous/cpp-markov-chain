#ifndef NCHAIN_H
#define NCHAIN_H

#include<vector>
#include<map>
#include<string>
#include<word.hpp>

using namespace std;

class NChain {
    public:
        int length;
        NChain(int length, bool debug);
        NChain(int length);
        NChain(int length, int default_soft_limit, int default_hard_limit);
        //NChain(string filepath);
        bool Train(string filepath);
        bool TrainDirectory(string path);
        bool HasWord(string word);
        string Regurgitate(string input);
        string Regurgitate(string input, int soft_limit, int hard_limit);
        string Regurgitate(string input, int soft_limit, int hard_limit, int* words_used);
        void DisplayDetails();
        bool ChangeOption(string input);

        bool SaveChain(string filepath);
        bool LoadChain(string filepath);
    private:
        bool debug = false;
        int default_soft_limit, default_hard_limit;
        const int save_format_version = -1;
        string path = "";
        void Initialise(int length, bool debug, int default_soft_limit, int default_hard_limit);
        bool InitialiseWordBuffer(string input, vector<string>* word_buffer);
        Word* PickWord(string target);
        Word* AddWord(string context, string newWord);
        //vector<Word*> words_;
        map<string, vector<Word*>> usedWords_;
};

NChain* LoadChain(string filepath);

#endif