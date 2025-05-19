#include<utils.hpp>

#include<fstream>
#include<iostream>
#include<filesystem>

using namespace std;

map<string, NChain*> models;

NChain* ParseChain() {
    cout << "parsing chain" << endl;
    int length;
    cout << "enter context size: " << endl;
    cin >> length;
    if(length <= 0)
        length = 2;

    // clearing input buffer, idk how eles
    while(getchar() != '\n');

    return new NChain(length);
}
NChain* LoadChain(string filepath) { // TODO remove?
    NChain* chain = new NChain(-1, -1);
    if(chain->LoadChain(filepath)) {
        chain->DisplayDetails();
        return chain;
    }
    return nullptr;
}
bool MakeNewChain(NChain* chain) {
    chain = ParseChain();
    chain->DisplayDetails();

    string training_filepath = "../training-data/";

    std::srand(std::time(nullptr));

    //cout << "hello world2" << endl;
    return chain->TrainDirectory(training_filepath);
}
int RunTerminalModel(int argc, char** argv) {
    NChain* chain = nullptr;
    if(argc > 1) {
        cout << argv[1] << endl;
    }
    if(argc > 1 && chain->LoadChain(argv[1]))
        cout << "model loaded successfully" << endl;

    std::srand(std::time({}));

    string inp = "";
    cout << ">>";
    //cin >> inp;
    getline(cin, inp);

    while(inp != "stop") {
        if(inp.length() >= 3 && inp[0] == 'l' && inp[1] == ' ')
            chain = LoadChain(inp.substr(2, inp.length()-1));
        else if(inp.length() >= 1 && inp[0] == 'n') {
            chain = ParseChain();

            // check if we have a filepath supplied, for some initial training data
        } else if(inp.length() == 1 && inp[0] == 'q')
            break;
        else if(chain != nullptr) {
            if(inp.length() >= 3 && inp[0] == 's' && inp[1] == ' ')
                chain->SaveChain(inp.substr(2, inp.length()-1));
            else if(inp.length() >= 3 && inp[0] == 't' && inp[1] == ' ') {
                string training_filepath = inp.substr(2, inp.size()-1);
                cout << training_filepath << endl;
                chain->TrainDirectory(training_filepath);
            } else
                cout << endl << chain->Regurgitate(inp, 50, 100) << endl;
        } else {
            cout << "cannot perform command; chain is null" << endl;
        }
        cout << ">>";
        getline(cin, inp);
    }
    return 0;
}
HTTPResponse TestWebsite(HTTPRequest request) {
    map<string, string> headers;
    headers["Accept-Ranges"] = "bytes";
    headers["Content-Type"] = "text/html; charset=UTF-8";
    return HTTPResponse(200, headers, "<h1>jorked</h1>\n<p>penis</p>");
}
HTTPResponse TestChain(HTTPRequest request) {
    map<string, string> headers;
    headers["Accept-Ranges"] = "bytes";
    //NChain* chain = LoadChain()
    NChain* chain = new NChain(2, 50);
    chain->TrainDirectory("../training-data/");
    return HTTPResponse(200, headers, chain->Regurgitate(request.requestLine.substr(1, request.requestLine.size()-1), 50, 100));
}

// TODO rewrite myself
void ParseSpaces(string* target) {
    string replace_word = "%20";

    size_t pos = target->find(replace_word);

    while (pos != string::npos) {
        target->replace(pos, replace_word.size(), " ");

        pos = target->find(replace_word, pos + 1);
    }
}
string ConvertToJson(string result, int status_code) {
    return "{\"status\":" + to_string(status_code) + ",\"response\":\"" + result + "\",\"length\":" + to_string(result.size()) + "}";
}
bool CanParseRequestParam(HTTPRequest request, string param_name, int* value) {
    try {
        *value = std::stoi(request.params.at(param_name));
        return true;
    } catch (std::invalid_argument const& e){
        // couldnt convert to string
        return false;
    } catch (std::out_of_range) {
        // one of the limits couldnt be found
        return true;
    }
}
HTTPResponse TestChain2(HTTPRequest request) {
    map<string, string> headers;
    headers["Accept-Ranges"] = "bytes";

    /* if(request.requestedResource == "/")
        return  */

    string result;
    int status_code = 200;
    int i = request.requestLine.size()-1;

    int soft_limit = 50, hard_limit = 100;

    if(!CanParseRequestParam(request, "soft_limit", &soft_limit) || !CanParseRequestParam(request, "hard_limit", &hard_limit))
        return HTTPResponse(400, headers, ConvertToJson("couldnt parse hard/soft limit", 400));

    cout << "hard limit: " << hard_limit << ", soft limit: " << soft_limit << endl;

    // goes to the last '/' in the string, which should be 
    while(request.requestLine[i--] != '/') {
        //cout << request.requestedResource[i];
    }
    cout << endl;

    /* string resource = "../models" + request.requestLine.substr(0, i+1); */
    /* string input = request.requestLine.substr(i+2, request.requestLine.size()-i); */
    string resource = "../models/" + request.params["model"]; // dont want to use .at(), as [] also initialises new values, so these will both default to ""
    string input = request.params["prompt"];
    ParseSpaces(&input);

    cout << "requesting: '" << input << "' from: '" << resource << "'" << endl;
 
    if(!(filesystem::exists(resource) && filesystem::path(resource).extension() == ".jkc")) {
        result = ConvertToJson("", 404);
        status_code = 404;
    } else {
        NChain* model;
        if(models.count(resource) == 0) {
            // load model
            model = LoadChain(resource);
            models[resource] = model;
        }

        model = models[resource];
        if(model == nullptr) {
            status_code = 405;
            //result = "error while loading model";
            result = ConvertToJson("", 405);
        } else {
            result = ConvertToJson(model->Regurgitate(input, 50, 100), 200);
        }
    }
    return HTTPResponse(status_code, headers, result);
}