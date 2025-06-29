#ifndef NCHAIN_H
#define NCHAIN_H

#include<vector>
#include<map>
#include<string>

class NChain {
    public:
        int length;
        bool debug = false;
        NChain(int length);
        NChain(int length, int default_soft_limit, int default_hard_limit);
        bool Train(std::string filepath);
        bool TrainDirectory(std::string path);
        bool HasContext(std::string context);
        std::string Regurgitate(std::string input);
        std::string Regurgitate(std::string input, int soft_limit, int hard_limit);
        std::string Regurgitate(std::string input, int soft_limit, int hard_limit, int* words_used);
        void DisplayDetails();
        bool ChangeOption(std::string input);

        bool SaveChain(std::string filepath);
        bool LoadChain(std::string filepath);
    private:
        int default_soft_limit, default_hard_limit;

        // negative, since previous versions saved the length first, and this cannot be negative, so older models cannot be misinterpreted as newer
        int save_format_version = -1;
        std::string path = "";
        int InitialiseWordBuffer(std::string input, std::vector<std::string>* word_buffer);
        std::string PickWord(std::string context);
        std::string AddWord(std::string context, std::string newWord);
        std::map<std::string, std::vector<std::string>> usedWords_;
};

NChain* LoadChain(std::string filepath);
NChain* ParseChain();

#endif