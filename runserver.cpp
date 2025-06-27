#include<filesystem>
//#include<utils.hpp>
#include<nchain.hpp>
#include<iostream>
#include<httpserver.hpp>
#include<map>
#include<string>
#include<fstream>

using namespace std;

map<string, NChain*> models;

static string model_path;
static int port;
#define DEFAULT_PORT 6678
#define DEFAULT_MODEL_PATH "../models"

string parse_punctuation(string prompt) {
    if(prompt == "")
        return "''";

    size_t pos = prompt.find("%");

    while (pos != string::npos && pos < prompt.size()-2) {
        try {
            int code = stoi(prompt.substr(pos+1,2), nullptr, 16);
            prompt.replace(pos, 3, std::string(1, (char)code) );
        } catch (...) {

        }
        pos = prompt.find("%", pos + 1);
    }
    return prompt;  
}

string convert_to_json(string result, int status_code) {
    return "{\"status\":" + to_string(status_code) + ",\"response\":\"" + result + "\"}";
}
bool can_parse_request_param(HTTPRequest request, string param_name, int* value) {
    try {
        *value = std::stoi(request.params.at(param_name));
        return true;
    } catch (std::invalid_argument const& e){
        // couldnt convert from string
        return false;
    } catch (std::out_of_range) {
        // one of the limits couldnt be found, so use default value downstream
        return true;
    }
}

HTTPResponse process_request(HTTPRequest request, bool quiet) {
    map<string, string> headers;
    headers["Accept-Ranges"] = "bytes";
    headers["Accept"] = "application/vnd.api+json";

    // parse headers/query parameters -----------------------------------------------
    string result;
    int status_code = 200;
    int i = request.requestLine.find("/?")+1;

    int soft_limit = -1, hard_limit = -1;

    cout << "responding to query\n\n";

    if(!quiet) {
        cout << "---------------------------------------------------\nHeaders" << endl;
        cout << request.headers["Host"] << endl;
        
        for(auto it = request.headers.begin(); it != request.headers.end(); ++it) {
            cout << it->first << ": '" << it->second << "'" << endl;
        }

        cout << "---------------------------------------------------\nParams" << endl;

        for(auto it = request.params.begin(); it != request.params.end(); ++it) {
            cout << it->first << ": '" << it->second << "'" << endl;
        }
    }

    if(!can_parse_request_param(request, "soft_limit", &soft_limit) || !can_parse_request_param(request, "hard_limit", &hard_limit))
        return HTTPResponse(400, headers, convert_to_json("couldnt parse hard/soft limit", 400));

    if(!quiet)
        cout << endl;

    // dont want to use .at(), as [] also initialises new values, so these will both default to ""
    string target_model = model_path + request.params["model"];
    string prompt = request.params["prompt"];
    
    // process query -----------------------------------------------
    // check it is a valid file (all models should end with .jkc)
    if(!(filesystem::exists(target_model) && filesystem::path(target_model).extension() == ".jkc")) {
        result = "Model '" + request.params["model"] + "' doesn't exist"; // 404 = resource doesnt exist
        status_code = 404;
    } else {
        NChain* model; // initialise so we can load it
        if(models.count(target_model) == 0) {
            // load model and cache it
            model = LoadChain(target_model);
            models[target_model] = model;
            // if(!quiet)
                cout << "model: " << target_model << " loaded" << endl;
        } else // dont search for it if we already just loaded
            model = models[target_model];


        if(model == nullptr) {
            status_code = 405; // 405 = method (chain) not found/couldnt be loaded correctly
            result = "Model could not be loaded correctly (may be out of date)";
        } else { // finally, a successful query
            //string input = "";
            prompt = parse_punctuation(prompt);

            if(prompt[0] == '\'' && prompt[prompt.size()-1] == '\'') {
                prompt = prompt.substr(1, prompt.length()-2); // gets rid of the ' on either side

                if(!quiet)
                    cout << "requesting: '" << prompt << "' from: '" << target_model << "'" << endl;

                result = model->Regurgitate(prompt);
            } else {
                status_code = 400;
                result = "couldnt parse prompt (" + prompt + "), it should be wrapped in 's. model = " + model_path;
            }
            // result = convert_to_json(model->Regurgitate(input), 200);
        }
    }
    std::cout << "returning " << result << endl;
    return HTTPResponse(status_code, headers, convert_to_json(result, status_code));
}

void make_config(string config_path) {
    // default values
    port = 6678;
    model_path = "../models/";

    /* format - 
    [port no]
    [model path]
    */
    ofstream file(config_path);

    file << to_string(port) << "\n" << model_path;

    file.close();
    cout << "config file created" << endl;
}

void process_config(string config_path) {
    if(filesystem::exists(config_path)) {
        // read from file
        try{
            ifstream file(config_path);

            string line;

            // read/parse port number - may create an error
            getline(file, line);
            port = stoi(line);
            
            // load model path
            getline(file, model_path);

            file.close();

            cout << "config file loaded" << endl;
        } catch (...) {
            // if we couldnt load the file (doesnt exist or poorly formatted), just create a new one at the same place
            cout << "couldnt successfuly read config file... ";
            make_config(config_path);
        }
    } else { // file couldnt be found, so just create the file there for the future
        std::cout << "couldnt find config file" << std::endl;
        port = DEFAULT_PORT;
        model_path = DEFAULT_MODEL_PATH;
    }
}

int main(int argc, char** argv) {
    bool config_loaded = false, quiet = false;

    cout << "server version b\n"; 

    // server params
    for(int i = 1; i < argc; i++) {
        if(argv[i] == std::string("q"))
            quiet = true;
        else if(argv[i] == std::string("-c") && i < argc-1 && !config_loaded) {
            process_config(argv[++i]);
            config_loaded = true;
        }
    }

    //config path will default to the same directory as the program
    if(!config_loaded)
        process_config("server.conf");

    HTTPServer server(port, process_request, quiet);
    return server.StartServer();
}