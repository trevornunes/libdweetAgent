#include "dweetApi.hpp"

DweetApi dweet;
int main(int argc, char **argv)
{
 cout << "DweetWriter" << endl;
 dweet = DweetApi();
 dweet.setup("something-0");
 dweet.push("varA=42");

 return 0;
}
