#include"diff.h"

#include<stdlib.h>
#include<filesystem>
#include<fstream>
#include"diff_other.h"
#include"data.h"


namespace DIFF{
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
        //std::cout<<"not change: "<<nodes[i].filepath<<std::endl;
      }
    }
    return diff_content;
  }
  // a concise version of diff_trees
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
          ((diff_str = diff_trees_new_c(nodes[i])) != "")     ||
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

  std::string diff_blobs(const std::string &t_from, const std::string &t_to, std::string output_path)
  {
    std::string file1_path = DATA::OBJECTS_DIR + "/" + t_from;
    std::string file2_path = DATA::OBJECTS_DIR + "/" + t_to;
    std::string pydiff_path = "../extra/py_diff.exe ";
    std::string output_content;
    // prepare environment
    std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
    pydiff_path = std::filesystem::absolute(std::filesystem::path(pydiff_path)).string();
    file1_path=std::filesystem::absolute(std::filesystem::path(file1_path)).string();
    file2_path=std::filesystem::absolute(std::filesystem::path(file2_path)).string();
    output_path=std::filesystem::absolute(std::filesystem::path(output_path)).string();
    // call the 'diff'
    std::string diff_cmd = pydiff_path + " "+ file1_path + " " + file2_path + " -o " + output_path;
    system(diff_cmd.c_str());
    // read the output_content
    std::ifstream in(output_path, std::ios::binary | std::ios::ate);
    if (!in.is_open()){
      std::cout << "diff_blobs:diff output create failed\n";
      return "";
    }
    output_content.resize(in.tellg());
    in.seekg(0);
    in.read(output_content.data(), output_content.size());
    in.close();
    return output_content;
  }
}




