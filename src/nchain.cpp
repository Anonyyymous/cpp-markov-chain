#include"nchain.hpp"
#include<fstream>
#include<iostream>
#include<map>
#include<algorithm>
#include<utility>
#include<filesystem>

void ShiftLeft(vector<string>* vec);

NChain::NChain(int length, int outputSize, bool debug) {
    Initialise(length, outputSize, debug);
}
NChain::NChain(int length, int outputSize) {
    Initialise(length, outputSize, false);
}
void NChain::Initialise(int length, int outputSize, bool debug) {
    this->length = length;
    this->debug_ = debug;
    this->outputSize = outputSize;
}
void NChain::DisplayDetails() {
    cout << length << "-chain with output of: " << outputSize << endl;
}
string concatVector(vector<string> vec) {
    string res = "";

    if(vec.size() == 0)
        return res;

    for(int i = 0; i < vec.size()-1; i++) 
        if(vec[i] != "")
            res += vec[i] + " ";
    return res + vec[vec.size()-1];
}

void ShiftLeft(vector<string>* vec) {
    for(int i = 0; i < vec->size()-1; i++) {
        vec->at(i) = vec->at(i+1);
    }
    vec->pop_back();
}

void displayVec(vector<string>* vec) {
    cout << "displaying vector of size " << vec->size() << ":";
    for(int i = 0; i < vec->size(); i++) {
        cout << vec->at(i) << " ";
    }
    cout << endl;
}
Word* NChain::AddWord(string context, string new_word) {
    Word* res = new Word(new_word);
    if(usedWords_.count(context) == 0) {
        usedWords_[context] = vector<Word*> {res};
    } else
        usedWords_[context].push_back(res);
    return res;
}
bool NChain::TrainDirectory(string path) {
    cout << "trying to train on directory: " << path << endl;
    for(const auto& entry : filesystem::directory_iterator(path)) {
        if(!Train(entry.path()))
            return false;

        cout << "file: " << entry.path() << " trained" << endl;
    }
    return true;
}
bool NChain::Train(string filepath) {
    vector<string> word_buffer; // TODO maybe use string[]?
    ifstream file(filepath);

    if(!file.is_open()) {
        cout << "couldnt find file for nchain (" << length << "): " << filepath << endl;
        return false;
    }

    string text, line;
    // vector<string> lines;
    while(getline(file, line)) {
        text += line + " ";
        //lines.push_back(line);
    }
    file.close();

    //for(string text : lines) {}
    int j = 0;
    for(int i = 0; i < text.length(); i++) {
        if(text[i] == ' ') {
            string word = text.substr(j, i-j);

            j = i+1;

            if(word_buffer.size() > 0) {
                string context_so_far = word_buffer[word_buffer.size()-1];

                AddWord(context_so_far, word);
                //words_.push_back(res);
                if(debug_)
                    cout << "trained on '" << context_so_far << "'" << endl;
                for(int i = word_buffer.size()-2; i >= 0; i--) {
                    context_so_far = word_buffer[i] + " " + context_so_far;
                    AddWord(context_so_far, word);

                }
            }

            if(word_buffer.size() >= length)
                ShiftLeft(&word_buffer);
            
            word_buffer.push_back(word);
        }
    }
    return true;
}
bool NChain::HasWord(string word) {
    return usedWords_.count(word) > 0;
}
bool NChain::InitialiseWordBuffer(string input, vector<string>* word_buffer) {
    string start;
    int j = 0;
    //if(debug_)
    cout << "processing input: '" << input << "'" << endl;
    for(int i = 0; i < input.length(); i++) {
        if(input[i] == ' ') {
            start = input.substr(j, i-j);
            j = i+1;
            if(!HasWord(start))
                continue;
            if(word_buffer->size() >= length)
                ShiftLeft(word_buffer);

            word_buffer->push_back(start);
            
            cout << "processing word: " << start << endl;
        }
            //cout << start << endl;
    }

    start = input.substr(j, input.length()-j);
    if(!HasWord(start))
        return false; // dont add this to the context
    if(word_buffer->size() >= length)
        ShiftLeft(word_buffer);
    word_buffer->push_back(start);
    cout << "processing final word: " << start <<":" << j << ":" << input.length() << endl;

    return true;
}
Word* NChain::PickWord(string target) {
    if(usedWords_.find(target) == usedWords_.end()) // if its not in the map
    {
        cout << "word: '" << target << "' could not be found" << endl;
        target = " ";

        cout << "word: '" << target << "' could not be found" << endl;
    }
    vector<Word*> words = usedWords_.at(target);
    return words[rand() % words.size()];
}
string NChain::Regurgitate(string input) {
    vector<string> word_buffer(length);
    //fill(word_buffer.begin(), word_buffer.end(), "");
    cout << word_buffer.size() << endl;

    Word* word; // = used_words[word_buffer[length-1]];
    string res = "";
    //InitialiseWordBuffer(input, &word_buffer);

    if(InitialiseWordBuffer(input, &word_buffer))
        res = input;

    string test = concatVector(word_buffer);
    word = PickWord(test);
    cout << "input: '" << test << "'" <<  endl;

    //cout << endl << word_buffer[0] << " " << word_buffer[1] << endl;
    cout << "starting from: '" << word_buffer[length-1] << "' with: " << word->word << " from: " << "'" << test << "'";

    res += ' ';

    for(int i = 0; i < outputSize; i++) {
        string context = concatVector(word_buffer);
        string nxt = "";
        if(usedWords_.count(context) > 0) {
            // if key exists
            word = PickWord(context);
            
        } else
            break;
        nxt = word->word;
        if(word == NULL) {
            //next = words[0];
            res += "\n";
            break;
        }
        ShiftLeft(&word_buffer);
        word_buffer.push_back(nxt);

        res += nxt + " ";
    }

    return "\nregurgitating:\n\n" + res + "...\n\n";
}

