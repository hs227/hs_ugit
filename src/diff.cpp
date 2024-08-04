#include"diff.h"

#include"base.h"
#include"diff_other.h"


namespace DIFF{

  std::string diff_trees(const std::string &t_from, const std::string &t_to)
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
      if (((diff_str = diff_trees_updated_c(nodes[i])) != "") ||
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
}




