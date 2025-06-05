#include"nchain.hpp"
#include<fstream>
#include<iostream>
#include<map>
#include<algorithm>
#include<utility>
#include<filesystem>
//#include<algorithm>

void shift_left(vector<string>* vec);

NChain::NChain(int length, bool debug) : length(length), debug(debug) {
    //Initialise(length, debug, 50, 100);
}
NChain::NChain(int length) : length(length), default_soft_limit(50), default_hard_limit(100) {
    //Initialise(length, false, 50, 100);
}
NChain::NChain(int length, int default_soft_limit, int default_hard_limit) : length(length), 
    default_soft_limit(default_soft_limit), default_hard_limit(default_hard_limit) {
    //Initialise(length, false, default_hard_limit, default_soft_limit);
}

// reduntant :(
void NChain::Initialise(int length, bool debug, int default_hard_limit, int default_soft_limit) {
    this->length = length;
    this->debug = debug;
    this->default_hard_limit = default_hard_limit;
    this->default_soft_limit = default_soft_limit;
}
void NChain::DisplayDetails() {
    cout << length << "-chain with an output count of: " << usedWords_.size() << endl << "soft_limit: " << default_soft_limit << ", hard_limit: " << default_hard_limit << endl;
    if(path != "")
        cout << "path: " << path << endl;
}
string concat_vector(vector<string>* vec, int max_index) {
    string res = "";
    if(max_index < 0)
        max_index = 0;
    //if(max_index < vec->size())
    max_index = min(max_index, (int) vec->size());

    if(vec->size() == 0)
        return res;

    for(int i = 0; i < max_index-1; i++) 
        if(vec->at(i) != "")
            res += vec->at(i) + " ";
            
    return res + vec->at(max_index-1);
}

/* void shift_left(vector<string>* vec) {
    for(int i = 0; i < vec->size()-1; i++) {
        vec->at(i) = vec->at(i+1);
    }
}
void shift_right(vector<string>* vec) {
    for(int i = vec->size()-1; i > 0; i--) {
        vec->at(i) = vec->at(i+1);
    }
} */

