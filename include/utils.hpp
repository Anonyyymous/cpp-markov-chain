#ifndef UTILS_H
#define UTILS_H
#include<nchain.hpp>
#include<iostream>

NChain* ParseChain();
//bool SaveChain(string filepath, NChain* chain);
NChain* LoadChain(string filepath);

#endif