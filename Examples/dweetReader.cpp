#include "dweetApi.hpp"

DweetApi dweet;
int main(int argc, char **argv)
{
 string thingName = "something-0";
 cout << "DweetReader" << endl;
 dweet = DweetApi();
 dweet.setup(thingName);
 dweet.pull();
 cout << dweet.get() << endl;

 // FIXME: SEGV with 'item' level payload retrieval ...
 //cout << dweet.get("var") << endl;
 return 0;
}
