#include "include/CLI11.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "base.h"
#include "dotter.h"
#include "diff.h"
#include "remote.h"

int parse_args(int argc, char *argv[]);
// argument modify
void modifier_name(std::string&,std::string="HEAD");
// other
void print_commit(const std::string &commit_oid, BASE::commit_ctx &ctx,
                  std::vector<std::string> &ref_names, std::vector<DATA::RefValue> &ref_values);
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
void branch(const std::string&,const std::string&);
void status();
void reset(const std::string &);
void show(const std::string &);
void diff(const std::string&,bool);
void merge(const std::string&);
void merge_base(const std::string&,const std::string&);
void fetch(const std::string&);
void push(const std::string&,const std::string&);
void add(const std::vector<std::string>&);

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
  //int option_parsed_count=0;
  std::vector<std::string> in_files_v;// for input a list of files
  bool flag1=false;

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
  sc_checkout->add_option("refvalue",input_file,"branch or commit_oid")->required();
  sc_checkout->callback([&](){
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
  sc_k->callback([&](){
    k();});


  // git branch
  CLI::App *sc_branch=app.add_subcommand("branch","create a branch");
  sc_branch->add_option("name",input_file,"branch name");
  sc_branch->add_option("oid",output_file,"oid");
  sc_branch->callback([&](){
    modifier_name(output_file);
    branch(input_file,output_file);});

  // git status
  CLI::App *sc_status=app.add_subcommand("status","print information");
  sc_status->callback([&](){
    status();});

  // git reset
  CLI::App *sc_reset=app.add_subcommand("reset","reset HEAD deref");
  sc_reset->add_option("oid", input_file, "specify a oid rather than HEAD")->required();
  sc_reset->callback([&](){
    modifier_name(input_file);
    reset(input_file);});

  // git show
  CLI::App*sc_show=app.add_subcommand("show","show the diff");
  sc_show->add_option("oid", input_file, "specify a oid rather than HEAD");
  sc_show->callback([&](){
    modifier_name(input_file);
    show(input_file); });

  // git diff
  CLI::App* sc_diff=app.add_subcommand("diff","diff the current tree with a commit_tree");
  sc_diff->add_option("oid", input_file, "specify a oid rather than HEAD");
  sc_diff->add_flag("--cached",flag1,"if cached");
  sc_diff->callback([&](){
    modifier_name(input_file);
    diff(input_file,flag1);});

  // git merge
  CLI::App* sc_merge=app.add_subcommand("merge","merge with other commit");
  sc_merge->add_option("oid",input_file,"specify a oid rather than HEAD")->required();
  sc_merge->callback([&](){
    modifier_name(input_file);
    merge(input_file);});

  // git merge-base
  CLI::App* sc_merge_base=app.add_subcommand("merge-base","find common commit of two commits");
  sc_merge_base->add_option("oid1",input_file,"first commit oid")->required();
  sc_merge_base->add_option("oid2",output_file,"second commit oid")->required();
  sc_merge_base->callback([&](){
    modifier_name(input_file);
    modifier_name(output_file);
    merge_base(input_file,output_file);});
  
  // git fetch
  CLI::App* sc_fetch=app.add_subcommand("fetch","fetch things from remote repo");
  sc_fetch->add_option("remote",input_file,"remote_path")->required();  
  sc_fetch->callback([&](){
    fetch(input_file);
  });

  // git push
  CLI::App* sc_push=app.add_subcommand("push","push things to remote repo");
  sc_push->add_option("remote",input_file,"remote_path")->required();
  sc_push->add_option("branch",output_file,"branch")->required();
  sc_push->callback([&](){
    push(input_file,output_file);
  });

  // git add
  CLI::App* sc_add=app.add_subcommand("add","add files to cache");
  sc_add->add_option("files",in_files_v,"add a list of files")->required();
  sc_add->callback([&](){
    add(in_files_v);
  });

  CLI11_PARSE(app, argc, argv);

  return 0;
}
// oid/ref ->oid
void modifier_name(std::string &input, std::string default_)
{ 
  if(input==""){
    if(default_=="")
      return;
    input=default_;
  }

  std::string oid=BASE::get_oid(input);
  input=oid;
}

void print_commit(const std::string &cmt_oid, BASE::commit_ctx &ctx,
                  std::vector<std::string> &ref_names, std::vector<DATA::RefValue> &ref_values)
{
  std::cout << "commit " << cmt_oid << " (";
  // print refs
  for (size_t i = 0; i < ref_values.size();)
  {
    if (ref_values[i].value == cmt_oid){
      std::string tmp = std::filesystem::path(ref_names[i]).filename().string();
      std::cout << ((i == 0) ? "" : ", ") << tmp;
      ++i;
    }else{
      ref_names.erase(ref_names.begin() + i);
      ref_values.erase(ref_values.begin() + i);
    }
  }
  std::cout << ")\n";

  std::cout << "tree " << ctx.tree << "\n";
  for(const auto& parent:ctx.parents){
    std::cout<<"parent "<<parent<<std::endl;
  }
  std::cout << " " << ctx.msg << "\n";
  std::cout << std::endl;
}


void init()
{
  BASE::init();
}

void hash_object(const std::string &args)
{
  std::string filepath = args;
  std::string oid = DATA::hash_object(filepath);
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
  BASE::read_tree(oid,true);
}

void commit(const std::string &args)
{
  std::string msg = args;
  std::cout << BASE::commit(msg) << std::endl;
}

void log(const std::string &args)
{
  std::string root_oid=args;


  // get all refs
  std::vector<std::string> ref_names;
  std::vector<DATA::RefValue> ref_values;
  DATA::iter_refs(ref_names, ref_values);

  // get all commits` oid
  std::vector<std::string> oids;
  oids.push_back(root_oid);
  std::vector<std::string> commits = BASE::iter_commits_and_parents(oids);
  
  for (const auto &cmt_oid : commits)
  {
    // oid -> Commit
    BASE::commit_ctx ctx = BASE::get_commit(cmt_oid);
    print_commit(cmt_oid,ctx,ref_names,ref_values);
  }

}

void checkout(const std::string &args)
{ 
  std::string value=args;
  BASE::checkout(value);
}

void tag(const std::string & arg_name, const std::string & arg_oid)
{
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
  std::vector<DATA::RefValue> refs_value;
  std::vector<std::string> refs_oid;
  DATA::iter_refs(refs_name, refs_value,"",false);
  for (size_t i = 0; i < refs_name.size(); ++i)
  {
    std::string name_here;
    std::string ref_here;


    std::cout << refs_name[i] << " : " << refs_value[i].value << std::endl;
    name_here = std::filesystem::path(refs_name[i]).filename().string();
    if (refs_value[i].is_symbolic){
      refs_oid.push_back(DATA::get_ref(refs_value[i].value,true).value);
      ref_here = std::filesystem::path(refs_value[i].value).filename().string();
    }else{
      refs_oid.push_back(refs_value[i].value);
      ref_here = refs_value[i].value.substr(0, oid_len);
    }

    std::string text1 = "  \"" + name_here + "\" [shape=\"record\"];\n";
    std::string text2 = "  \"" + name_here + "\" -> \"" + ref_here + "\";\n";
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
    for(const auto& parent:ctx.parents){
      std::cout << " " << parent << std::endl;
      std::string text = "  \"" + cmt_oid.substr(0, oid_len) + "\" -> \"" + parent.substr(0, oid_len) + "\";\n";
      DOT::add_text(&cmt_ctx, text);
    }
  }
  // ".dot" file create
  std::cout << "\nDOT\n";
  std::string buf = DOT::build_dot(&cmt_ctx);
  std::string file_path = DOT::dot_write_out(buf, "tmp.dot", DATA::LAB_GIT_DIR + "/tmp/graphviz");
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

void branch(const std::string & arg_name, const std::string & arg_oid)
{
  if(arg_name.empty()){
    // show all branches
    std::vector<std::string> branches=BASE::iter_branch_names();
    DATA::RefValue head_value = DATA::get_ref(DATA::HEAD_PATH, false);
    for(const auto& name:branches){
      if(head_value.is_symbolic&&head_value.value==name){
        std::cout<<"*"<<name<<"\n";
        continue;
      }
      std::cout<<" "<<name<<"\n";
    }
  }else{
    // create branch
    BASE::create_branch(arg_name,arg_oid);
    std::cout<< "create branch ["<<arg_name<<"] at "<<arg_oid<<std::endl;
  }
}

void status()
{
  // print current branch name
  std::string head_path=DATA::HEAD_PATH;
  DATA::RefValue head_value=DATA::get_ref(head_path,false);
  if(head_value.is_symbolic){
    std::string tmp=std::filesystem::path(head_value.value).filename().string();
    printf("On branch :%s\n", tmp.c_str());
  }else{
    printf("HEAD detached at :%s\n",head_value.value.c_str());
  }
  // print merge_head
  std::string mhead_path=DATA::MHEAD_PATH;
  DATA::RefValue mhead_value=DATA::get_ref(mhead_path,false);
  if(mhead_value.is_symbolic){
    std::string tmp=std::filesystem::path(mhead_value.value).filename().string();
    printf("mHEAD exisited:%s-%s\n",mhead_value.value.c_str(),tmp.c_str());
  }else{
    printf("mHEAD not existed:%s\n",mhead_value.value.c_str());
  }



  //  print changed files
  std::string head_oid=DATA::get_ref(head_path,true).value;
  std::string head_tree_oid=BASE::get_commit(head_oid).tree;
  //    index tree
  std::string index=BASE::get_index_tree();
  std::string index_tree_oid=index.substr(0,40);
  if(index_tree_oid=="")
    return;
  //    diff
  std::string diff_info = DIFF::iter_changed_file(head_tree_oid, index_tree_oid);
  std::cout<<diff_info<<std::endl;
  
}

void reset(const std::string & args)
{
  std::string value = args;
  BASE::reset(value);
}

void show(const std::string &args)
{
  std::string cmt_oid = args;
  if (cmt_oid == "")
    return;

  // get all refs
  std::vector<std::string> ref_names;
  std::vector<DATA::RefValue> ref_values;
  DATA::iter_refs(ref_names, ref_values);
  // print commit
  BASE::commit_ctx ctx = BASE::get_commit(cmt_oid);
  print_commit(cmt_oid, ctx, ref_names, ref_values);
  // print parents` commits
  for(const auto& parent:ctx.parents){
    BASE::commit_ctx parent_ctx=BASE::get_commit(parent);
    std::string diff_info=DIFF::diff_trees(parent_ctx.tree,ctx.tree); 
    std::cout<<diff_info<<std::endl;   
  }

}

void diff(const std::string & commit_oid,bool flag_cached)
{
  std::string from_tree_oid;
  std::string to_tree_oid;

  if(commit_oid==""&& flag_cached==false){
    /* no arguments : "ugit diff" */
    from_tree_oid=BASE::get_index_tree();
    to_tree_oid=BASE::get_working_tree();
  }else if(commit_oid==""&& flag_cached==true){
    /* --cached : "ugit diff --cached" */
    from_tree_oid=BASE::get_commit(DATA::get_ref(DATA::HEAD_PATH,true).value).tree;
    to_tree_oid=BASE::get_index_tree();
  }else if(commit_oid!=""&&flag_cached==false){
    /* argument and no '--cached' : "ugit diff oid" */
    from_tree_oid=BASE::get_commit(commit_oid).tree;
    to_tree_oid=BASE::get_working_tree();
  }else if(commit_oid!=""&&flag_cached==true){
    /* argument with '--cached' : "ugit diff --cached oid" */
        from_tree_oid=BASE::get_commit(commit_oid).tree;
    to_tree_oid=BASE::get_index_tree();
  }else{
    // NOT REACHED();
    return;
  }

  if(from_tree_oid==""||to_tree_oid=="")
    return;

  //diff
  DIFF::empty_diff_tmp();
  std::string diff_info = DIFF::diff_trees(from_tree_oid,to_tree_oid);
  std::cout<<diff_info<<std::endl;
  DIFF::empty_diff_tmp();
}

void merge(const std::string& args)
{
  std::string oid=args;
  BASE::merge(oid);
}

void merge_base(const std::string& oid1,const std::string& oid2)
{
  std::string msg=BASE::get_merge_base(oid1,oid2);
  std::cout<<msg<<std::endl;
}

void fetch(const std::string& remote_path)
{
  REMOTE::fetch(remote_path);
}

void push(const std::string& remote_path,const std::string& branch)
{
  REMOTE::push(remote_path,REMOTE::REMOTE_REFS_BASE+"/"+branch);
}

void add(const std::vector<std::string>& files)
{
  if(files.empty()){
    std::cout<<"You need to input some files"<<std::endl;
    return;
  }
  
  BASE::add(files);

}


