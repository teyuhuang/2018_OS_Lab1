#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#define SYMBOL_LENGTH_LIMIT 16

using namespace std;
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
    exit(0);
}
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
struct symbolTable{
    map<string, int> symTableAddress;
    map<string, int> symTableUsage;
    map<string, bool> multipleDefined;
    map<string, int> symModuleID;
    vector<string> order;
    string unusedList2Str(){
        string output="";
        for(string sym: order){
            if(symTableUsage.at(sym)==0){
                output += "Warning: Module "+to_string(symModuleID.at(sym))+": "+sym+" was defined but never used\n";
            }
        }
        return output;
    }
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
    bool add(string s, int addr, int mId){
        if(lookup(s)>0) {
            multipleDefined.at(s) = true;
            return false;
        }
        symTableAddress.insert({s,addr});
        symTableUsage.insert({s,0});
        multipleDefined.insert({s, false});
        symModuleID.insert({s,mId});
        order.push_back(s);
        return true;
    }
    void set(string s, int addr){
        if(lookup(s)<0) return;
        symTableAddress.at(s) = addr;
    }

    string table2String(){
        string output = "Symbol Table\n";
        for(string s: order){
            output+=s+'='+to_string(lookup(s));
            if(multipleDefined.at(s))
                output+=" Error: This variable is multiple times defined; first value used\n";
            else
                output+='\n';
        }
        return output;
    }
    

};
struct contentTool{
    string content = "";
    int length = 0;
    bool eof = false;
    /*Indices to trace the file*/

    int currentIdx = 0;
    int nextIdx = -1;

    int prevLineNum = 1;
    int currentLineNum = 1;
    int nextLineNumber = 1;

    int prevRealNextOffset = 1;
    int realNextOffset = 1;
    int currentLineOffset = 1;
    int nextLineOffset = 0;


    contentTool(string filename){
        content = readFile2String(filename);
        length = content.size();
    }
    bool isBlankFile(int begining){
        if(length == 0) return true;
        for(int i=begining; i<length; i++){
            char c = content.at(i);
            if(!((c==' ')||(c=='\n')||(c=='\t'))) return false;
        }
        return true;
    }
    bool isLegalSymbol(string token){
        int tokenLength = token.length();
        if(tokenLength <=0){
            return false;
        }
        for(char c:token){
            if(!(('0'<=c&&c<='9')||('a'<=c&&c<='z')||('A'<=c&&c<='Z'))) return false;
        }
        char first = token.at(0);
        if(!(('a'<=first&&first<='z')||('A'<=first&&first<='Z'))) return false;
        return true;
    }
    bool isSymbolTooLong(string t){
        int tokenLength = t.length();
        if(t.at(tokenLength-1)=='\0') tokenLength--;
        return (tokenLength > SYMBOL_LENGTH_LIMIT);
    }
    bool isNumber(string token){
        if(token.size()<=0) return false;
        for(char c:token){
            if(!('0'<=c&&c<='9')) return false;
        }
        return true;
    }
    int s2i(string s){
        return stoi(s,nullptr,10);
    }
    
