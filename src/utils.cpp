#include<utils.hpp>

#include<fstream>
#include<iostream>
#include<filesystem>

using namespace std;

map<string, NChain*> models;

NChain* ParseChain() {
    cout << "parsing chain" << endl;
    int length, outputSize;
    cout << "enter context size: " << endl;
    cin >> length;
    if(length <= 0)
        length = 2;

    cout << "enter output size: " << endl;
    cin >> outputSize;
    if(outputSize <= 0)
        outputSize = 2;

    // clearing input buffer, idk how eles
    while(getchar() != '\n');

    return new NChain(length, outputSize);
}
NChain* LoadChain(string filepath) { // TODO remove?
    NChain* chain = new NChain(-1, -1);
    if(chain->LoadChain(filepath)) {
        chain->DisplayDetails();
        return chain;
    }
    return NULL;
}
int RunTerminalModel(int argc, char** argv) {
    NChain* chain = new NChain(-1, -1);
    if(argc > 1) {
        cout << argv[1] << endl;
    }
    if(argc > 1 && chain->LoadChain(argv[1])) {
        cout << "model loaded successfully" << endl;
    } else {
        chain = ParseChain();
        chain->DisplayDetails();

        string training_filepath = "../training-data/";

        std::srand(std::time(NULL));

        //cout << "hello world2" << endl;
        if(!chain->TrainDirectory(training_filepath)) {
            cout << "womp womp, training failed" << endl;
            return -1;
        }
    }

    std::srand(std::time({}));

    string inp = "";
    cout << ">>";
    //cin >> inp;
    getline(cin, inp);

    while(inp != "stop") {
        if(inp.length() <= 1)
            cout << chain->Regurgitate(inp) << endl;
        else if(inp[0] == 's' && inp[1] == ' ')
            chain->SaveChain(inp.substr(2, inp.length()-1));
        else if(inp[0] == 'l' && inp[1] == ' ')
            chain = LoadChain(inp.substr(2, inp.length()-1));
        else
            cout << chain->Regurgitate(inp) << endl;
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
    return HTTPResponse(200, headers, chain->Regurgitate(request.requestedResource.substr(1, request.requestedResource.size()-1)));
}

void ParseSpaces(string* target) {
    string replace_word = "%20";

    // Replacement string
    string replace_by = " ";

    // Find the first occurrence of the substring
    size_t pos = target->find(replace_word);

    // Iterate through the string and replace all
    // occurrences
    while (pos != string::npos) {
        // Replace the substring with the specified string
        target->replace(pos, replace_word.size(), replace_by);

        // Find the next occurrence of the substring
        pos = target->find(replace_word,
                         pos + replace_by.size());
    }
}
string ConvertToJson(string result, int status_code) {
    return "{\"status\":" + to_string(status_code) + ",\"response\":\"" + result + "\",\"length\":" + to_string(result.size()) + "}";
}
HTTPResponse TestChain2(HTTPRequest request) {
    map<string, string> headers;
    headers["Accept-Ranges"] = "bytes";

    string result;
    int status_code = 200;
    int i = request.requestedResource.size()-1;

    while(request.requestedResource[i--] != '/') {
        //cout << request.requestedResource[i];
    }
    cout << endl;

    string resource = ".." + request.requestedResource.substr(0, i+1);
    string input = request.requestedResource.substr(i+2, request.requestedResource.size()-i);
    ParseSpaces(&input);

    cout << "requesting: '" << input << "' from: '" << resource << "'" << endl;
 
    if(!filesystem::exists(resource)) {
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
            result = ConvertToJson(model->Regurgitate(input), 200);
        }
    }
    return HTTPResponse(status_code, headers, result);
}