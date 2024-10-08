#include"../../../src/include/CLI11.hpp"
#include<iostream>
#include<string>

int main(int argc,char* argv[]){

  CLI::App app("K3pi goofit fitter");
  // add version output
  app.set_version_flag("-v,--version",std::string(CLI11_VERSION));
  std::string file;
  CLI::Option*opt=app.add_option("-f,--file",
    file,"File name");  

  int count=0;
  CLI::Option* copt=app.add_option("-c,--count",
    count,"Counter");
  int v=0;
  CLI::Option* flag=app.add_flag("--flag",
    v,"Some flag that can be passed multiple times");
  double value=0.0;
  app.add_option("-d,--double",
    value,"Some Value");

  CLI11_PARSE(app,argc,argv);

  std::cout<<"Working on file: "<<file
    <<", direct count: "<<app.count("--file")
    <<", opt count: "<<opt->count()<<std::endl;
  std::cout << "Working on count: " << count 
            << ", direct count: " << app.count("--count")
            << ", opt count: " << copt->count() << std::endl;
  std::cout<<"Received flag: "<<v<<" ("<<flag->count()
    <<") times\n";
  std::cout<<"Some value: "<<value<<std::endl;


  return 0;
}
