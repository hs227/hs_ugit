#ifndef DIFF_OTHER_H
#define DIFF_OTHER_H


#include"diff.h"

// for diff_trees updated_file case
inline static std::string diff_trees_updated_c(const DIFF::ct_node& node)
{
  if(node.oid_idx.size()==2&&
     node.oids[0]!=node.oids[1]){
      std::string updated_msg;
      updated_msg="updated: "+node.filepath;
      return updated_msg;
  }
  return "";
}
// for diff_trees new_file case
inline static std::string diff_trees_new_c(const DIFF::ct_node &node)
{
  if(node.oid_idx.size()==1&&
     node.oid_idx[0]==1){
      std::string new_msg;
      new_msg="new: "+node.filepath;
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
      deleted_msg="deleted: "+node.filepath;
      return deleted_msg;
  }
  return "";
}

#endif
