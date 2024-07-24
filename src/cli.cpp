#include"include/CLI11.hpp"
#include<stdio.h>

int parse_args(int argc, char *argv[]);

void init(const std::string& args);

int main(int argc,char* argv[]){
  return parse_args(argc,argv);
}

int parse_args(int argc, char *argv[])
{
  CLI::App app("ugit_cpp here");

  app.require_subcommand(1,1);//only 1 subcommand needed

  // sc_=subcmd_
  CLI::App* sc_init=app.add_subcommand("init","ugit_cpp init");
  sc_init->callback([&](){init("");});

  CLI11_PARSE(app,argc,argv);

  return 0;
}

void init(const std::string& args)
{
  printf("Hello, World!\n");
}
