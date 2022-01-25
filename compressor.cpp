#include <bits/stdc++.h>
using namespace std;

//Node Definition of Huffman Tree
class Node
{
    public:
        int freq;
        char c;
        Node* left;
        Node* right;
        Node()
        {
            left=NULL;
            right=NULL;
        }
};

//Compare Class for Node in Priority Queue
class Comp{
    public:
    int operator()(Node* a,Node* b)
    {
        return a->freq>b->freq;
    }
};

//Recursive Function to create Symbol Table from Huffman Tree
void createSymbolTable(Node* curr,unordered_map<char,string> &Symbol_Table,string &path)
{
    if(curr->left==NULL&&curr->right==NULL)
    {
        Symbol_Table[curr->c]=path;
        return;
    }
    path=path+"0";
    createSymbolTable(curr->left,Symbol_Table,path);
    path[path.length()-1]='1';
    createSymbolTable(curr->right,Symbol_Table,path);
    path=path.substr(0,path.length()-1);
}

//Function to write bit by bit in Compressed File
void WriteBit (int bit,int &current_bit,unsigned char &bit_buffer,FILE* f)
{
  if (bit)
    bit_buffer |= (1<<current_bit);

  current_bit++;
  if (current_bit == 8)
  {
    fwrite (&bit_buffer, 1, 1, f);
    current_bit = 0;
    bit_buffer = 0;
  }
}

int main()
{
    string filename("input.txt"); //Input File
    unordered_map<char,int> count; //Unordered map to store count of each character
    vector<char> bytes; //Vector to store input file
    char byte;

    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cout << "Could not open the file - " << filename << endl;
        return EXIT_FAILURE;
    }

    //Read input file
    while (input_file.get(byte)) {
        count[byte]++;
        bytes.push_back(byte);
    }

    //Create Huffman Tree Using Priority Queue
    priority_queue<Node*,vector<Node*>,Comp > pq;
    for(auto i:count)
    {
        Node* n=new Node();
        n->freq=i.second;
        n->c=i.first;
        pq.push(n);
    }
    while(pq.size()!=1)
    {
        Node* n1=pq.top();
        pq.pop();
        Node* n2=pq.top();
        pq.pop();
        Node* n=new Node();
        n->left=n1;
        n->right=n2;
        n->freq=n1->freq+n2->freq;
        pq.push(n);
    }

    //Create and show Symbol Table
    unordered_map<char,string> Symbol_Table;
    string path="";
    Node* p=pq.top();
    createSymbolTable(p,Symbol_Table,path);
    for(auto i:Symbol_Table)
    {
        cout << i.first  << " " << i.second << endl;
    }

    //Write Bit by Bit into Compressed File
    string s;
    for(int i=0;i<bytes.size();i++)
    {
        s+=Symbol_Table[bytes[i]];
    }
    cout << s << endl;
    FILE* op;
    op=fopen("compressed_file.bin","wb");
    if(!op)
    {
        cout << "Could not open File - compressed_file.bin" << endl;
    }
    int current_bit = 0;
    unsigned char bit_buffer=0;
    for(int i=0;i<s.length();i++)
    {
        WriteBit(s[i]-48,current_bit,bit_buffer,op);
    }
    while(current_bit)
    {
        WriteBit(0,current_bit,bit_buffer,op);
    }

    //Create file containing information to Decompress the Compressed file
    FILE* passInfo;
    passInfo=fopen("DecompressInfo.txt","w");
    if(!passInfo)
    {
        cout << "Could not open File - DecompressInfo.txt" << endl;
    }
    string sz=to_string(bytes.size())+"\n";
    for(int i=0;i<sz.length();i++)
    {
        fwrite(&sz[i],1,1,passInfo);
    }
    for(auto i:Symbol_Table)
    {
        fwrite(&i.first,1,1,passInfo);
        char space=' ';
        fwrite(&space,1,1,passInfo);
        for(int j=0;j<i.second.length();j++)
        {
            fwrite(&i.second[j],1,1,passInfo);
        }
        char endLine='\n';
        fwrite(&endLine,1,1,passInfo);
    }

    //Close Opened Files
    fclose (op);
    input_file.close();
    fclose (passInfo);

    return 0;
}