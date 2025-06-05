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


// TODO rewrite myself
void parse_spaces(string* target) {
    string replace_word = "%20";

    size_t pos = target->find(replace_word);

    while (pos != string::npos) {
        target->replace(pos, replace_word.size(), " ");

        pos = target->find(replace_word, pos + 1);
    }
}
string convert_to_json(string result, int status_code) {
    return "{\"status\":" + to_string(status_code) + ",\"response\":\"" + result + "\"}";
}
bool can_parse_request_param(HTTPRequest request, string param_name, int* value) {
    try {
        *value = std::stoi(request.params.at(param_name));
        return true;
    } catch (std::invalid_argument const& e){
        // couldnt convert to string
        return false;
    } catch (std::out_of_range) {
        // one of the limits couldnt be found, so use default value downstream
        return true;
    }
}

HTTPResponse process_request(HTTPRequest request) {
    map<string, string> headers;
    headers["Accept-Ranges"] = "bytes";
    headers["Accept"] = "application/vnd.api+json";

    // parse headers/query parameters -----------------------------------------------
    string result;
    int status_code = 200;
    int i = request.requestLine.size()-1;

    int soft_limit = -1, hard_limit = -1;

    if(!can_parse_request_param(request, "soft_limit", &soft_limit) || !can_parse_request_param(request, "hard_limit", &hard_limit))
        return HTTPResponse(400, headers, convert_to_json("couldnt parse hard/soft limit", 400));

    // goes to the last '/' in the string, which should be 
    while(request.requestLine[i--] != '/');
    cout << endl;

    // dont want to use .at(), as [] also initialises new values, so these will both default to ""
    string resource = model_path + request.params["model"];
    string input = request.params["prompt"].substr(1, request.params["prompt"].length()-2); // gets rid of the ' on either side
    parse_spaces(&input);

    cout << "requesting: '" << input << "' from: '" << resource << "'" << endl;
    
    // process query -----------------------------------------------
    // check it is a valid file (all models should end with .jkc)
    if(!(filesystem::exists(resource) && filesystem::path(resource).extension() == ".jkc")) {
        result = convert_to_json("", 404); // 404 = resource doesnt exist
        status_code = 404;
    } else {
        NChain* model; // initialise so we can load it
        if(models.count(resource) == 0) {
            // load model and cache it
            model = LoadChain(resource);
            models[resource] = model;
            cout << "model: " << resource << " loaded" << endl;
        } else // dont search for it if we already just loaded
            model = models[resource];


        if(model == nullptr) {
            status_code = 405; // 405 = method (chain) not found/couldnt be loaded correctly
            result = convert_to_json("", 405);
        } else { // finally, a successful query
            result = convert_to_json(model->Regurgitate(input, 50, 100), 200);
        }
    }
    return HTTPResponse(status_code, headers, result);
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
        } catch (exception e) {
            // if we couldnt load the file, just create a new one at the same place
            cout << "couldnt successfuly read config file... ";
            make_config(config_path);
        }
    } else // file couldnt be found, so just create the file there for the future
        make_config(config_path);
}

int main(int argc, char** argv) {
    //config path will default to the same directory as the program
    string config_path = "server.conf";
    if(argc > 1)
        config_path = argv[1];

    cout << "config path: " << config_path << endl;

    process_config(config_path);

    HTTPServer server(port, process_request);
    return server.StartServer();
}