#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;
struct symbolTable{
    map<string, int> symTableAddress;
    map<string, int> symTableUsage;
    vector<string> order;

    int lookup(string s){
        try{
            return symTableAddress.at(s);
        }
        catch(const std::out_of_range& oor){
            return -1;
        }
    }
    bool hasUsed(string s){
        try{
            return (symTableUsage.at(s)>0?true:false);
        }
        catch(const std::out_of_range& oor){
            return false;
        }
    }
    void addUse(string s){
        try{
            symTableUsage.at(s)++;
        }
        catch(const std::out_of_range& oor){
        }
    }
    bool add(string s, int addr){
        if(lookup(s)>0) return false;
        symTableAddress.insert({s,addr});
        symTableUsage.insert({s,0});
        order.push_back(s);
        return true;
    }
    string table2String(){
        string output = "Symbol Table\n";
        for(string s: order){
            output+=s+'='+to_string(lookup(s))+"\n";
        }
        return output;
    }
    

};
struct contentParser{
    string content = "";
    int length = 0;

    /*Indices to trace the file*/
    int currentIdx = 0;
    int nextIdx = -1;

    int currentLineNum = 1;
    int nextLineNumber = 1;

    int currentLineOffset = 1;
    int nextLineOffset = 0;


    contentParser(string input){
        content = input;
        length = input.size();
    }
    bool isLegalSymbol(string token){
        int tokenLength = token.length();
        for(char c:token){
            if(c=='\0') tokenLength--;
            else if(!(('0'<=c&&c<='9')||('a'<=c&&c<='z')||('A'<=c&&c<='Z'))) return false;
        }
        if(tokenLength>16 || tokenLength <=0){
            return false;
        }
        char first = token.at(0);
        if(!(('a'<=first&&first<='z')||('A'<=first&&first<='Z'))) return false;
        return true;
    }
    string getToken(){
        currentIdx = nextIdx+1;
        currentLineOffset = nextLineOffset+1;
        currentLineNum = nextLineNumber;
        //locate currentIdx
        while(currentIdx < length){
            char c = content.at(currentIdx);
            if(c == ','|| c == ' ' || c == '\t'){
                currentIdx++;
                currentLineOffset++;
            }
            else if(c == '\n'){
                currentIdx++;
                currentLineOffset = 1;
                currentLineNum++;
            }
            else break;
        }

        //locate nexIdx;
        nextIdx = currentIdx+1;
        nextLineOffset = currentLineOffset+1;
        nextLineNumber = currentLineNum;
        while(nextIdx < length){
            char c = content.at(nextIdx);
            if(c == ','|| c == ' ' || c == '\t'){
               break;
            }
            else if(c == '\n'){
                nextLineNumber++;
                nextLineOffset = 0;
                break;
            }
            else{
                nextLineOffset++;
                nextIdx++;
            }
        }
        if(currentIdx<length){
            if(nextIdx<length)
                return content.substr(currentIdx, nextIdx-currentIdx);
            else
                return content.substr(currentIdx, length-currentIdx-1);
        }
        else{
            return "";
        }
    }
};
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
void print_symbol_table(){
    cout<<"Symbol Table"<<endl;
}
void print_memory_map(){
    cout<<"Memory Map"<<endl;
}
void __parseerror(int errcode, int linenum, int lineoffset) {
 static string errstr[] = {
    "NUM_EXPECTED", // Number expect
    "SYM_EXPECTED", // Symbol Expected
    "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
    "SYM_TOO_LONG", // Symbol Name is too long
    "TOO_MANY_DEF_IN_MODULE", // > 16
    "TOO_MANY_USE_IN_MODULE", // > 16
    "TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode].c_str());
}

int main (int argc, char* argv[]) {
    if(argc != 2){
        cout<< "GIVE FILENAME AS AN PARAMETER" <<endl;
        return 1;
    }
    FILENAME = argv[1];

    /*First Pass*/

    string fileContent = "";
    fileContent = readFile2String(FILENAME);
    if(fileContent.size() <= 0){
        print_symbol_table();
        print_memory_map();
        return 0;
    }
    
    /*From now, file has been read into fileContent and ready for 1st pass*/

    contentParser cp(fileContent);
    symbolTable st;
    
    string tmp = cp.getToken();
    while(tmp.size()>0){
        cout<<"\""<<tmp<<"\" l:"<<cp.currentLineNum<<" offset:"<<cp.currentLineOffset<<"  nl:"<<cp.nextLineNumber<<"nof:"<<cp.nextLineOffset<<endl;
        tmp = cp.getToken();
    }


    // st.add("asdfasf",10);
    // st.add("sadfasfa2",30);
    // st.add("sadfasfa3",12);
    // st.add("sadfasfa4",78);
    // cout<<st.table2String();

    
  return 0;
}