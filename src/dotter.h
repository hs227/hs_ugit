#ifndef DOTTER_H
#define DOTTER_H

#include<iostream>
#include<vector>




namespace DOT{

  // digraph_line for dot_ctx
  struct dc_line{
    std::string* from;
    std::string* to;
    std::string descrip;
  };

  // context
  struct dot_ctx
  {
    std::string digraph_name; // graph name
    // mode 1 create realtime
    std::vector<std::string> oid_nodes;// oids examples:oid/oid_name
    std::vector<std::string> ref_nodes;// ref examples:ref/ref_name
    std::vector<dc_line> lines; // from -> to  
    // mode 2 just print texts directly
    std::vector<std::string> texts;
  };

  int init_dotter(dot_ctx *ctx, const std::string &commit_name);
  int add_node(dot_ctx* ctx,const std::string& name);
  int add_oid(dot_ctx* ctx,const std::string& oid_name);
  int add_ref(dot_ctx* ctx,const std::string& ref_name);
  int add_line(dot_ctx* ctx,const std::string& from,const std::string& to);
  int add_text(dot_ctx* ctx,const std::string& newtext);
  std::string build_dot(dot_ctx* ctx,int mode=2);
  std::string dot_write_out(const std::string &buf,const std::string name,const std::string &path = "../resource/graphviz");
}

#endif
