#include"diff.h"

#include<stdlib.h>
#include<filesystem>
#include<fstream>
#include"diff_other.h"
#include"data.h"


namespace DIFF{
  // iter each tree, and get all the diffs_content
  // in: t_from(tree_oid),t_to(tree_oid)
  // out: diff_content
  std::string diff_trees(const std::string &t_from, const std::string &t_to)
  {
    std::vector<std::string> tree_oids({t_from,t_to});
    std::vector<ct_node> nodes=compare_tree(tree_oids);

    std::string diff_content;
    
    //  diff:updated_file , new_file or deleted_file
    //    updated_file:different oid
    //    new_file:only (idx=1)_oid
    //    deleted_file: only (idx=0)_oid
    const size_t nodes_size=nodes.size();
    for(size_t i=0;i<nodes_size;++i){
      std::string diff_str;
      if (((diff_str = diff_trees_updated_c(nodes[i])) != "") ||
          ((diff_str = diff_trees_new_c(nodes[i])) != "")     ||
          ((diff_str = diff_trees_deleted_c(nodes[i])) != "")) {
        // updated or new or deleted
        //std::cout<<diff_str<<std::endl;
        diff_content+=diff_str;
      }else{
        //std::cout<<"nochanged: "<<nodes[i].filepath<<std::endl;
        diff_str=diff_trees_nochanged_c(nodes[i]);
        diff_content+=diff_str;
      }
    }
    return diff_content;
  }
  // actually, a concise version of diff_trees
  std::string iter_changed_file(const std::string &t_from, const std::string &t_to)
  {
    std::vector<std::string> tree_oids({t_from,t_to});
    std::vector<ct_node> nodes=compare_tree(tree_oids);

    std::string diff_info;
    
    //  diff:updated_file , new_file or deleted_file
    //    updated_file:different oid
    //    new_file:only (idx=1)_oid
    //    deleted_file: only (idx=0)_oid
    const size_t nodes_size=nodes.size();
    for(size_t i=0;i<nodes_size;++i){
      std::string diff_str;
      if (((diff_str = diff_trees_updated_c(nodes[i],false)) != "") ||
          ((diff_str = diff_trees_new_c(nodes[i],false)) != "")     ||
          ((diff_str = diff_trees_deleted_c(nodes[i])) != "")) {
        // updated or new or deleted
        //std::cout<<diff_str<<std::endl;
        diff_info+=diff_str;
      }else{
        //std::cout<<"not change: "<<nodes[i].filepath<<std::endl;
      }
    }
    return diff_info;
  }

  // compare all the trees` files,to find the
  // differences that means the same path with different oid.
  // one path means one file; different oids mean diffrent content.
  // in: tree_oids
  // out: diffs
  std::vector<ct_node> compare_tree(const std::vector<std::string> tree_oids)
  {

    std::map<std::string,
             std::pair<std::vector<int>, 
                       std::vector<std::string>
                      >
            >diff_map;
    std::vector<ct_node> diffs;

    int tree_idx=0;
    // iterate all trees
    for (const std::string &tree_oid : tree_oids)
    {
      std::set<BASE::gt_iter_node> tree_entries;
      BASE::get_tree(tree_entries,tree_oid);
      // all trees
      for(const auto& entry:tree_entries){
        diff_map[entry.dst].first.push_back(tree_idx);
        diff_map[entry.dst].second.push_back(entry.src);
      }
      tree_idx++;
    }

    // for output
    for(const auto& p:diff_map){
      diffs.emplace_back(p.first,p.second.first,p.second.second);
    }
    return diffs;
  }
  // call the diff
  // in: t_from,t_to,output_path
  // out: output_content
  std::string diff_blobs(const std::string &t_from, const std::string &t_to, std::string output_path)
  {
    DIFF_TYPE type=DIFF_TYPE::diff_update;
    if(t_from=="EMPTY"){
      type=DIFF_TYPE::diff_new;
    }else if (t_from=="NOCHANGED"){
      type=DIFF_TYPE::nochanged;
    }
    std::string r_filepath=call_diff(t_from,t_to,output_path,type);
    std::string o_filepath=diff_output_content_post(r_filepath,true);
    std::string output_content;

    std::ifstream in(o_filepath,std::ios::binary|std::ios::ate);
    output_content.resize(in.tellg());
    in.seekg(0);
    in.read(output_content.data(),output_content.size());
    in.close();

    return output_content;
  }
  // iter each tree, and get all the merge_content
  // in: t_base(base_tree_oid),t_HEAD(HEAD_tree_oid),t_other(other_tree_oid)
  // out: tree_merge_content
  std::string merge_trees(const std::string& t_base,const std::string& t_HEAD,const std::string& t_other)
  {
    std::vector<std::string> tree_oids({t_base,t_HEAD,t_other});
    std::vector<ct_node> nodes=compare_tree(tree_oids);

    std::string merge_content;
    
    const size_t nodes_size=nodes.size();
    for(size_t i=0;i<nodes_size;++i){
      std::string diff_str;
      diff_str=merge_trees_reception(nodes[i]);
      merge_content+=diff_str;
    }
    return merge_content;
  }
  // in: t_base,t_HEAD,t_MHEAD
  // out: output_content
  std::string merge_blobs(const std::string &t_base, const std::string &t_HEAD,const std::string& t_MHEAD, std::string output_path,int flag)
  {
    std::string r_filepath;
    switch(flag){
      case 0b001:
        r_filepath=call_diff("EMPTY",t_MHEAD,output_path,DIFF_TYPE::merge_new);break;
      case 0b010:
        r_filepath=call_diff("EMPTY",t_HEAD,output_path,DIFF_TYPE::merge_new);break;
      case 0b011:
        r_filepath=call_diff(t_HEAD,t_MHEAD,output_path,DIFF_TYPE::merge_update);break;
      case 0b101:
        r_filepath=call_diff(t_base,t_MHEAD,output_path,DIFF_TYPE::merge_update);break;
      case 0b110:
        r_filepath=call_diff(t_base,t_HEAD,output_path,DIFF_TYPE::merge_update);break;
      case 0b111:
        r_filepath=call_diff3(t_base,t_HEAD,t_MHEAD,output_path,DIFF_TYPE::diff3_update);break;
      default:
        return "";
    }
    
    std::string o_filepath=diff_output_content_post(r_filepath,false);
    std::string output_content;

    std::ifstream in(o_filepath,std::ios::binary|std::ios::ate);
    output_content.resize(in.tellg());
    in.seekg(0);
    in.read(output_content.data(),output_content.size());
    in.close();

    return output_content;
  }
  // clean the ".ugit/tmp/python_diff"
  void empty_diff_tmp()
  {
    std::string tmp_dir_path=DATA::LAB_GIT_DIR+"/tmp/python_diff";
    for(const auto& entry:std::filesystem::recursive_directory_iterator(tmp_dir_path)){
      if(entry.is_regular_file()){
        std::filesystem::remove(entry);
      }
    }
  }  


}




