#include"dotter.h"

#include<fstream>
#include<filesystem>

namespace DOT
{
  int init_dotter(dot_ctx* ctx,const std::string& commit_name)
  {
    if(ctx==NULL){
      return -1;
    }
    ctx->digraph_name = commit_name;
    ctx->oid_nodes.resize(0);
    ctx->ref_nodes.resize(0);
    ctx->lines.resize(0);
    ctx->texts.resize(0);
    return 0;
  }

  int add_node(dot_ctx *ctx, const std::string &name)
  {
    if(ctx==NULL){
      return -1;
    }
    size_t pos;
    if((pos=name.find("oid/"))==0){
      // "oid/*"
      std::string substr_ = name.substr(pos + 4);
      return add_oid(ctx, substr_);
    }
    if((pos=name.find("ref/"))==0){
      // "ref/*"
      std::string substr_=name.substr(pos+4);
      return add_ref(ctx,substr_);
    }
    return -1;
  }

  int add_oid(dot_ctx *ctx, const std::string &oid_name)
  {
    if(ctx==NULL){
      return -1;
    }
    ctx->oid_nodes.push_back(oid_name);
    return 0;
  }

  int add_ref(dot_ctx *ctx, const std::string &ref_name)
  {
    if(ctx==NULL){
      return -1;
    }
    ctx->ref_nodes.push_back(ref_name);
    return 0;
  }


  inline static std::vector<std::string>::iterator
    line_str_find(std::vector<std::string>& vec,
                  std::string node)
  {
    using namespace std;
    using it_type=vector<string>::iterator;
    it_type it;
    for(it=vec.begin();it!=vec.end();++it){
      if(*it==node)
        return it;
    }
    
    return vec.end();
  }



  int add_line(dot_ctx *ctx, 
               const std::string &from, 
               const std::string &to,
               const std::string descrip)
  {
    if(ctx==NULL){
      return -1;
    }
    size_t f_pos,f_type,t_pos,t_type;
    std::vector<std::string>::iterator f_it,t_it;
    std::string f_sub,t_sub;
    // check from_node
    if((f_pos=from.find("oid/"))==0){
      f_type=1;// oid
      f_sub=from.substr(f_pos+4);
      f_it = line_str_find(ctx->oid_nodes,f_sub);
    }else if((f_pos=from.find("ref/"))==0){
      f_type=2;// ref
      f_sub=from.substr(f_pos+4);
      f_it=line_str_find(ctx->ref_nodes,f_sub);
    }else{
      return -1;
    }
    // check to_node
    if((t_pos=to.find("oid/"))==0){
      t_type=1;// oid
      t_sub=to.substr(t_pos+4);
      t_it=line_str_find(ctx->oid_nodes,t_sub);
    }else if((t_pos=to.find("ref/"))==0){
      t_type=2;// ref
      t_sub=to.substr(t_pos+4);
      t_it=line_str_find(ctx->ref_nodes,t_sub);
    }else{
      return -1;
    }

    // "from" doesn't exist or "to" doesn't exist
    if(f_it==ctx->oid_nodes.end()||f_it==ctx->ref_nodes.end()||
      t_it==ctx->oid_nodes.end()||t_it==ctx->ref_nodes.end()){
        return -1;
    }
    dc_line line_node;
    // set from_node
    line_node.from=&*f_it;
    // set to_node
    line_node.to=&*t_it;
    // set descrip
    line_node.descrip=descrip;
    ctx->lines.push_back(line_node);
    return 0;
  }

  int add_text(dot_ctx *ctx, const std::string &newtext)
  {
    if(ctx==NULL){
      return -1;
    }
    ctx->texts.push_back(newtext);
    return 0;
  }

  static std::string build_dot_m2(dot_ctx* ctx)
  {
    std::string buf;
    buf = "digraph " + ctx->digraph_name + "{\n";
    // buf+="  HEAD;\n";
    // buf+="  HEAD -> oid1;\n";
    // buf+="  oid1 -> oid2;\n";
    // buf+="  ref;\n";
    // buf+="  ref -> oid3;\n";
    // buf+="  oid3 -> oid2;\n";
    for(const auto& entry:ctx->texts){
      buf+=entry;
    }
    buf+="}\n";
    return buf;
  }

  std::string build_dot(dot_ctx *ctx,int mode)
  {
    if(ctx==NULL)
      return "";
    if(mode==1){
      // TODO
      return "";
    }else if(mode ==2){
      return build_dot_m2(ctx);
    }else{
      return "";
    }
  }

  std::string dot_write_out(const std::string &buf, const std::string name, const std::string &path)
  {
    std::string file_path=path+"/"+name;

    std::filesystem::create_directories(
      std::filesystem::path(file_path).parent_path());

    std::ofstream out(file_path);
    if(!out.is_open()){
      return "";
    }

    out.write(buf.data(),buf.size());
    out.close();
    return file_path;
  }
}





