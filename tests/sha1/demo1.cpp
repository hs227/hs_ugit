#include"../../src/include/sha1.hpp"
#include<string>
#include<iostream>
using std::string;
using std::cout;
using std::endl;

int main()
{
  const string input="abc";

  SHA1 checksum;
  checksum.update(input);
  const string hash=checksum.final();
  cout<<"The SHA-1 of \""<<input<<"\" is: "<<hash<<endl;
}
