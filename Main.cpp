#include <iostream>
#include <fstream>
#include <bits/stdc++.h> 
#include <vector>
#include <string>
#include <iterator>
#include <map>

using namespace std;

struct HuffmanTree{
    char c;
    int freq;
    struct HuffmanTree* left;
    struct HuffmanTree* right;
};

struct DataList{
    HuffmanTree* element;
    struct DataList* next;
};

DataList* head = nullptr;
int sizeOfList = 0;

map<char,string> encodingTable;
int indexOfVector = 0; // for reading saved tree

vector<string> parserFile(string str,char c);
void addToList(HuffmanTree* node);
void removeFromList();
void encodingTree(HuffmanTree* root,string encodingWord);
void makeTree();
void findEncodingStr(string &str,string &str2);
void writeTree(HuffmanTree *root,ofstream &out);
void readTree(HuffmanTree *&root, vector<string> s,int len);
void printTree(HuffmanTree* r);
string decodeTree(HuffmanTree* root,string s);

int main(int argc,char *argv[]){
    
    if ( !strcmp(argv[1],"-i") && !strcmp(argv[3],"-encode")){
        ifstream input;
        input.open(argv[2]);
        for (string line; getline(input, line);){
            transform(line.begin(), line.end(), line.begin(), ::tolower);
            map<char,int> freqList;
            for ( int i = 0; i < line.size(); i++){
                int temp = freqList.count(line[i]);
                if ( temp == 0){
                    freqList.insert( pair<char, int>(line[i],1) );
                }else{
                    freqList[line[i]]++;
                }    
            }

            for ( auto e : freqList){
                HuffmanTree* element = new HuffmanTree();
                element->c = e.first;
                element->freq = e.second;
                element->left = nullptr;
                element->right = nullptr;
                addToList(element);
            }

            makeTree();
            
            string encodeStr = "";
            ofstream fout;
            fout.open("outForDecode.txt");
            writeTree(head->element,fout);
            fout.close();
            findEncodingStr(line,encodeStr);
            ofstream fout2;
            fout2.open("output.txt");
            for ( auto x: encodingTable){
                fout2 << x.first << x.second << endl;
            }
            fout2.close();
            cout << encodeStr << endl;          
        }
        input.close();
    }else if ( !strcmp(argv[1],"-s")){
        ifstream input;
        input.open("output.txt");
        for (string line; getline(input, line);){
            if ( line.substr(0,1) == argv[2] ){
                int len = line.length();
                string s = line.substr(1,len-1);
                cout << s << endl;
            }
        }
        input.close();
    }else if ( !strcmp(argv[1],"-i") && !strcmp(argv[3],"-decode") ){
        string encodingstring;
        ifstream in;
        in.open(argv[2]);
        in >> encodingstring;
        in.close();
        ifstream input;
        input.open("outForDecode.txt");
        for (string line; getline(input, line);){
            vector<string> records = parserFile(line,'%');
        
            HuffmanTree* root = nullptr;
            readTree(root,records,records.size()-1);
            
            string answer = decodeTree(root,encodingstring);
            cout << answer << endl;
        }
        input.close();

    }else{
        ifstream input;
        input.open("outForDecode.txt");
        for (string line; getline(input, line);){
            vector<string> records = parserFile(line,'%');
        
            HuffmanTree* root = nullptr;
            readTree(root,records,records.size()-1);
            printTree(root);            
        }
        input.close();
    }
    
    return 0;
}

vector<string> parserFile(string str,char c){
    vector<string> wordArr;
    string word = "";
    for (auto x : str) 
    {
        if (x == c){  // if find speacial char,add them to list 
            wordArr.push_back(word);
            word = "";    // then continue for new word
        }
        else {
            word = word + x;
        }
    }
    wordArr.push_back(word);   // all list for we want words
    return wordArr;
}

void addToList(HuffmanTree* node){
    DataList* newNode = new DataList();
    newNode->element = node;
    sizeOfList++;
    if ( head == nullptr){
        head = newNode;
        newNode->next = nullptr;
    }else{
        if ( node->freq <= head->element->freq){
            newNode->next = head;
            head = newNode;
        }else{
            DataList* iter;
            iter = head;
            while ( iter->next != nullptr ){
                if ( node->freq >= iter->element->freq && iter->next != nullptr && node->freq <= iter->next->element->freq){
                    newNode->next = iter->next;
                    iter->next = newNode;
                    return;
                }
                iter = iter->next;
            }
            iter->next = newNode;
            newNode->next = nullptr;
        }
    }
}

void removeFromList(){
    if ( head != nullptr){
        DataList* temp;
        temp = head;
        head = head->next;
        temp->next = nullptr;
        delete temp;
        sizeOfList--;
    }
}

void encodingTree(HuffmanTree* root,string encodingWord){
    if ( root == nullptr){
        return;
    }
    if ( root->c != '\0'){
        encodingTable[root->c] = encodingWord;
    }
    encodingTree(root->left,encodingWord+"0");
    encodingTree(root->right,encodingWord+"1");
}

void makeTree(){
    HuffmanTree *left;
    HuffmanTree *right;
    HuffmanTree *top;
    while ( sizeOfList != 1){
        left = head->element;
        removeFromList();

        right = head->element;
        removeFromList();

        top = new HuffmanTree();
        top->c = '\0';
        top->freq = left->freq + right->freq;
        top->left = left;
        top->right = right;
        addToList(top);
    }
    encodingTree(head->element,"");
}

void findEncodingStr(string &str,string &str2){
    int len = str.length();
    for ( int i = 0; i < len; i++){
        str2 += encodingTable[str[i]];
    }
}

void writeTree(HuffmanTree *root,ofstream &out){
    if ( !root){
        out << '^' << "%";
    }else{
        out << root->c << root->freq << "%";
        writeTree(root->left,out);
        writeTree(root->right,out);
    }
}

void readTree(HuffmanTree *&root, vector<string> s,int len){
    if ( indexOfVector >= len-1){
        indexOfVector++;
        return;
    }
    if ( s[indexOfVector][0] == '^'){
        indexOfVector++;
        return;
    }
    root = new HuffmanTree();
    int slen = s[indexOfVector].size();
    if ( s[indexOfVector][0] == '\0'){
        root->c = '\0';
        root->freq = stoi(s[indexOfVector].substr(1,slen-1));
    }else{
        root->c = s[indexOfVector][0];
        root->freq = stoi(s[indexOfVector].substr(1,slen-1));
    }
    indexOfVector++;
    readTree(root->left,s,len);
    readTree(root->right,s,len);    
}

void printTree(HuffmanTree* r){
    if ( r != nullptr){
        if ( r->c != '\0'){
            cout << r->c << "-->" << r->freq << endl; 
        }else{
            cout << "sibling-->" << r->freq << endl;
        }

        printTree(r->left);
        printTree(r->right);
    }
}

string decodeTree(HuffmanTree* root,string s){
    string result = ""; 
    HuffmanTree* iter = root; 
    for (int i=0;i<s.size();i++) { 
        if (s[i] == '0'){
            iter = iter->left;
        }else{
           iter = iter->right; 
        }
        
        if (iter->left == nullptr && iter->right == nullptr){ 
            result += iter->c; 
            iter = root; 
        } 
    } 
    return result; 
}
