#include<nchain.hpp>
#include<fstream>
#include<iostream>
#include<map>
#include<algorithm>
#include<utility>
#include<filesystem>

/// @brief Creates a new chain with the given length. Debug defaults to false
/// @param length The length of the new chain
NChain::NChain(int length) : length(length), default_soft_limit(50), default_hard_limit(100), debug(false) {
    //
}

/// @brief Creates a new chain with the given length, default soft limit, and default hard limit. Debug defaults to false
/// @param length 
/// @param default_soft_limit 
/// @param default_hard_limit 
NChain::NChain(int length, int default_soft_limit, int default_hard_limit) : length(length), 
    default_soft_limit(default_soft_limit), default_hard_limit(default_hard_limit), debug(false) {
    //
}

/// @brief Displays the details of the chain, including it's context length, soft/hard limits, and the saved path, if applicable
void NChain::DisplayDetails() {
    std::cout << length << "-chain with an output count of: " << usedWords_.size() << std::endl << "soft_limit: " << default_soft_limit << ", hard_limit: " << default_hard_limit << std::endl;
    if(path != "")
        std::cout << "path: " << path << std::endl;
}

/// @brief Concatenates each eleement in a std::string std::vector together
/// @param vec The std::vector to iterate through
/// @param max_index The maximum element to concatenate up to
/// @return The result of std::string combinations
std::string concat_vector(std::vector<std::string>* vec, int max_index) {
    std::string res = "";
    if(max_index < 0)
        max_index = 0;

    max_index = std::min(max_index, (int) vec->size());

    if(vec->size() == 0)
        return res;

    for(int i = 0; i < max_index-1; i++) 
        if(vec->at(i) != "" && vec->at(i+1) != "")
            res += vec->at(i) + " ";
            
    return res + vec->at(max_index-1);
}

/// @brief Adds a mapping between some context and the next word
/// @param context The context of the word
/// @param new_word The word to map to
/// @return The result of the addition
std::string NChain::AddWord(std::string context, std::string new_word) {
    if(!HasContext(context)) {
        usedWords_[context] = std::vector<std::string> {new_word};
    } else
        usedWords_[context].push_back(new_word);
    return new_word;
}

/// @brief Executes the Train function on the entire directory
/// @param path The directory root to iterate over
/// @return True if it was successful, false otherwise
bool NChain::TrainDirectory(std::string path) {
    if(!std::filesystem::exists(path)) {
        std::cout << "path not found: " << path << std::endl;
        return false;
    }
    std::cout << "trying to train on directory: " << path << std::endl;
    if(!std::filesystem::is_directory(path))
        return Train(path);

    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(!Train(entry.path()))
            return false;

        std::cout << "file: " << entry.path() << " trained" << std::endl;
    }
    return true;
}

/// @brief Adds mappings between word contexts and words, for each line in a file
/// @param filepath The path of the file to read
/// @return True if the training was a success, false otherwise
bool NChain::Train(const std::string filepath) {
    if(!std::filesystem::exists(filepath)) {
        std::cout << "Unfortunately, the file " << filepath << " could not be found" << std::endl;
        return false;
    }
    std::vector<std::string> word_buffer;
    word_buffer.reserve(length);
    std::ifstream file(filepath);

    if(!file.is_open()) {
        std::cout << "couldnt find file for nchain (" << length << "): " << filepath << std::endl;
        return false;
    }

    std::string line;
    while(std::getline(file, line)) {
        if(line == "\n")
            continue;
        int j = 0;
        word_buffer.clear();

        for(int i = 0; i <= line.length(); i++) {
            if(line[i] != ' ' && i != line.length())
                continue;

            std::string word = line.substr(j, i-j);

            j = i+1;

            // dont want to include the space in the first one
            std::string context_so_far = "";
            if(word_buffer.size() > 0)
                context_so_far = word_buffer[word_buffer.size()-1];

            AddWord(context_so_far, word);

            if(debug)
                std::cout << "trained on '" << context_so_far << "' -> '" << word << "'" << std::endl;

            for(int i = word_buffer.size()-2; i >= 0; i--) {
                context_so_far = word_buffer[i] + " " + context_so_far;
                AddWord(context_so_far, word);

                if(debug)
                    std::cout << "trained on '" << context_so_far << "' -> '" << word << "'" << std::endl;
            }

            if(word_buffer.size() >= length)
                word_buffer.erase(word_buffer.begin());
            
            word_buffer.push_back(word);

        }
        // finish off the contents of the word buffer
        while(word_buffer.size() > 1) {
            std::string context = concat_vector(&word_buffer, word_buffer.size()-1), word = word_buffer[word_buffer.size()-1];
            AddWord(context, word);
            word_buffer.erase(word_buffer.begin());
            if(debug)
                std::cout << context << " -> " << word << std::endl;
        }
    }
    file.close();
    return true;
}

