#include "include/CLI11.hpp"
#include <stdio.h>
#include "data.h"
#include "base.h"

int parse_args(int argc, char *argv[]);
// argument modify
void modifier_name(std::string&,std::string="HEAD");
// subcommand
void init();
void hash_object(const std::string &);
void cat_file(const std::string &);
void write_tree();
void read_tree(const std::string &);
void commit(const std::string &);
void log(const std::string &);
void checkout(const std::string&);
void tag(const std::string&,const std::string&);


int main(int argc, char *argv[])
{
  return parse_args(argc, argv);
}

int parse_args(int argc, char *argv[])
{
  CLI::App app("ugit_cpp here");

  app.require_subcommand(1, 1); // only 1 subcommand needed
  // sc_=subcmd_
  // those std::strings` name are meaningless now
  std::string input_file;// actually argu1
  std::string output_file;// actually argu2
  int option_parsed_count=0;

  // git init
  CLI::App *sc_init = app.add_subcommand("init", "ugit_cpp init");
  sc_init->callback([&](){ 
    init(); });

  // git hash-object
  CLI::App *sc_hash_object = app.add_subcommand("hash-object", "hash-object a file");
  sc_hash_object->add_option("file", input_file, "file input")
      ->required();
  sc_hash_object->callback([&](){ 
    hash_object(input_file); });

  // git cat-file
  CLI::App *sc_cat_file = app.add_subcommand("cat-file", "print file content");
  sc_cat_file->add_option("object", input_file, "oid need")
    ->required();
  sc_cat_file->callback([&](){ 
    modifier_name(input_file,"");
    cat_file(input_file); });

  // git write-tree
  CLI::App *sc_write_tree = app.add_subcommand("write-tree", "tree object");
  sc_write_tree->callback([&](){ 
    write_tree(); });

  // git read-tree
  CLI::App *sc_read_tree = app.add_subcommand("read-tree", "restore the workshop");
  sc_read_tree->add_option("tree", input_file, "tree oid need")->required();
  sc_read_tree->callback([&](){ 
    modifier_name(input_file,"");
    read_tree(input_file); });

  // git commit
  CLI::App *sc_commit = app.add_subcommand("commit", "a snapshot");
  sc_commit->add_option("-m,--message", input_file, "commit need a message")->required();
  sc_commit->callback([&](){ 
    commit(input_file); });

  // git log
  CLI::App *sc_log = app.add_subcommand("log", "walk commit tree");
  sc_log->add_option("oid", input_file, "specify a oid rather than HEAD");
  sc_log->callback([&](){ 
    modifier_name(input_file);
    log(input_file); });

  // git checkout
  CLI::App *sc_checkout=app.add_subcommand("checkout","read_tree+set_HEAD");
  sc_checkout->add_option("oid",input_file,"chose commit")->required();
  sc_checkout->callback([&](){
    modifier_name(input_file);
    checkout(input_file);});
  
  // git tag
  CLI::App *sc_tag=app.add_subcommand("tag","alias of OID");
  sc_tag->add_option("name",input_file,"tag name")->required();
  sc_tag->add_option("oid",output_file,"oid");
  sc_tag->callback([&](){ 

    modifier_name(output_file);
    tag(input_file,output_file);});


  CLI11_PARSE(app, argc, argv);

  return 0;
}

void modifier_name(std::string &input, std::string default_)
{ 
  // oid/ref ->oid
  if(input==""){
    if(default_=="")
      return;
    input=default_;
  }

  std::string res=BASE::get_oid(input);
  input=res;
}

void init()
{
  DATA::init();
}

void hash_object(const std::string &args)
{
  std::string file = args;
  std::string oid = DATA::hash_object(file);
  std::cout << "hash-object finished: " << oid << std::endl;
}

void cat_file(const std::string &args)
{
  std::string oid = args;
  std::string res = DATA::get_object(oid, "");
  std::cout << (res.empty() ? "cat_file failed" : res) << std::endl;
}

void write_tree()
{
  std::string oid = BASE::write_tree(DATA::CUR_DIR + "/");
  printf("tree %s\n", oid.c_str());
}

void read_tree(const std::string &args)
{
  std::string oid = args;
  BASE::read_tree(oid);
}

void commit(const std::string &args)
{
  std::string msg = args;
  std::cout << BASE::commit(msg) << std::endl;
}

void log(const std::string &args)
{
//  std::string oid = args != "" ? args : BASE::get_oid("HEAD");
  std::string oid=args;
  while (oid != "")
  {
    BASE::commit_cxt cxt = BASE::get_commit(oid);
    std::cout << "commit " << oid << "\n";
    std::cout << "tree " << cxt.tree << "\n";
    if (cxt.parent != "")
      std::cout << "parent " << cxt.parent << "\n";
    std::cout << "  " << cxt.msg << "\n";
    std::cout << std::endl;

    oid = cxt.parent;
  }
}

void checkout(const std::string &args)
{ 
  std::string oid=args;
  BASE::checkout(oid);
}

void tag(const std::string & arg_name, const std::string & arg_oid)
{
  //std::string oid=arg_oid!=""?arg_oid:BASE::get_oid("HEAD");
  std::string oid=arg_oid;

  if(arg_name.find('/')!=std::string::npos){
    std::cout<<"tag`s name cant contain the '/'.["<<arg_name<<"]\n";
    return;
  }
  BASE::create_tag(arg_name,oid);
}







