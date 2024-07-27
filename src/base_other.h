#ifndef BASE_OTHER_HPP
#define BASE_OTHER_HPP

#include <iostream>
#include <filesystem>
#include <set>

#include "data.h"

// for tree hash
struct wt_iter_node
{
  std::string type;
  std::string oid;
  std::string name;

  bool operator<(const wt_iter_node &rhs) const
  {
    return name.compare(rhs.name) <= 0;
  }
};
// for get tree
struct gt_iter_node
{
  std::string src;// oid in objects
  std::string dst;// path in workshop

  bool operator<(const gt_iter_node& rhs) const
  {
    return dst.compare(rhs.dst)<=0;
  }
};


// for file_hash parameter passing
struct wt_fh_node
{
  std::filesystem::directory_entry *entry;
  std::set<wt_iter_node> *entries;
  std::string *tree_oid; // if tree type, needed
  wt_fh_node() {}
  wt_fh_node(std::filesystem::directory_entry *entry_,
             std::set<wt_iter_node> *entries_,
             std::string *tree_oid_)
      : entry(entry_), entries(entries_), tree_oid(tree_oid_)
  {
  }
};

struct wt_fee_node
{
  std::set<wt_iter_node> *entries;
  std::string *path;
  std::string *res_oid;
  wt_fee_node() {}
  wt_fee_node(std::set<wt_iter_node> *entries_, std::string *path_, std::string *res_oid_)
      : entries(entries_), path(path_), res_oid(res_oid_)
  {
  }
};

// dir iter funcs
inline static void file_print(void *entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry *)entry_;
  if (entry->is_regular_file())
  {
    // 普通文件
    std::cout << "File: " << entry->path() << std::endl;
  }
  else
  {
    // 目录
    std::cout << "Directory: " << entry->path() << std::endl;
  }
}
inline static void file_nothing(void *entry_)
{
}
inline static void file_hash(void *entry_)
{
  wt_fh_node *node = (wt_fh_node *)entry_;

  std::string oid;
  std::string type;

  if (node->entry->is_regular_file())
  {
    // 普通文件
    type = "blob";
    oid = DATA::hash_object(node->entry->path().string(), "blob");
  }else{
    // 目录
    type = "tree";
    oid = *node->tree_oid;
  }
  node->entries->insert({
      type,
      oid,
      node->entry->path().filename().string(),
  });
}
inline static void file_entries_e(void *entry_)
{
  wt_fee_node *node = (wt_fee_node *)entry_;
  std::string data;

  for (const auto &entry : *node->entries)
  {
    data += entry.type + " " + entry.oid + " " + entry.name + "\n";
  }

  *node->res_oid = DATA::hash_object(data, "tree");
}

inline static bool is_ignore(void *entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry *)entry_;
  // filter the .ugit
  if (entry->path().filename() == ".ugit")
    return true;
  // TODO support the .ugitignore
  return false;
}
// dir iter
// func_start()
// func_head(before the recur)
// filter:filts the files or dirs
// func_tail(after the recur)
// func_end()
inline static void iter_files(const std::string &path,
                              void func_s(void *) = file_nothing,
                              void func_h(void *) = file_print,
                              bool filter(void *) = is_ignore,
                              void func_t(void *) = file_print,
                              void func_e(void *) = file_nothing,
                              void *res = NULL)
{
  std::set<wt_iter_node> entries;
  std::string oid;


  func_s(NULL);

  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (filter((void *)&entry))
      continue;

    struct wt_fh_node node((std::filesystem::directory_entry *)&entry, &entries, &oid);

    func_h((void *)&node);
    if (entry.is_directory())
    {
      // 递归遍历
      iter_files(entry.path().string(), func_s, func_h, filter, func_t, func_e, &oid);
    }
    func_t((void *)&node);
  }

  struct wt_fee_node node(&entries, (std::string *)&path, (std::string *)res);
  func_e((void *)&node);
}

static inline std::string write_tree_compact(const std::string &path)
{
  std::set<wt_iter_node> entries;


  for(const auto& entry:std::filesystem::directory_iterator(path)){
    std::string oid;
    std::string type;
    if(is_ignore((void*)&entry))
      continue;

    if (entry.is_regular_file())
    {
      // 普通文件
      type = "blob";
      oid = DATA::hash_object(entry.path().string(), "blob");
    }
    else
    {
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