    string getToken(){
        prevLineNum = currentLineNum;
        prevRealNextOffset = realNextOffset;
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
        realNextOffset = currentLineOffset+1;
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
                realNextOffset++;
                nextIdx++;
            }
        }
        if(currentIdx<length){
            if(isBlankFile(nextIdx)) eof = true;
            if(nextIdx<length)
                return content.substr(currentIdx, nextIdx-currentIdx);
            else{
                eof = true;
                return content.substr(currentIdx, length-currentIdx-1);
            }
        }
        else{
            eof = true;
            return "";
        }
    }
    string readSym(){
        string t = getToken();
        if(!isLegalSymbol(t)){
            if(t.size()<=0)
                __parseerror(1,prevLineNum,prevRealNextOffset);
            else   
                __parseerror(1,currentLineNum,currentLineOffset);
        }
        else if(isSymbolTooLong(t)){
           __parseerror(3,currentLineNum,currentLineOffset);
        }
        return t;
    }
    int readInt(){
        string t = getToken();
        if(!isNumber(t)){
            if(t.size()<=0)
                __parseerror(0,prevLineNum,prevRealNextOffset);
            else   
                __parseerror(0,currentLineNum,currentLineOffset);
        }
        return s2i(t);
    }
    char readAddr(){
        string t = getToken();
        if(t.size()<=0)
            __parseerror(2,prevLineNum,prevRealNextOffset);
        else if(t.size()>1 || !((t.at(0) == 'A')||(t.at(0) == 'E')||(t.at(0) == 'I')||(t.at(0) == 'R')))
            __parseerror(2,currentLineNum,currentLineOffset);
        
        return t.at(0);
    }
};
struct module{
    module(int b, int i){
        base = b;
        id = i;
    }
    int base = 0;
    int id = 0;
    int numOfSymbol = 0;
    int length = 0;
    vector<pair<string, int>> defList;
    vector<pair<string, bool>> useList;
    void checkSymbolRelativeError(symbolTable& st){
        for(auto sym: defList){ 
            if(sym.second>=length){
                cout<<"Warning: Module "<<id<<": "<<sym.first<<" too big "<<sym.second<<" (max="<<length-1<<") assume zero relative\n";
                st.set(sym.first, base);
            }
        }
    }
};
struct pass1And2{
    int numOfInstrucs = 0;
    int pass1or2 = 1;
    int addressIdx = 0;
    string addr2Str(int addr3, int addr4){
        string zeros;
        string a3 = to_string(addr3);
        if(a3.size()<3){
           for(int i = 0; i< 3-a3.size(); i++){
                zeros+="0";
            }
            a3 = zeros+a3; 
        }
        zeros.clear();
        string a4 = to_string(addr4);
        if(a4.size()<4){
            for(int i = 0; i< 4-a4.size(); i++){
                zeros+="0";
            }
        }
        a4 = zeros+a4;
        return a3+": "+a4;
    }
    void readDefList(contentTool& ct, symbolTable& st, module& modl){
        int numDefs=ct.readInt();
        modl.numOfSymbol = numDefs;
        if(pass1or2 == 1){
            if(numDefs > 16){
                __parseerror(4,ct.currentLineNum,ct.currentLineOffset);
            }
        }
        for (int i=0;i<numDefs; i++){
            readDef(ct, st, modl);
        }
    }
    void readDef(contentTool& ct, symbolTable& st, module& modl) {
        string symbol= ct.readSym(); 
        int offset= ct.readInt();
        if(pass1or2 == 1){
            if(st.add(symbol, modl.base + offset,modl.id)){
                modl.defList.push_back({symbol, offset});
            }
        }
    }


    void readUseList(contentTool& ct, symbolTable& st, module& modl){
        int numUses=ct.readInt();
        if(pass1or2 == 1){
            if(numUses > 16){
            __parseerror(5,ct.currentLineNum,ct.currentLineOffset);
            }
        }
        for (int i=0;i<numUses; i++){
            readUse(ct, modl);
        }
    }
    void readUse(contentTool& ct, module& modl) { 
        string symbol= ct.readSym(); 
        modl.useList.push_back({symbol, false});
    }
    
