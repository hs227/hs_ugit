#ifndef DIFF_OTHER_H
#define DIFF_OTHER_H


#include"diff.h"
#include"data.h"

#include<sstream>

enum class DIFF_TYPE:int{
  diff_update=0,
  diff_new,
  merge_update,
  merge_new,
  nochanged
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
        if(line=="blob\r"||
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

// for merge_trees updated_file case
inline static std::string merge_trees_updated_c(const DIFF::ct_node &node,bool verbose=true)
{
  if(node.oid_idx.size()==2&&
     node.oids[0]!=node.oids[1]){
    std::string updated_msg;
    updated_msg="updated: "+node.filepath+"\n";
    if(verbose){
      std::string merge_content=DIFF::merge_blobs(node.oids[0],node.oids[1],diff_output_name_pre(node.filepath));
      std::string len=std::to_string(merge_content.length());
      updated_msg=len+"\n"+updated_msg+merge_content;
    }
    updated_msg+="\n";
    return updated_msg;
  }
  return "";
}
// for merge_trees new_file case 
inline static std::string merge_trees_new_c(const DIFF::ct_node &node,bool verbose=true){
  if(node.oid_idx.size()==1&&
     node.oid_idx[0]==1){
    std::string new_msg;
    new_msg="new: "+node.filepath+"\n";
    if(verbose){
      std::string merge_content=DIFF::merge_blobs("EMPTY",node.oids[0],diff_output_name_pre(node.filepath));
      std::string len=std::to_string(merge_content.length());
      new_msg=len+"\n"+new_msg+merge_content;    
    }
    new_msg+="\n";
    return new_msg;
  }
  return "";
}
// for merge_trees deleted_file case
inline static std::string merge_trees_deleted_c(const DIFF::ct_node &node)
{
  if(node.oid_idx.size()==1&&
     node.oid_idx[0]==0){
    std::string deleted_msg;
    deleted_msg="deleted: "+node.filepath+"\n";
    std::string len=std::to_string(0);
    deleted_msg=len+"\n"+deleted_msg;
    deleted_msg+="\n";
    return deleted_msg;
  }
  return "";
}
// for merge_trees nochanged_file case 
inline static std::string merge_trees_nochanged_c(const DIFF::ct_node &node,bool verbose=true)
{
  std::string nochanged_msg;
  nochanged_msg="nochanged: "+node.filepath+"\n";
  if(verbose){
    std::string merge_content=DIFF::merge_blobs("NOCHANGED",node.oids[0],diff_output_name_pre(node.filepath));
    std::string len=std::to_string(merge_content.length());
    nochanged_msg=len+"\n"+nochanged_msg+merge_content;    
  }
  nochanged_msg+="\n";
  return nochanged_msg;
}







#endif
