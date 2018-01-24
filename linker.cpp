#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
string FILENAME;
string readFile2String(string filename){
    ifstream inFile;
    try {
        inFile.open(filename);
        if(inFile.is_open()){
            stringstream strStream;
            strStream << inFile.rdbuf();
            inFile.close();
            return strStream.str();
        }
    }
    catch (ifstream::failure e) {
        cerr << "Exception opening/reading/closing file\n";
    }
    return "";
}
int main (int argc, char* argv[]) {
    if(argc != 2){
        cout<< "GIVE FILENAME AS AN PARAMETER" <<endl;
        return 1;
    }
    FILENAME = argv[1];
    string fileContent = "";
    fileContent = readFile2String(FILENAME);
    if(fileContent.size() <= 0) return 1;
    
    /*From now, file has been read into fileContent and ready for 1st pass*/
    
  return 0;
}