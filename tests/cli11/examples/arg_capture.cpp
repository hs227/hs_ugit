#include"../../../src/include/CLI11.hpp"
#include<iostream>
#include<string>

/* 
  This example demonstrates the use of 'prefix_command'
  on a subcommand to capture all subsequent arguments
  along with an alias to 
  make it appear as a regular options.

  All the values after the "sub" or "--sub" are
  available in the remaining() method.
*/

int main(int argc,char*argv[])
{
  int value(0);
  CLI::App app("Test App");
  app.add_option("-v",value,"value");
  app.require_subcommand(1);
  CLI::App* subcom=app.add_subcommand("sub",
    "")->prefix_command();
  subcom->alias("--sub");
  CLI11_PARSE(app,argc,argv);

  std::cout<<"value="<<value<<std::endl;
  std::cout<<"after Args:";
  for(const auto& arg:subcom->remaining()){
    std::cout<<arg<<" ";    
  }
  std::cout<<"\n";

  return 0;
}



