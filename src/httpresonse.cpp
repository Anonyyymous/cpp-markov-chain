#include<httpresponse.hpp>



HTTPResponse::HTTPResponse(int status_code, std::map<std::string, std::string> headers, std::string result) {
    // handle status line
    contents = "HTTP/1.1 " + std::to_string(status_code) + GetStatusString(status_code) + "\n";

    // handle headers
    for(const auto &pair : headers) {
        contents += pair.first + ": " + pair.second + "\n";
    }
    contents += "\n";

    // handle body
    contents += result;
}
std::string HTTPResponse::GetStatusString(int status_code) {
    switch(status_code) {
        case 200: {
            return " OK";
        }

        // redirection
        case 301: {
            return " permenant redirect";
        } case 302: {
            return " temporary redirect";
        } case 304: {
            return " not modified";
        }

        // client error
        case 401: {
            return " unauthorized error";
        } case 403: {
            return " forbidden";
        } case 404: {
            return " not found";
        } case 405: {
            return " method not found";
        }

        // server error
        case 501: {
            return " not implemented";
        } case 502: {
            return " bad gateway";
        } case 503: {
            return " service unavaliable";
        } case 504: {
            return " gateway timeout";
        } default: {
            return " unsure";
        }
    }
}