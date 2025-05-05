#include<twochain.hpp>
#include<fstream>
#include<iostream>


TwoChain::TwoChain() {
    cout << "initialising two-chain" << endl;
    Initialise();
}
TwoChain::TwoChain(string creation_message) {
    cout << "initialising two-chain" << endl << creation_message << endl;
    Initialise();
}
void TwoChain::Initialise() {
    //words = new vector<Word*>();
    vector<Word*> words;
    map<string, Word*> used_words;
}
bool TwoChain::Train(string filepath) {
    ifstream file(filepath);

    if(!file.is_open()) {
        cout << "couldnt find file: " << filepath << endl;
        return false;
    }

    string text, line;
    //file.read(te, 10);
    while(getline(file, line)) {
        //cout << line << endl;
        text += line + " ";
    }
    file.close();

    Word* lastWord = new Word(""); // empty word, or sentence starter

    int j = 0;
    for(int i = 0; i < text.length(); i++) {
        if(text[i] == ' ') {
            string word = text.substr(j, i-j);
            j = i+1;
            //cout << word << endl;
            Word* res = used_words[word];
            if(res == NULL) {
                /*Word test(word); // TODO
                res = &test;*/
                res = new Word(word);
                //used_words.insert({word, res});
                used_words[word] = res;
                //cout << "making new word: \'" << res->word << "\' after tryign to get " << used_words[word] << endl;
            }
            words.push_back(res);
            lastWord->AddWord(res);
            lastWord = res;
        }
    }
    /*
    for(auto& p : used_words) {
        cout << p.first << ", " << p.second << endl; 
    }*/
    return true;
}
string TwoChain::Regurgitate(string input) {
    string start;
    int j = 0;
    for(int i = 0; i < input.length(); i++) {
        if(input[i] == ' ')
            j = i+1;
            //cout << start << endl;
    }
    start = input.substr(j, input.length()-j);

    Word* word = used_words[start];
    string res = input;
    if(word == NULL) {
        word = words[rand() % words.size()];
        res = word->word;
        cout << "couldnt start chain with \'" << start << "\' so, starting with: " << res << endl;
    }

    res += ' ';

    for(int i = 0; i < 50; i++) {
        Word* next = word->GetRandomWord();

        if(next == NULL) {
            //next = words[0];
            res += "\n";
            break;
        }

        res += next->word + " ";

        word = next;
    }

    return "regurgitating:\n\n" + res + "...\n\n";
}