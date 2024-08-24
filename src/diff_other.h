#ifndef DIFF_OTHER_H
#define DIFF_OTHER_H


#include"diff.h"
#include"data.h"

#include<sstream>
#include<iostream>

enum class DIFF_TYPE:int{
  diff_update=0,
  diff_new,
  merge_update,
  merge_new,
  nochanged,
  diff3_update,
};

// call the real diff executation
// in:t_from(first oid),t_to(second oid),output_path,diff_type
// out:output_path
inline static std::string call_diff(const std::string &t_from, const std::string &t_to, std::string output_path,DIFF_TYPE diff_type)
{
    std::string file1_path = DATA::OBJECTS_DIR + "/" + t_from;
    std::string file2_path = DATA::OBJECTS_DIR + "/" + t_to;
    std::string pydiff_path = "../extra/py_diff.exe ";
    // prepare environment
    std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
    pydiff_path = std::filesystem::absolute(std::filesystem::path(pydiff_path)).string();
    file1_path=std::filesystem::absolute(std::filesystem::path(file1_path)).string();
    file2_path=std::filesystem::absolute(std::filesystem::path(file2_path)).string();
    output_path=std::filesystem::absolute(std::filesystem::path(output_path)).string();
    // call the 'diff'
    std::string diff_cmd;
    switch(diff_type){
      case DIFF_TYPE::diff_update:// call the 'diff'
        diff_cmd = pydiff_path + " " + file1_path + " " + file2_path + " -o " + output_path;
        break;
      case DIFF_TYPE::diff_new:// call the 'diff -el'
        diff_cmd = pydiff_path + " " + file1_path + " " + file2_path + " -o " + output_path + " -el";
        break;
      case DIFF_TYPE::merge_update:// call the 'diff -DHEAD -u'
        diff_cmd = pydiff_path + " " + file1_path + " " + file2_path + " -o " + output_path +" -DHEAD -u";
        break;
      case DIFF_TYPE::merge_new:// call the ' diff -DHEAD -u -el'
        diff_cmd = pydiff_path + " " + file1_path + " " + file2_path + " -o " + output_path +" -DHEAD -u -el";
        break;
      case DIFF_TYPE::nochanged:// call the ' diff -DHEAD -el'
        diff_cmd = pydiff_path + " " + file1_path + " " + file2_path + " -o " + output_path +" -DHEAD -el";
        break;
      default:
        return "";
    }

    system(diff_cmd.c_str());
    return output_path;
}
// call the real diff3 executation
// in: t_base,t_HEAD,t_MHEAD
// out: output_path
inline static std::string call_diff3(const std::string &t_base,const std::string &t_HEAD,const std::string& t_MHEAD, std::string output_path,DIFF_TYPE diff_type)
{
  std::string file1_path  = DATA::OBJECTS_DIR + "/" + t_HEAD;
  std::string base_path   = DATA::OBJECTS_DIR + "/" + t_base;
  std::string file2_path  = DATA::OBJECTS_DIR + "/" + t_MHEAD;
  std::string pydiff_path = "../extra/py_diff3.exe ";
  // prepare environment
  std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
  pydiff_path = std::filesystem::absolute(std::filesystem::path(pydiff_path)).string();
  file1_path=std::filesystem::absolute(std::filesystem::path(file1_path)).string();
  base_path=std::filesystem::absolute(std::filesystem::path(base_path)).string();
  file2_path=std::filesystem::absolute(std::filesystem::path(file2_path)).string();
  output_path=std::filesystem::absolute(std::filesystem::path(output_path)).string();
  // call the 'py_diff3 --merge versionA base versionB -L HEAD BASE MHEAD'
  std::string diff_cmd;
  diff_cmd = pydiff_path + " --merge -L HEAD BASE MHEAD " + file1_path + " " + base_path+ " " + file2_path + " -o " + output_path;

  system(diff_cmd.c_str());
  return output_path;
}