bool NChain::SaveChain(string filepath) {
    if(!filesystem::exists(filepath)) {
        cout << "file: '" << filepath << "' does not exist, so creating it first" << endl;
    } else 
        cout << filepath << " exists, about to save to it." << endl;
    ofstream file(filepath, ios::out | ios::binary);

    //file << length;// << outputSize;
    //file << outputSize;
    file.write(reinterpret_cast<char*>(&length), sizeof(int));
    file.write(reinterpret_cast<char*>(&outputSize), sizeof(int));

    // total size
    int temp = usedWords_.size();
    file.write(reinterpret_cast<char*>(&temp), sizeof(int));

    for(auto const& p : usedWords_) {
        // size of string
        temp = p.first.length();
        file.write(reinterpret_cast<char*>(&temp), sizeof(int));
        file << p.first.c_str();

        temp = p.second.size();
        file.write(reinterpret_cast<char*>(&temp), sizeof(int));
        for(auto const& s : p.second) {
            temp = s->word.length();
            file.write(reinterpret_cast<char*>(&temp), sizeof(int));
            file << s->word.c_str();
        }
    }
    //file.write(reinterpret_cast<const char*>(usedWords_[0].size()), sizeof(int));

    cout << "closing file" << endl;
    file.close();

    return true;
}
string read_string(ifstream* file, int size) {
    // cant pass file as value, so have to use pointer
    char* buffer = new char[size+1];
    file->read(buffer, size * sizeof(char));
    buffer[size] = '\0'; // Cpp strings end with this, so otherwise the string is kindof fucked
    // post recommended deleting the buffer before doing more with the code, but since this is a stand-alone function, not sure what to do?
    return buffer;

}
bool NChain::LoadChain(string filepath) {
    if(!filesystem::exists(filepath)) {
        cout << "file: '" << filepath << "' does not exist, couldnt load to it" << endl;
        return false;
    }

    ifstream file(filepath, ios::binary);
    if(!file.is_open()) {
        cout << "couldnt open file :(" << endl;
        return false;
    }
    //file >> length >> outputSize;
    //file >> length;
    length = -1;
    outputSize = -1;


    int size = 0;
    file.read(reinterpret_cast<char*>(&length), sizeof length);
    file.read(reinterpret_cast<char*>(&outputSize), sizeof outputSize);
    file.read(reinterpret_cast<char*>(&size), sizeof(int));
    
    usedWords_ = map<string, vector<Word*>>();
    for(int i = 0; i < size; i++) {
        // size of string
        int string_size = 0;
        file.read(reinterpret_cast<char*>(&string_size), sizeof(int));
        string str = read_string(&file, string_size);

        vector<Word*> words;
        int mapping_count = 0;
        file.read(reinterpret_cast<char*>(&mapping_count), sizeof(int));
        //cout << "read string of size: " << string_size << ", '" << str << "', with " << mapping_count << " mapping(s)" << endl; 
        for(int x = 0; x < mapping_count; x++) {
            int mapping_length = 0;
            file.read(reinterpret_cast<char*>(&mapping_length), sizeof(int));

            string mapping = read_string(&file, mapping_length);
            words.push_back(new Word(mapping));
        }
        usedWords_[str] = words;
    }
    //file.write(reinterpret_cast<const char*>(usedWords_[0].size()), sizeof(int));

    file.close();

    return true;
}