/// @brief Checks whether the chain contains a mapping between a context and any words
/// @param context The context to check against
/// @return True if a mapping exist, false otherwise
bool NChain::HasContext(const std::string context) {
    return usedWords_.count(context) > 0;
}

/// @brief Fills the given word_buffer, and returns the number of words in the input
/// @param input The input to process
/// @param word_buffer The buffer to fill
/// @return The number of words in the input
int NChain::InitialiseWordBuffer(std::string input, std::vector<std::string>* word_buffer) {
    std::string start;
    int j = input.length();

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
                std::cout << "processing word: '" << start << "' - " << word_buffer->size() << "/" << length << std::endl;
            
            if(!HasContext(start)) {
                if(debug)
                    std::cout << "dont have word: '" << start << "'" <<  std::endl;
                buffer_filled = true; // if we don't have a word, use the buffer as it is, for some continuity rather than none
                continue;
            }

            word_buffer->insert(word_buffer->begin(), start);
            if(word_buffer->size() >= length)
                buffer_filled = true;
        }
    }
    if(debug)
        std::cout << word_count << " words in the input" << std::endl;
    return word_count;
}

/// @brief Picks a word based on some context
/// @param context The context to pick based on
/// @return A pointer to the word selected, or nullptr if no such mapping exists
std::string NChain::PickWord(std::string context) {
    if(!HasContext(context)) // if its not in the map
    {
        return "";
    }
    std::vector<std::string> words = usedWords_.at(context);
    return words[rand() % words.size()];
}

/// @brief Regurgitates based on some input, using the chain's default soft/hard limits
/// @param input The input to regurgitate from
/// @return The resultant std::string (includes the input)
std::string NChain::Regurgitate(std::string input) {
    return Regurgitate(input, default_soft_limit, default_hard_limit, nullptr);
}

/// @brief Regurgitates based on some input, using the given default soft/hard limits
/// @param input The input to regurgitate from
/// @param soft_limit The soft limit of this output - once the soft limit is reached, if a word ends in a full stop, the chain will end
/// @param hard_limit The hard limit of this output - once the hard limit is reached, the chain stops
/// @return The resultant std::string (includes the input)
std::string NChain::Regurgitate(std::string input, int soft_limit, int hard_limit) {
    return Regurgitate(input, soft_limit, hard_limit, nullptr);
}

/// @brief Returns true if the character given is either a full stop, exclamation mark, or question mark
/// @param inp The character to check against
/// @return True if the character given is either a full stop, exclamation mark, or question mark, else false
bool isTerminator(char inp) {
    const std::vector<char> terminators = {'.', '?', '!'};

    for(char ter : terminators)
        if(inp == ter)
            return true;

    return false;
}

