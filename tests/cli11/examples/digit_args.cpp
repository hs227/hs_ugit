#include"../../../src/include/CLI11.hpp"
#include<iostream>

int main(int argc,char* argv[])
{
  CLI::App app;

  int val=0;
  // sum a set of flags with default values 
  // associate with them
  app.add_flag("-1{1},-2{2},-3{3}",val,"compression level");

  CLI11_PARSE(app,argc,argv);

  std::cout<<"value = "<<val<<std::endl;
  return 0;
}


