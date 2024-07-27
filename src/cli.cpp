#include "include/CLI11.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "base.h"
#include "dotter.h"

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
void k();

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

  // git k
  CLI::App *sc_k=app.add_subcommand("k","visualization tool");
  sc_k->callback([&](){k();});

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
    BASE::commit_ctx cxt = BASE::get_commit(oid);
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

void k()
{
  // 显示的oid位数
  const size_t oid_len = 6;
  DOT::dot_ctx cmt_ctx;
  DOT::init_dotter(&cmt_ctx, "commit");

  // refs
  std::vector<std::string> refs_name;
  std::vector<std::string> refs_oid;
  DATA::iter_refs(refs_name, refs_oid);
  for (size_t i = 0; i < refs_name.size(); ++i)
  {
    std::cout << refs_name[i] << " " << refs_oid[i] << std::endl;
    refs_name[i] = std::filesystem::path(refs_name[i]).filename().string();
    std::string text1 = "  \"" + refs_name[i] + "\" [shape=\"record\"];\n";
    std::string text2 = "  \"" + refs_name[i] + "\" -> \"" + refs_oid[i].substr(0, 6) + "\";\n";
    DOT::add_text(&cmt_ctx, text1);
    DOT::add_text(&cmt_ctx, text2);
  }
  // HEAD ref
  std::string text_head =
      "  \"HEAD\" [style=\"filled\",fillcolor=\"red\"];\n";
  DOT::add_text(&cmt_ctx, text_head);

  // get all commits` oid
  std::vector<std::string> commits = BASE::iter_commits_and_parents(refs_oid);
  for (const auto &cmt_oid : commits)
  {
    // oid -> Commit
    BASE::commit_ctx ctx = BASE::get_commit(cmt_oid);
    std::cout << cmt_oid;
    if (ctx.parent != "")
    {
      std::cout << " " << ctx.parent << std::endl;
      std::string text = "  \"" + cmt_oid.substr(0, 6) + "\" -> \"" + ctx.parent.substr(0, 6) + "\";\n";
      DOT::add_text(&cmt_ctx, text);
    }
  }
  // ".dot" file create
  std::cout << "\nDOT\n";
  std::string buf = DOT::build_dot(&cmt_ctx);
  std::string file_path = DOT::dot_write_out(buf, "tmp.dot");
  std::cout << buf;

  // subprocess(graphviz) PIPE:popen/pclose
  if (file_path == "")
    return;
  std::string png_file_path = file_path.substr(0, file_path.length() - 3) + "png";
  file_path = std::filesystem::absolute(file_path).string();
  png_file_path = std::filesystem::absolute(png_file_path).string();
  std::string graph_cmd = "dot -Tpng " + file_path + " -o " + png_file_path;
  std::cout << "graph_cmd: " << graph_cmd << std::endl;
  
  FILE *graph_fp = _popen(graph_cmd.c_str(), "w");
  if (graph_fp == NULL)
    return;
  fwrite(buf.data(), buf.size(), 1, graph_fp);
  int res = _pclose(graph_fp);
  printf("k=%d\n", res);
  // return;
  //  subprocess(imageglass) system(no need PIPE)
  std::string image_cmd = "imageglass " + png_file_path;
  system(image_cmd.c_str());

}
