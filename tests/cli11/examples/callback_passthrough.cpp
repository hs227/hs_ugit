#include"../../../src/include/CLI11.hpp"
#include<iostream>
#include<string>

int main(int argc,char* argv[])
{
  CLI::App app("callback_passthrough");
  app.allow_extras();//允许多余的参数
  std::string argName;
  std::string val;
  app.add_option("--argname",
    argName,
    "the name of the custom command line argument");
  app.callback([&](){
    if(!argName.empty()){
      CLI::App subapp;
      std::cout<<"callback in"<<std::endl;
      subapp.add_option("--"+argName,val,
        "custom argument option");
      subapp.parse(app.remaining_for_passthrough());
      std::cout<<"callback out"<<std::endl;
    }
  });

  CLI11_PARSE(app,argc,argv);
  std::cout<<"the value is now "<<val<<std::endl;
  return 0;
}



