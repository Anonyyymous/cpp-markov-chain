#ifndef WORD_H
#define WORD_H

#include<vector>
#include<string>

using namespace std;

class Word {
    public:
        Word(string str);
        Word* GetRandomWord();
        void AddWord(Word* newword);
        Word* GetWord(string target);
        string word;
        operator string&();
    private:
        vector<Word*> words;
};

#endif