#ifndef UTILS_H
#define UTILS_H

#include<nchain.hpp>
#include<iostream>
#include<httpserver.hpp>

NChain* ParseChain();
//bool SaveChain(string filepath, NChain* chain);
NChain* LoadChain(string filepath);
int RunTerminalModel(int argc, char** argv);
HTTPResponse TestWebsite(HTTPRequest request);
HTTPResponse TestChain(HTTPRequest request);
HTTPResponse TestChain2(HTTPRequest request);

#endif