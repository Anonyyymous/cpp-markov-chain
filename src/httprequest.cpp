#include<httprequest.hpp>
#include<iostream>

HTTPRequest::HTTPRequest(int type, const char* resource) : requestLine(resource), requestType(type) {
    std::cout << "creating http request" << std::endl;
}
HTTPRequest::HTTPRequest(const char* contents_) {
    std::string contents(contents_);
    size = contents.size();

    int i = ParseRequestType(contents[0]);
    int length = 0;
    while(contents[i + length++] != '\n');

    // will now be at the end of the line
    // since this line ends with HTTP/1.1, the resource will be the rest of the line
    requestLine = contents.substr(i, length-11); // may need adjusting

    ParseHeadersAndBody(contents, i + length);
    ParseParams();
}
// returns the index at which to start analysing the request from, including a space
int HTTPRequest::ParseRequestType(char first_char) {
    switch (first_char) {
        case 'G': { // get
            requestType = GET;
            return 4;
        } case 'P': {
            requestType = POST;
            return 5;
        }
    }

    return 0;
}
void HTTPRequest::ParseParams() {
    int i = requestLine.size()-1;

    // should be changed somehow to ignore the target URL completely
    while(requestLine[--i] != '/');

    // j = start of a new parameter name, e = index of the equals
    int j = i, e = i;
    bool parsing_name = true; // allows us to use '=' inside a parameter (e.g. var='i=j')
    // TODO forloop?
    while(i <= requestLine.size()) {
        if(!parsing_name && (i == requestLine.size() || requestLine[i] == '&')) {
            std::string param_name = requestLine.substr(j+1, e-j-1);
            if(i == requestLine.size())
                i++; // counteracts the -1 in the length of param_value, as we dont have to skip the & 
            std::string param_value = requestLine.substr(e+1, i-e-1);

            params[param_name] = param_value;
            j = i;
            parsing_name = true;
        } else if(parsing_name && requestLine[i] == '=') {
            e = i;
            parsing_name = false;
        }
        i++;
    }
}
void HTTPRequest::ParseHeadersAndBody(std::string contents, int start) {
    int i = start, j;
    while(contents[i] != '\n' && i <= contents.size()-1) { 
        start = i;
        j = i;
        while(contents[j++] != ':');
        i = j;
        
        while(contents[++i] != '\n');
        std::string fst = contents.substr(start, j-start-1);
        std::string snd = contents.substr(j+1, i - j - 2);
        headers[fst] = snd;

    }
}