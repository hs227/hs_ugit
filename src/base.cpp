#include "base.h"
#include <filesystem>
#include <fstream>
#include <sstream>

#include "data.h"
#include "base_other.h"

namespace BASE
{

  std::string write_tree(std::string directory)
  {
    // 遍历目录

    // structure version
    // std::string oid;
    // iter_files(directory,
    //            file_nothing, file_nothing,
    //            is_ignore,
    //            file_hash, file_entries_e,
    //            (void*)&oid);
    // return oid;

    // compact version
    std::string oid = write_tree_compact(directory);
    return oid;
  }

  void empty_current_directory(const std::string& path=DATA::CUR_DIR)
  {
    for(const auto& entry:std::filesystem::directory_iterator(path)){
      if(is_ignore((void*)&entry))
        continue;
      
      if(entry.is_regular_file()){
        // file
        std::filesystem::remove(entry.path());
      }else{
        // dir
        empty_current_directory(entry.path().string());
        if (std::filesystem::is_empty(entry.path().string())){
          std::filesystem::remove(entry.path().string());
        }
      }
    }
  }



  void iter_tree_entries(std::set<wt_iter_node> &entries, const std::string &tree)
  {
    std::string content = DATA::get_object(tree, "tree");
    std::stringstream ss(content);

    while (!ss.eof())
    {
      std::string type;
      std::string oid;
      std::string name;
      ss >> type >> oid >> name;
      if (type != "")
        entries.insert({type, oid, name});
    }
  }

  void get_tree(std::set<gt_iter_node> &results,
                const std::string &oid,
                const std::string &base_path = "")
  {
    results.clear();
    std::set<wt_iter_node> entries;
    iter_tree_entries(entries, oid);

    for (const auto &entry : entries)
    {
      if (entry.type == "blob")
      {
        gt_iter_node node;
        node.src = entry.oid;
        node.dst = base_path + entry.name;
        results.insert(node);
      }
      else if (entry.type == "tree")
      {
        std::set<gt_iter_node> res;
        std::string path = base_path + entry.name + "/";
        get_tree(res, entry.oid, path);
        results.insert(res.begin(), res.end());
      }
    }
  }

  void read_tree(const std::string &oid)
  {
    empty_current_directory();

    std::set<gt_iter_node> entries;
    get_tree(entries, oid); // get all blob in tree
    std::string cur_path = DATA::CUR_DIR;

    for (const auto &entry : entries)
    {
      std::cout << entry.src << " -> " << entry.dst << std::endl;
      std::filesystem::path path(cur_path + "/" + entry.dst);
      std::filesystem::path dir = path.parent_path();
      std::filesystem::path file = path.filename();
      // dir create
      std::filesystem::create_directories(dir);
      // input
      std::string buf = DATA::get_object(entry.src, "blob");
      // output
      std::ofstream out(path, std::ios::binary | std::ios::ate);
      if (!out.is_open())
      {
        continue;
      }
      out.write(buf.data(), buf.size());
      out.close();
    }
  }

  std::string commit(const std::string & msg)
  {
    std::string commit_data = "tree " + write_tree(DATA::CUR_DIR + "/") + "\n";
    commit_data+="\n";
    commit_data+=msg+"\n";
    std::string oid=DATA::hash_object(commit_data,"commit");
    DATA::set_HEAD(oid);
    return oid;
  }
}
