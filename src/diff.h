#ifndef DIFF_H
#define DIFF_H

#include<iostream>
#include<vector>
#include<set>
#include<map>

#include"base.h"

namespace DIFF{
  // compare_tree_node
  // 1.map_version(map=>directory(python))
  using ct_node_p=std::pair<std::string,std::vector<std::string>>;  
  // 2.struct_version
  struct ct_node
  {
    // means same file
    std::string filepath;
    // the index of oids for trees
    std::vector<int> oid_idx;
    // one file in different trees might have different oids
    std::vector<std::string> oids;
    ct_node(){}
    ct_node(const std::string& filepath_,
            const std::vector<int>& oid_idx_,
            const std::vector<std::string>& oids_)
      :filepath(filepath_),oid_idx(oid_idx_),oids(oids_)
    {}
  };

  std::string diff_trees(const std::string& t_from,const std::string& t_to);
  std::vector<ct_node> compare_tree(const std::vector<std::string> tree_oids);
  std::string diff_blobs(const std::string &t_from, const std::string &t_to, std::string output_path = "../resource/python_diff/output.txt");
}

#endif
