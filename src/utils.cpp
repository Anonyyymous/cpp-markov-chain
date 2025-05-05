#include<utils.hpp>

#include<fstream>
#include<iostream>
#include<filesystem>

using namespace std;

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