// prepare to get tmp/output_filename 
inline static std::string diff_output_name_pre(const std::string filename)
{
  std::string out_filename=DATA::LAB_GIT_DIR + "/tmp/python_diff/diff_output_";
  for(const auto& c:filename){
    if(c=='/'||c=='\\')
      out_filename+='_';
    else
      out_filename+=c;
  }
  return out_filename;
}
// post-process of call_diff
// 1.delete the object_type in diff_file
// in:filepath_,diff_or_merge(t:diff:f:merge)
// out:o_filepath
inline static std::string diff_output_content_post(const std::string filepath_,bool diff_or_merge)
{
  std::string r_filepath=std::filesystem::absolute(filepath_).string();
  std::string o_filepath=r_filepath+"post";
  std::string output_content_in;
  std::string output_content_out;

  
  // read diff_output content from 'tmp/python_diff/'`s file
  std::ifstream in(r_filepath,std::ios::binary|std::ios::ate);
  output_content_in.resize(in.tellg());
  in.seekg(0);
  in.read(output_content_in.data(),output_content_in.size());
  in.close();

  // post_step 1
  if(diff_or_merge){
    // diff
    std::stringstream ss(output_content_in);
    std::string line;
    bool flag=true;
    while(std::getline(ss,line)){
      if(flag){
        if(line==" blob\r"||
          line=="-blob\r"||
          line=="+blob\r"){
            flag=false;
        }
        continue;
      }
      output_content_out+=line+"\n";
    }
  }else{
    // merge
    std::stringstream ss(output_content_in);
    std::string line;
    bool flag=true;
    while(std::getline(ss,line)){
      if(flag&&line=="blob\r"){
        flag=false;
        continue;
      }
      output_content_out+=line+"\n";
    }
  }

  // write the processed_content return to file
  std::ofstream out(o_filepath,std::ios::binary);
  out.write(output_content_out.data(),output_content_out.size());
  out.close();

  return o_filepath;
}

// for diff_trees updated_file case
inline static std::string diff_trees_updated_c(const DIFF::ct_node& node,bool verbose=true)
{
  if(node.oid_idx.size()==2&&
     node.oids[0]!=node.oids[1]){
      std::string updated_msg;
      updated_msg="updated: "+node.filepath+"\n";
      if(verbose)
        updated_msg+= DIFF::diff_blobs(node.oids[0], node.oids[1], diff_output_name_pre(node.filepath));
      updated_msg+="\n";
      return updated_msg;
  }
  return "";
}
// for diff_trees new_file case
inline static std::string diff_trees_new_c(const DIFF::ct_node &node,bool verbose=true)
{
  if(node.oid_idx.size()==1&&
     node.oid_idx[0]==1){
      std::string new_msg;
      new_msg="new: "+node.filepath+"\n";
      if(verbose)
        new_msg+=DIFF::diff_blobs("EMPTY",node.oids[0],diff_output_name_pre(node.filepath));
      new_msg+="\n";
      return new_msg;
  }
  return "";
}
// for diff_trees deleted_file case
inline static std::string diff_trees_deleted_c(const DIFF::ct_node &node)
{
  if(node.oid_idx.size()==1&&
     node.oid_idx[0]==0){
      std::string deleted_msg;
      deleted_msg="deleted: "+node.filepath+"\n";
      deleted_msg+="\n";
      return deleted_msg;
  }
  return "";
}
// for diff_trees nochanged_file case 
inline static std::string diff_trees_nochanged_c(const DIFF::ct_node &node,bool verbose=true)
{
  std::string nochanged_msg;
  nochanged_msg="nochanged: "+node.filepath+"\n";
  if(verbose)
    nochanged_msg+=DIFF::diff_blobs("NOCHANGED",node.oids[0],diff_output_name_pre(node.filepath));
  nochanged_msg+="\n";
  return nochanged_msg;
}

/* for merge_trees all cases descriptions:
  caseXXX|base|HEAD|MHEAD|
         |   0|   0|    0| impossible case
         |   0|   0|    1| newfile
         |   0|   1|    0| newfile
         |   0|   1|    1| diff2_update
         |   1|   0|    0| delete
         |   1|   0|    1| diff2_update
         |   1|   1|    0| diff2_update
         |   1|   1|    1| diff3_update
*/

#define MT_STUB_CASE(x) \
  case 0b ## x:            \
    res=merge_trees_case ## x(node);\
    break
