#include "dweetApi.hpp"



void DweetApi::setup(string thing) {
      sThing = thing;
      dweetInit( sThing.c_str() );
      initialized=true;
      cout << "DweetApi::setup" << endl;
};

bool DweetApi::isConfigured()  { return initialized; };

bool DweetApi::push(string stuff)   {
  cerr << "DweetApi::push " + stuff << endl;
  if( dweetPushThis( stuff.c_str() ) != 0) {

       return false;
  }
   else return true;
};

string DweetApi::pull(void) {
    cerr << "DweetApi::pull " << endl;

    dweetPull();
    sJsonRespStr = dweetGetPayloadStr();
    return sJsonRespStr;
};

string DweetApi::getStatus(void) {
   string msgState = dweetGetCurlStatusStr();
   if(msgState.size() == 0)
       return "ok";
   else
       return msgState;
}


// Return a string object of the payload of a single item in the 'content'
// JSON array.  e.g. tbot thing has sensorX, sensorY,sensorZ.   ::get("sensorY") would return
// just sensorY value.
string DweetApi::get(string item) {
    string tmp;
    // dweetPull();  // We work with the last message ...

    tmp = dweetGetItemVal( item.c_str() );
    if( tmp.size() <= 0 )
       return "";

    cout << "DweetApi::pull " + item + " -> '" + tmp + "'" << endl;
    return tmp;
}

float DweetApi::stof(string item) {

    if( item.size() == 0) {
        return 0.0f;
    }

    istringstream ss(item);
    float val;
    if (! (ss>>val) )
    {
        cerr << "::getFloat failed to convert " + item + " to float" << endl;
        return 0.0f;
    }
    return val;

}

int DweetApi::stoi(string item) {
    return atoi( item.c_str() );
}