Word* NChain::AddWord(string context, string new_word) {
    Word* res = new Word(new_word);
    if(usedWords_.count(context) == 0) {
        usedWords_[context] = vector<Word*> {res};
    } else
        usedWords_[context].push_back(res);
    return res;
}
bool NChain::TrainDirectory(string path) {
    if(!filesystem::exists(path)) {
        cout << "path not found: " << path << endl;
        return false;
    }
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
    word_buffer.reserve(length);
    ifstream file(filepath);

    if(!file.is_open()) {
        cout << "couldnt find file for nchain (" << length << "): " << filepath << endl;
        return false;
    }

    // TODO
    string line;
    vector<string> lines;
    while(getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    for(string text : lines) {
        if(text == "\n")
            continue;
        int j = 0;
        word_buffer.clear();

        for(int i = 0; i <= text.length(); i++) {
            if(text[i] == ' ' || i == text.length()) {
                string word = text.substr(j, i-j);

                j = i+1;

                if(word_buffer.size() > 0) {
                    string context_so_far = word_buffer[word_buffer.size()-1];

                    AddWord(context_so_far, word);
                    //words_.push_back(res);
                    if(debug)
                        cout << "trained on '" << context_so_far << "'" << endl;

                    for(int i = word_buffer.size()-2; i >= 0; i--) {
                        context_so_far = word_buffer[i] + " " + context_so_far;
                        AddWord(context_so_far, word);
                    }
                }

                if(word_buffer.size() >= length)
                    word_buffer.erase(word_buffer.begin());
                
                word_buffer.push_back(word);
            }
        }

        // finish off the contents of the word buffer
        while(word_buffer.size() > 1) {
            string context = concat_vector(&word_buffer, word_buffer.size()-1), word = word_buffer[word_buffer.size()-1];
            AddWord(context, word);
            word_buffer.erase(word_buffer.begin());
            cout << context << " -> " << word << endl;
        }
    }
    return true;
}
bool NChain::HasWord(string word) {
    return usedWords_.count(word) > 0;
}

// Takes the input and returns whether the word_buffer was successfully filled, and fills the word buffer with words in our vocabulary from the input
bool NChain::InitialiseWordBuffer(string input, vector<string>* word_buffer) {
    string start;
    int j = input.length();
    cout << "processing input: '" << input << "'" << endl;

    int word_count = 0;
    bool buffer_filled = false;

    // works from the end, which means we can quit early once we reach bad words, or a sufficient amount of good ones
    for(int i = input.length() - 1; i >= -1; i--) {
        if(i == -1 || input[i] == ' ') {
            word_count++;
            if(buffer_filled)
                continue;

            start = input.substr(i+1, j-i-1);
            
            j = i;
            if(debug)
                cout << "processing word: '" << start << "' - " << word_buffer->size() << "/" << length << endl;

            
            if(!HasWord(start)) {
                cout << "dont have word: '" << start << "'" <<  endl;
                break; // if we don't have a word, use the buffer as it is, for some continuity rather than none
            }

            word_buffer->insert(word_buffer->begin(), start);
            if(word_buffer->size() >= length)
                buffer_filled = true;
        }
    }
    cout << word_count << "words in the input" <<endl;
    return true;
}
Word* NChain::PickWord(string target) {
    if(!HasWord(target)) // if its not in the map
    {
        // TODO
        cout << "word: '" << target << "' could not be found" << endl;
        return nullptr;
    }
    vector<Word*> words = usedWords_.at(target);
    return words[rand() % words.size()];
}

// Basically just for the terminal
string NChain::Regurgitate(string input) {
    return Regurgitate(input, default_soft_limit, default_hard_limit, nullptr);
}
string NChain::Regurgitate(string input, int soft_limit, int hard_limit) {
    return Regurgitate(input, soft_limit, hard_limit, nullptr);
}
string NChain::Regurgitate(string input, int soft_limit, int hard_limit, int* words_used) {
    vector<string> word_buffer;
    word_buffer.reserve(length); // avoids resizing later
    if(soft_limit < 0)
        soft_limit = default_soft_limit;
    if(hard_limit <= 0)
        hard_limit = default_hard_limit;

    Word* word; 
    string res = "";

    if(InitialiseWordBuffer(input, &word_buffer))
        res = input + " ";

    string test = concat_vector(&word_buffer, word_buffer.size());
    cout << "input: '" << test << "'" <<  endl;

    int i;
    // once we reach the hard limit, stop immediately, else go until a full stop after the soft limit, or end of data is found
    for(i = 0; i < hard_limit; i++) {
        string context = concat_vector(&word_buffer, word_buffer.size());

        if(debug)
            cout << "generated context: '" << context << "' " << word_buffer.size() << "/" << length << endl;

        string nxt = "";
        
        word = PickWord(context);

        // if PickWord didnt find anything
        if(word == nullptr) {
            // if we reach the end of the training data
            cout << "reached the end of the training data" << endl;
            break;
        }
        nxt = word->word;

        if(word_buffer.size() >= length)
            word_buffer.erase(word_buffer.begin()); // also shifts the array
        word_buffer.push_back(nxt);

        res += nxt;
        if(i > soft_limit && nxt[nxt.size()-1] == '.') {
            cout << "quitting regurgitation early after full stop was found after soft limit of " << soft_limit << endl;
            break;
        }
        res +=  " "; // added after the if statement to remove trailing spaces
    }
    // check for hard limit / change the ending depending on whether it was reached
    if(i == hard_limit) {
        cout << "hard limit reached: " << hard_limit << endl;
        res += "...";
    }
    cout << "chain of length " << i << " extra words generated" << endl;
    if(words_used != nullptr)
        *words_used = i;
    return res;
}

// changes some setting (e.g. default hard/soft limit)
// all of the input is parsed inside this function, including the 'c' that lets users select this
bool NChain::ChangeOption(string input) {
    switch(input[2]){ // expect 'c '... so skip first 2 characters
        case 's': // soft limit
            default_soft_limit = std::stoi(input.substr(4, input.length() - 4));
            cout << "soft limit changed" << endl;
            return true;
        case 'h':
            default_hard_limit = std::stoi(input.substr(4, input.length() - 4));
            cout << "hard limit changed" << endl;
            return true;
        default:
            return false;

    }
}

bool is_valid_extension(string filepath, string extension) {
    if(filepath.size() <= extension.size())
        return false;

    int start = filepath.size() - extension.size();
    for(int i = 0; i < 4; i++) {
        if(extension[i] != filepath[start + i])
            return false;
    }
    return true;
}

// saving integers has to be done this goofy way, for some reason
// writing takes a char* and the buffer size, so just tell the compiler to treat it as such
void write(ofstream* file, int i) {
    file->write(reinterpret_cast<char*>(&i), sizeof(int));
}

bool NChain::SaveChain(string filepath) {
    if(filepath == "" && path != "") {
        filepath = path;
        cout << "file path defaulted to: " << path << endl;
    }

    if(!is_valid_extension(filepath, ".jkc")) {
        cout << "invalid file extension, should be '.jkc'" << endl;
        return false;
    }

    if(!filesystem::exists(filepath)) {
        cout << "file: '" << filepath << "' does not exist, so creating it first" << endl;
    } else 
        cout << filepath << " exists, about to save to it." << endl;
    ofstream file(filepath, ios::out | ios::binary);
    path = filepath;

    write(&file, save_format_version);
    write(&file, length);
    write(&file, usedWords_.size());
    write(&file, default_soft_limit);
    write(&file, default_hard_limit);

    for(auto const& p : usedWords_) {
        write(&file, p.first.length());
        file << p.first.c_str();

        write(&file,p.second.size());
        for(auto const& s : p.second) {
            write(&file, s->word.length());
            file << s->word.c_str();
        }
    }
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
int read_int(ifstream* file) {
    int tmp = 0;
    file->read(reinterpret_cast<char*>(&tmp), sizeof(int));
    return tmp;
}
bool NChain::LoadChain(string filepath) {
    if(!filesystem::exists(filepath)) {
        cout << "file: '" << filepath << "' does not exist, couldnt load to it" << endl;
        return false;
    } else if(!is_valid_extension(filepath, ".jkc")) {
        cout << "invalid file extension, should be '.jkc'" << endl;
        return false;
    }

    ifstream file(filepath, ios::binary);
    if(!file.is_open()) {
        cout << "couldnt open file :(" << endl;
        return false;
    }
    path = filepath;

    // check save version
    if(read_int(&file) != save_format_version) {
        cout << "this model is outdated, aborting" << endl;
        return false;
    }

    length = read_int(&file);
    // number of registered words
    int size = read_int(&file);
    default_soft_limit = read_int(&file);
    default_hard_limit = read_int(&file);
    
    usedWords_ = map<string, vector<Word*>>();
    for(int i = 0; i < size; i++) {
        string str = read_string(&file, read_int(&file));

        vector<Word*> words;
        int mapping_count = read_int(&file);
        for(int x = 0; x < mapping_count; x++) {
            string mapping = read_string(&file, read_int(&file));
            words.push_back(new Word(mapping));
        }
        usedWords_[str] = words;
    }

    file.close();
    cout << "model loaded from " << filepath << endl;
    DisplayDetails();

    return true;
}

NChain* LoadChain(string filepath) { // TODO remove?
    NChain* chain = new NChain(-1, -1);
    if(chain->LoadChain(filepath)) {
        //chain->DisplayDetails();
        return chain;
    }
    return nullptr;
}