    void readInstrList(contentTool& ct, symbolTable& st, module& modl){
        int numInstrs=ct.readInt();
        modl.length = numInstrs;
        if(pass1or2 == 1){
             numOfInstrucs += numInstrs;
            if(numOfInstrucs > 512){
                __parseerror(6,ct.currentLineNum,ct.currentLineOffset);
            }
        }
        for (int i=0;i<numInstrs; i++){
            readInstr(ct, st, modl);
        }
    }
    void readInstr(contentTool& ct, symbolTable& st, module& modl) {
        char addr= ct.readAddr();
        int command = ct.readInt();
        if(pass1or2 == 2){
            int op = command/1000;
            int operand = command%1000;
            string errors = "";
            switch(addr){
                case 'R':
                    if(op > 9){
                        errors="Error: Illegal opcode; treated as 9999";
                        command = 9999;
                    }
                    else if(operand > modl.length-1){
                        errors = "Error: Relative address exceeds module size; zero used";
                        command = op*1000+modl.base;
                    }
                    else command = op*1000+modl.base+operand;
                    break;
                case 'A':
                    if(op > 9){
                        errors="Error: Illegal opcode; treated as 9999";
                        command = 9999;
                    }
                    else if(operand>=512){
                        errors="Error: Absolute address exceeds machine size; zero used";
                        command = op*1000;
                    }
                    break;
                case 'I':
                    if(op > 9){
                        errors="Error: Illegal immediate value; treated as 9999";
                        command = 9999;
                    }
                    break;
                case 'E':
                    if(op > 9){
                        errors="Error: Illegal opcode; treated as 9999";
                        command = 9999;
                    }
                    else if(operand >= modl.useList.size()){
                        errors = "Error: External address exceeds length of uselist; treated as immediate";
                    }
                    else{
                        int eAddr = st.lookup(modl.useList[operand].first);
                        modl.useList[operand].second = true;
                        if(eAddr >= 0){
                            st.addUse(modl.useList[operand].first);
                            command = op*1000+eAddr;
                        }
                        else{
                            errors = "Error: "+modl.useList[operand].first+" is not defined; zero used";
                            command = op*1000;
                        }
                    }
                    break;
            }
            if(errors.size()>0){
                cout<<addr2Str(addressIdx,command)<<" "<<errors<<endl;
            }
            else
                cout<<addr2Str(addressIdx,command)<<endl;
            addressIdx++;
        }
    }

    
    void pass1(contentTool& ct, symbolTable& st){
        pass1or2 = 1;
        numOfInstrucs = 0;
        int moduleIdx = 1;
        int base = 0;
        while(!ct.eof){
            module tmpModule(base,moduleIdx);
            readDefList(ct, st, tmpModule);
            readUseList(ct, st, tmpModule);
            readInstrList(ct, st, tmpModule);
            tmpModule.checkSymbolRelativeError(st);
            moduleIdx++;
            base += tmpModule.length;
        }
    }
    void pass2(contentTool& ct, symbolTable& st){
        pass1or2 = 2;
        numOfInstrucs = 0;
        int moduleIdx = 1;
        int base = 0;
        print_memory_map();
        string warnings="";
        while(!ct.eof){
            module tmpModule(base,moduleIdx);
            readDefList(ct, st, tmpModule);
            readUseList(ct, st, tmpModule);
            readInstrList(ct, st, tmpModule);
            for(auto use:tmpModule.useList){
                if(!use.second){
                    warnings+="Warning: Module "+to_string(tmpModule.id)+": "+use.first+" appeared in the uselist but was not actually used\n";
                }
            }
            moduleIdx++;
            base += tmpModule.length;
        }
        warnings+=st.unusedList2Str();
        if(warnings.size()>0){
            cout<<"\n"<<warnings;
        }
    }
};

int main (int argc, char* argv[]) {
    if(argc != 2){
        cout<< "GIVE FILENAME AS AN PARAMETER" <<endl;
        return 1;
    }
    FILENAME = argv[1];

    symbolTable st;
    pass1And2 pass12;
    /*First Pass*/

    contentTool ct(FILENAME); // Read in the file for the 1st time
    
    if(ct.isBlankFile(0)){
        print_symbol_table();
        print_memory_map();
        return 0;
    }
    pass12.pass1(ct,st);
    cout<<st.table2String()<<endl;

    /*Second Pass*/
    contentTool ct2(FILENAME); // Read in the file for the 2nd time
    pass12.pass2(ct2,st);
    
  return 0;
}