/// @brief Regurgitates based on some input, using the given default soft/hard limits
/// @param input The input to regurgitate from
/// @param soft_limit The soft limit of this output - once the soft limit is reached, if a word ends in a full stop, the chain will end
/// @param hard_limit The hard limit of this output - once the hard limit is reached, the chain stops
/// @param words_used The words contained in the total output, in case this can be used for downstream tasks, such as for a http request
/// @return The resultant std::string (includes the input)
std::string NChain::Regurgitate(std::string input, int soft_limit, int hard_limit, int* words_used) {
    std::vector<std::string> word_buffer;
    word_buffer.reserve(length); // avoids resizing later

    if(soft_limit < 0)
        soft_limit = default_soft_limit;
    if(hard_limit < 0)
        hard_limit = default_hard_limit;

    int i = InitialiseWordBuffer(input, &word_buffer);

    if(debug) {
        std::string initial_context = concat_vector(&word_buffer, word_buffer.size());
        std::cout << "original input: '" << input << "', parsed input: '" << initial_context << "'" <<  std::endl;
    }

    // once we reach the hard limit, stop immediately, else go until a full stop after the soft limit, or end of data is found
    for(i = i; i < hard_limit; i++) {
        std::string context = concat_vector(&word_buffer, word_buffer.size());

        if(debug)
            std::cout << "generated context: '" << context << "' " << word_buffer.size() << "/" << length << std::endl;
        
        std::string word = PickWord(context);

        // if PickWord didnt find anything
        if(word == "") {
            // if we reach the end of the training data
            if(debug)
                std::cout << "reached the end of the training data" << std::endl;
            break;
        }

        input +=  " "; // added after the if statement to remove trailing spaces

        if(word_buffer.size() >= length)
            word_buffer.erase(word_buffer.begin()); // also shifts the array
        word_buffer.push_back(word);

        input += word;
        if(i >= soft_limit && isTerminator(word.at(word.size()-1))) {
            if(debug)
                std::cout << "quitting regurgitation early after full stop was found after soft limit of " << soft_limit << std::endl;
            break;
        }
    }
    // check for hard limit / change the ending depending on whether it was reached
    if(i >= hard_limit && !isTerminator(input[input.size()-1])) {
        if(debug)
            std::cout << "hard limit reached: " << hard_limit << std::endl;
        input += "...";
    }
    if(debug)
        std::cout << "chain of length " << i << " extra words generated" << std::endl;
    if(words_used != nullptr)
        *words_used = i;
    return input;
}

/// @brief Changes an option in the chain. Intended for use in the terminal
/// @param input The std::string containing the details about what to change/how
/// @return True if the change was a success, false otherwise
bool NChain::ChangeOption(std::string input) {
    try {
        switch(input[2]){ // expect 'c '... so skip first 2 characters
            case 's': // soft limit
                default_soft_limit = std::stoi(input.substr(4, input.length() - 4));
                std::cout << "soft limit changed" << std::endl;
                return true;
            case 'h':
                default_hard_limit = std::stoi(input.substr(4, input.length() - 4));
                std::cout << "hard limit changed" << std::endl;
                return true;
            case 'd':
                debug = std::stoi(input.substr(4, input.length() - 4)) != 0;
                std::cout << "debug mode changed to " << std::to_string(debug) << std::endl;
                return true;
            default:
                return false;

        }
    } catch(std::exception ex) {
        std::cout << "could not make change; an error occured." << std::endl;
        return false;
    }
}

/// @brief Checks whether the last characters of filepath match the characters of extension
/// @param filepath The name of the filepath to check against
/// @param extension The extension to check against (does not include a .)
/// @return True if they match, false otherwise
bool is_valid_extension(std::string filepath, std::string extension) {
    if(filepath.size() <= extension.size())
        return false;

    int start = filepath.size() - extension.size();
    for(int i = 0; i < 4; i++) {
        if(extension[i] != filepath[start + i])
            return false;
    }
    return true;
}

/// @brief Shorthand to write to an int file
/// @param file The file to write to
/// @param i The int to write
void write(std::ofstream* file, int i) {
    file->write(reinterpret_cast<char*>(&i), sizeof(int));
}

