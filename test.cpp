#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int main () {
  ifstream myfile;
  myfile.open ("iexample.txt");
  myfile.close();
  vector<string> strV;
  strV.push_back("asdf");
  strV.push_back("asdfads");
  for(auto a: strV){
   cout<<a<<endl;
  }
  return 0;
}