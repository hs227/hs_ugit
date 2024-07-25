#include"include/CLI11.hpp"
#include<stdio.h>
#include"data.h"

int parse_args(int argc, char *argv[]);

void init();
void hash_object(const std::string&);

int main(int argc,char* argv[]){
  return parse_args(argc,argv);
}

int parse_args(int argc, char *argv[])
{
  CLI::App app("ugit_cpp here");

  app.require_subcommand(1,1);//only 1 subcommand needed
  // sc_=subcmd_
  std::string input_file;


  // git init
  CLI::App* sc_init=app.add_subcommand("init","ugit_cpp init");
  sc_init->callback([&](){init();});

  // git hash-object
  CLI::App* sc_hash_object=app.add_subcommand("hash-object","hash-object a file");
  sc_hash_object->add_option("file",input_file,"file input")
    ->required();
  sc_hash_object->callback([&](){
    hash_object(input_file);
  });

  CLI11_PARSE(app,argc,argv);

  return 0;
}

void init()
{
  DATA::init();
}

void hash_object(const std::string &args)
{
  std::string file=args;
  DATA::hash_object(file);
}
