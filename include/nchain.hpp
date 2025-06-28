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
        bool debug = false;
        NChain(int length);
        NChain(int length, int default_soft_limit, int default_hard_limit);
        bool Train(const string filepath);
        bool TrainDirectory(const string path);
        bool HasContext(const string context);
        string Regurgitate(string input);
        string Regurgitate(string input, int soft_limit, int hard_limit);
        string Regurgitate(string input, int soft_limit, int hard_limit, int* words_used);
        void DisplayDetails();
        bool ChangeOption(const string input);

        bool SaveChain(string filepath);
        bool LoadChain(string filepath);
    private:
        int default_soft_limit, default_hard_limit;

        // negative, since previous versions saved the length first, and this cannot be negative, so older models cannot be misinterpreted as newer
        const int save_format_version = -1;
        string path = "";
        int InitialiseWordBuffer(const string input, vector<string>* word_buffer);
        Word* PickWord(const string context);
        Word* AddWord(const string context, const string newWord);
        map<string, vector<Word*>> usedWords_;
};

NChain* LoadChain(string filepath);
NChain* ParseChain();

#endif