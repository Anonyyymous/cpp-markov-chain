#include<httprequest.hpp>
#include<iostream>

/// @brief Creates a new HTTPRequest, with a given request type (GET, PUT, etc) and request line.
/// More useful for formatting a request to send, where the body/headers can be added later
/// @param request_type 
/// @param request_line 
HTTPRequest::HTTPRequest(RequestType request_type, const char* request_line) : requestLine(request_line), requestType(request_type) {
    std::cout << "creating http request" << std::endl << std::endl << std::endl;
}

/// @brief Creates a new HTTPRequest object from an appropriately formatted string
/// @param contents_ The string to parse
HTTPRequest::HTTPRequest(const char* contents_) {
    std::string contents(contents_);

    int i = 0;// = ParseRequestType(contents[0]);
    while(contents[i++] != ' ');
    requestType = string_to_request_type(contents[0], contents[1]);
    int length = 0;
    while(contents[i + length++] != '\n');

    // will now be at the end of the line
    // since this line ends with HTTP/1.1, the resource will be the rest of the line
    requestLine = contents.substr(i, length-11); // may need adjusting

    int param_start = requestLine.find("/?")+1;
    resource = contents.substr(i, param_start);

    ParseHeadersAndBody(contents, i + length);
    ParseParams(param_start);
    std::cout << std::endl;
}

/// @brief Returns the index at which to start analysing the request from, and parses the requestType of this request, based on the first character
/// @param first_char 
/// @return The number of characters to progress the index by
int HTTPRequest::ParseRequestType(char first_char) {
    switch (first_char) {
        case 'G': {
            requestType = GET;
            return 4;
        } case 'P': {
            requestType = POST;
            return 5;
        }
    }

    return 0;
}

/// @brief Parses the string of HTTP parameters into a std::map
/// @param i The index to start looking through the contents at
void HTTPRequest::ParseParams(int i) {
    // j = start of a new parameter name, e = index of the equals
    int j = i, e = i;
    bool parsing_name = true; // allows us to use '=' inside a parameter (e.g. var='i=j')
    // TODO forloop?
    for(i = i; i <= requestLine.size(); i++) {
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
    }
}

/// @brief Parses the headers and contents of the HTTP request to a std::map and a string respectively
/// @param contents The total http request, still as a string
/// @param start The index to start parsing from
void HTTPRequest::ParseHeadersAndBody(std::string contents, int start) {
    int i = start, j;

    while(i < contents.size()-2 && contents[i] != '\n') { 
        start = i;
        j = i;
        while(contents[j++] != ':');
        i = j;
        
        while(contents[i++] != '\n');
        std::string fst = contents.substr(start, j-start-1);
        std::string snd = contents.substr(j+1, i - j - 2);
        headers[fst] = snd;
    }
    body = contents.substr(i, contents.size()-i);
}

/// @brief Converts a HTTPRequest to an appropriately formatted string
/// @return The string
std::string HTTPRequest::FormatToSend() {
    std::string res = requestLine + "\r\n";
    for(auto it = headers.begin(); it != headers.end(); ++it) {
        res += it->first + ": " + it->second + "\r\n";
    }
    return res + "\r\n";
}

/// @brief Creates the request line and headers for a HTTPRequest to the given destination/subdirectory
/// @param destination The destination address
/// @param resource The resource to query/access
/// @return A pointer to a new HTTPRequest containing this information
HTTPRequest* format_get_for(std::string destination, std::string resource) {
    HTTPRequest* req = new HTTPRequest(GET, ("GET " + resource + " HTTP/1.1").c_str());
    req->resource = resource;

    // add headers
    req->headers["Host"] = destination;
    req->headers["User-Agent"] = "jarkov-client";
    req->headers["Accept-Encoding"] = "gzip, deflate";
    req->headers["Accept"] = "*/*";
    req->headers["Connection"] = "keep-alive";
    
    return req;
}

/// @brief Converts a request type enum to a string corresponding to that type
/// @param type The type to convert
/// @return A string containing the english name of the request type
std::string request_type_to_string(RequestType type) {
    switch(static_cast<int>(type)) {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case DELETE: return "DELETE";

        default: return "GET";
    }
}

/// @brief Uses the first two characters of a request type name to determine which request type it is
/// @param fst The first character to consider
/// @param snd The second character to consider (PUT and POST both start with P, so a 2nd character is required)
/// @return The RequestType
RequestType string_to_request_type(char fst, char snd) {
    switch (fst) {
        case 'G': return GET;
        case 'P': return (snd == 'U') ? PUT : POST;
        default: return DELETE;
    }
}