#include "word.hpp"
#include<iostream>

Word::Word(string str) {
    word = str;
    words = vector<Word*>();
}
void Word::AddWord(Word* newword) {
    //cout << newword->word << " follows " << word << endl;
    words.push_back(newword);
}
int GetRandom(int max) {
    return rand() % max;
}
Word* Word::GetRandomWord() {
    //cout << "trying to pick a word out of " << words.size() << "," << word << ":" << endl;

    if(words.size() == 0)
        return NULL;

    /*for(Word* wrd : words) {
        cout << wrd->word << ", ";
    }*/
    //cout << endl;
    return words[GetRandom(words.size())];
}
Word* Word::GetWord(string target) {
    for(Word* wrd : words) {
        if(wrd->word == target)
            return wrd;
    }
    return GetRandomWord();
}

Word::operator string&() {
    return ((string)"Word object: ").append(word); // TODo
}