inline static std::string merge_trees_case001(const DIFF::ct_node& node);
inline static std::string merge_trees_case010(const DIFF::ct_node& node);
inline static std::string merge_trees_case011(const DIFF::ct_node& node);
inline static std::string merge_trees_case100(const DIFF::ct_node& node);
inline static std::string merge_trees_case101(const DIFF::ct_node& node);
inline static std::string merge_trees_case110(const DIFF::ct_node& node);
inline static std::string merge_trees_case111(const DIFF::ct_node& node);

// for merge_trees case 
inline static std::string merge_trees_reception(const DIFF::ct_node& node)
{
  int status=0;
  std::string res;
  for(const auto& idx:node.oid_idx){
    status|=1<<(2-idx);
  }
  switch (status){
    MT_STUB_CASE(001);
    MT_STUB_CASE(010);
    MT_STUB_CASE(011);
    MT_STUB_CASE(100);
    MT_STUB_CASE(101);
    MT_STUB_CASE(111);
  default:
    break;
  }
  return res;
}

// |base|HEAD|MHEAD|
// |   0|   0|    1| newfile
inline static std::string merge_trees_case001(const DIFF::ct_node& node)
{
  std::string msg;
  std::string content;
  msg="new: "+node.filepath+"\n";
  content=DIFF::merge_blobs("","",node.oids[0],diff_output_name_pre(node.filepath),0b001);
  msg=std::to_string(content.size())+"\n"+msg+content;
  msg+="\n";
  return msg;
}
// |base|HEAD|MHEAD|
// |   0|   1|    0| newfile
inline static std::string merge_trees_case010(const DIFF::ct_node& node)
{
  std::string msg;
  std::string content;
  msg="new: "+node.filepath+"\n";
  content=DIFF::merge_blobs("",node.oids[0],"",diff_output_name_pre(node.filepath),0b010);
  msg=std::to_string(content.size())+"\n"+msg+content;
  msg+="\n";
  return msg;
}
//|base|HEAD|MHEAD|
//|   0|   1|    1| diff2_update
inline static std::string merge_trees_case011(const DIFF::ct_node& node)
{
  std::string msg;
  std::string content;
  msg="update: "+node.filepath+"\n";
  content=DIFF::merge_blobs("",node.oids[0],node.oids[1],diff_output_name_pre(node.filepath),0b011);
  msg=std::to_string(content.size())+"\n"+msg+content;
  msg+="\n";
  return msg;
}
// |base|HEAD|MHEAD|
// |   1|   0|    0| delete
inline static std::string merge_trees_case100(const DIFF::ct_node& node)
{
  std::string msg;
  msg="delete: "+node.filepath+"\n";
  msg="0\n"+msg;
  msg+="\n";
  return msg;
}
// |base|HEAD|MHEAD|
// |   1|   0|    1| diff2_update
inline static std::string merge_trees_case101(const DIFF::ct_node& node)
{
  std::string msg;
  std::string content;
  msg+="update: "+node.filepath+"\n";
  content=DIFF::merge_blobs(node.oids[0],"",node.oids[1],diff_output_name_pre(node.filepath),0b101);
  msg=std::to_string(content.size())+"\n"+msg+content;
  msg+="\n";
  return msg;
}
// |base|HEAD|MHEAD|
// |   1|   1|    0| diff2_update
inline static std::string merge_trees_case110(const DIFF::ct_node& node)
{
  std::string msg;
  std::string content;
  msg+="update: "+node.filepath+"\n";
  content=DIFF::merge_blobs(node.oids[0],node.oids[1],"",diff_output_name_pre(node.filepath),0b110);
  msg=std::to_string(content.size())+"\n"+msg+content;
  msg+="\n";
  return msg;
}
// |base|HEAD|MHEAD|
// |   1|   1|    1| diff3_update
inline static std::string merge_trees_case111(const DIFF::ct_node& node)
{
  std::string msg;
  std::string content;
  msg+="update: "+node.filepath+"\n";
  content=DIFF::merge_blobs(node.oids[0],node.oids[1],node.oids[2],diff_output_name_pre(node.filepath),0b111);
  msg=std::to_string(content.size())+"\n"+msg+content;
  msg+="\n";
  return msg;
}



#endif
