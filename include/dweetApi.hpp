#ifndef __DWEET_API__
#define __DWEET_API__




#include <iostream>
#include <string>
#include <sstream>
#include "dweet.h"
using namespace std;

class DweetApi {

public:
     DweetApi() { initialized = false; };
    ~DweetApi() { dweetDisconnect(); };

     void   close() { dweetDisconnect(); };
     void   setup(string thing);
     void   securityCheck(bool enableDisable) {  dweetCurlSecurityCheck(enableDisable ? 1:0); };
     void   chunkedHTTP(bool enableDisable) { dweetChunkedHTTP(enableDisable ? 1:0); };
     string getStatus();
     bool   isConfigured();
     bool   push(string stuff);
     string pull(void);
     string get(string item);  // return the contents of the JSON payload named by item.
     string get(void) { return sJsonRespStr; };  // return  JSON payload as a big string.
     float  stof(string item); // convert string item into float
        int stoi(string item); // convert string item into integer
     void   debug(bool enableDisable) { dweetEnableDebug( enableDisable? 1:0); cout << "dweetEnableDebug" << endl; };

private:
    string sThing;
    string sJsonRespStr;
    bool initialized;
};


#endif
