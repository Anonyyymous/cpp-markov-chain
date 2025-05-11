#include<httprequest.hpp>
#include<iostream>

HTTPRequest::HTTPRequest(int type, const char* resource) : requestedResource(resource), requestType(type) {
    std::cout << "creating http request" << std::endl;
}
HTTPRequest::HTTPRequest(const char* contents_) {
    std::string contents(contents_);
    size = contents.size();
    //std::cout << "parsing http request contents:" << std::endl << contents << std::endl;

    int i = ParseRequestType(contents[0]);
    int length = 0;
    while(contents[i + length++] != '\n');

    // will now be at the end of the line
    // since this line ends with HTTP/1.1, the resource will be the rest of the line
    requestedResource = contents.substr(i, length-11); // may need adjusting
    std::cout << "resource extracted: '" << requestedResource << "'" << std::endl;

    ParseHeadersAndBody(contents, i + length);
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
void HTTPRequest::ParseHeadersAndBody(std::string contents, int start) {
    int i = start, j;
    std::cout << "headers:" << std::endl;
    while(contents[i] != '\n' && i <= contents.size()-1) { 
        start = i;
        j = i;
        while(contents[j++] != ':');
        i = j;
        
        while(contents[++i] != '\n');
        std::string fst = contents.substr(start, j-start-1);
        std::string snd = contents.substr(j+1, i - j - 2);
        std::cout << "'" << fst << "':'" << snd << "'" << std::endl; 
        headers[fst] = snd;

    }
    std::cout << "-----------------------------" << std::endl;
}