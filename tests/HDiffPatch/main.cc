#include<iostream>
#include<fstream>
#include<stdlib.h>


using namespace std;


int main()
{
  std::string in_data;
  
  ifstream in("output.txt",ios::binary|ios::ate);
  if(!in.is_open()){
    cout<<"open failed"<<endl;
    return EXIT_FAILURE;
  }
  in_data.resize(in.tellg());
  in.seekg(0);
  in.read((char*)in_data.data(),in_data.size());
  in.close();
  cout.write(in_data.data(),in_data.size());

}


