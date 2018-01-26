#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void test(){
  cout<<"before exit"<<endl;
  exit(0);
  cout<<"after exit"<<endl;
}

int main () {
  test();
  return 0;
}