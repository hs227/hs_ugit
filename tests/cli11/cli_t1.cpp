#include"../../src/include/CLI11.hpp"
#include<iostream>

using namespace std;

int main(int argc,char** argv)
{
  CLI::App app("CLI11 example");
  std::string name;
  // 需要 -n和--name后带参数
  // app.add_option("-n,--name",name,"Your name");
  // 必须的参数
  // app.add_option("-n,--name,name",name,"Your name")->required();
  // name直接代表参数输入
  app.add_option("-n,--name,name",name,"Your name");

  CLI11_PARSE(app,argc,argv);
  std::cout<<"Hello, "<<name<<"!"<<std::endl;

  return 0;
}


