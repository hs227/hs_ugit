#include"include/CLI11.hpp"
#include<stdio.h>
#include"data.h"
#include"base.h"

int parse_args(int argc, char *argv[]);

void init();
void hash_object(const std::string&);
void cat_file(const std::string&);
void write_tree();

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
  sc_hash_object->callback([&](){hash_object(input_file);});

  // git cat-file
  CLI::App* sc_cat_file=app.add_subcommand("cat-file","print file content");
  sc_cat_file->add_option("object",input_file,"oid need")->required();
  sc_cat_file->callback([&](){cat_file(input_file);});

  // git write-tree
  CLI::App* sc_write_tree=app.add_subcommand("write-tree","tree object");
  sc_write_tree->callback([&](){ write_tree(); });

  CLI11_PARSE(app,argc,argv);

  return 0;
}

void init()
{
  DATA::init();
}

void hash_object(const std::string &args)
{
  std::string file= args;
  std::string oid = DATA::hash_object(file);
  std::cout << "hash-object finished: " << oid << std::endl;
}

void cat_file(const std::string & args)
{
  std::string oid=args;
  std::string res=DATA::cat_file(oid,"");
  std::cout<<(res.empty()?"cat_file failed":res)<<std::endl;
}

void write_tree()
{
  BASE::write_tree(DATA::CUR_DIR+"/");
}