/// @brief Saves the details of a chain to a given file. If the given filepath doesnt match, it will try and use the saved filepath, if it exists
/// @param filepath The filepath to save to. Must end in '.jkc'
/// @return True if the saving was a success, false otherwise
bool NChain::SaveChain(std::string filepath) {
    if(filepath == "" && path != "") {
        filepath = path;
        std::cout << "file path defaulted to: " << path << std::endl;
    }

    if(!is_valid_extension(filepath, ".jkc")) {
        std::cout << "invalid file extension, should be '.jkc'" << std::endl;
        return false;
    }

    if(!std::filesystem::exists(filepath)) {
        std::cout << "file: '" << filepath << "' does not exist, so creating it first" << std::endl;
    } else 
        std::cout << filepath << " exists, about to save to it." << std::endl;
    std::ofstream file(filepath, std::ios::out | std::ios::binary);
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
            write(&file, s.length());
            file << s.c_str();
        }
    }
    std::cout << "closing file" << std::endl;
    file.close();

    return true;
}

/// @brief Shorthand to read a std::string of a given size from a file
/// @param file The file to read from
/// @param size The size of the std::string (number of bytes to read)
/// @return The std::string, read from the file
std::string read_string(std::ifstream* file, int size) {
    char* buffer = new char[size+1];
    file->read(buffer, size * sizeof(char));
    buffer[size] = '\0'; // Cpp std::strings end with this, so otherwise the std::string is kindof fucked
    // post recommended deleting the buffer before doing more with the code, but since this is a stand-alone function, not sure what to do?
    return buffer;

}

/// @brief Shorthand to read an int from a file
/// @param file The file to read from
/// @return The int that just got read
int read_int(std::ifstream* file) {
    int tmp = 0;
    file->read(reinterpret_cast<char*>(&tmp), sizeof(int));
    return tmp;
}

/// @brief Loads the contents of a file into a chain, setting this chain's default filepath in the process
/// @param filepath The filepath to load from
/// @return True if loading was successful, false otherwise
bool NChain::LoadChain(std::string filepath) {
    if(!std::filesystem::exists(filepath)) {
        std::cout << "file: '" << filepath << "' does not exist, couldnt load to it" << std::endl;
        return false;
    } else if(!is_valid_extension(filepath, ".jkc")) {
        std::cout << "invalid file extension, should be '.jkc'" << std::endl;
        return false;
    }

    debug = false;

    std::ifstream file(filepath, std::ios::binary);
    if(!file.is_open()) {
        std::cout << "couldnt open file :(" << std::endl;
        return false;
    }
    path = filepath;

    // check save version
    if(read_int(&file) != save_format_version) {
        std::cout << "this model is outdated, aborting" << std::endl;
        return false;
    }

    length = read_int(&file);
    // number of registered words
    int size = read_int(&file);
    default_soft_limit = read_int(&file);
    default_hard_limit = read_int(&file);
    
    usedWords_ = std::map<std::string, std::vector<std::string>>();
    for(int i = 0; i < size; i++) {
        std::string str = read_string(&file, read_int(&file));

        std::vector<std::string> words;
        int mapping_count = read_int(&file);
        for(int x = 0; x < mapping_count; x++) {
            std::string mapping = read_string(&file, read_int(&file));
            words.push_back(mapping);
        }
        usedWords_[str] = words;
    }

    file.close();
    std::cout << "model loaded from " << filepath << std::endl;
    DisplayDetails();

    return true;
}

/// @brief Wrapper to load a chain from nothing
/// @param filepath The filepath to load from
/// @return A pointer to the chain
NChain* LoadChain(std::string filepath) {
    NChain* chain = new NChain(-1);
    if(chain->LoadChain(filepath)) {
        return chain;
    }
    return nullptr;
}

/// @brief Creates a chain based on terminal inputs (only takes chain length)
/// @return A pointer to the new chain created
NChain* ParseChain() {
    std::string inp;
    int length;
    while(inp != "q") {
        std::cout << "parsing chain" << std::endl;
        std::cout << "enter context size: " << std::endl;
        try {
            std::getline(std::cin, inp);
            length = stoi(inp);
            if(length <= 0)
                std::cout << "chain lengths must be greater than 0" << std::endl;
            else
                inp = "q";
        } catch (std::exception e) {
            std::cout << "sorry, thats not a valid length. Enter 'q' to quit" << std::endl;
        }
    }

    return new NChain(length);
}