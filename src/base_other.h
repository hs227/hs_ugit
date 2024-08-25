#ifndef BASE_OTHER_H
#define BASE_OTHER_H

#include <iostream>
#include <filesystem>
#include <set>

#include "data.h"



// ignore the specific files when iterate the workshop
inline static bool is_ignore(void *entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry *)entry_;
  // filter the .ugit
  if (entry->path().filename() == ".ugit")
    return true;
  // TODO support the .ugitignore
  return false;
}
// iterate the workshop`s files to write a 'tree' object
// in: dir_path
// out: 'tree'_object`s oid
// side: create a tree_object and (blobs and trees)
static inline std::string write_tree_compact(const std::string &path)
{
  std::set<BASE::wt_iter_node> entries;


  for(const auto& entry:std::filesystem::directory_iterator(path)){
    std::string oid;
    std::string type;
    if(is_ignore((void*)&entry))
      continue;

    if (entry.is_regular_file()){
      // 普通文件
      type = "blob";
      oid = DATA::hash_object(entry.path().string(), "blob");
    }else{
      // 目录
      type = "tree";
      oid =  write_tree_compact(entry.path().string());
    }
    entries.insert({
        type,oid,entry.path().filename().string(),});
  }

  std::string tree_data;
  for (const auto &entry : entries)
  {
    tree_data += entry.type + " " + entry.oid + " " + entry.name + "\n";
  }

  std::string res_oid = DATA::hash_object(tree_data, "tree");
  return res_oid;
}





#endif
