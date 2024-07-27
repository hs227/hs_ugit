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

  void empty_current_directory(const std::string &path = DATA::CUR_DIR)
  {
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
      if (is_ignore((void *)&entry))
        continue;

      if (entry.is_regular_file())
      {
        // file
        std::filesystem::remove(entry.path());
      }
      else
      {
        // dir
        empty_current_directory(entry.path().string());
        if (std::filesystem::is_empty(entry.path().string()))
        {
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

  std::string commit(const std::string &msg)
  {
    std::string commit_data = "tree " + write_tree(DATA::CUR_DIR + "/") + "\n";
    // parent commit
    std::string HEAD = DATA::get_ref("HEAD");
    if (HEAD != "")
      commit_data += "parent " + HEAD + "\n";

    commit_data += "\n";
    commit_data += msg + "\n";
    std::string oid = DATA::hash_object(commit_data, "commit");
    DATA::update_ref("HEAD",oid);
    return oid;
  }
  // oid -> commit_ctx
  commit_ctx get_commit(const std::string &oid)
  {
    std::string commit_data = DATA::get_object(oid, "commit");
    commit_ctx res;
    if (commit_data == "")
      return commit_ctx();
    std::stringstream ss(commit_data);
    std::string tag;
    while (ss){
      ss >> tag;
      if (tag == "tree"){
        ss >> res.tree;
      }else if (tag == "parent") {
        ss >> res.parent;
      }else{
        res.msg = tag;
        break;
      }
    }
    return res;
  }

  void checkout(const std::string &commit_oid)
  {
    // restore workshop
    commit_ctx cxt = get_commit(commit_oid);
    if (cxt.tree == "")
      return;
    std::string &tree_oid = cxt.tree;
    read_tree(tree_oid);

    // set HEAD
    std::string head = DATA::LAB_GIT_DIR + "/" + "HEAD";
    DATA::update_ref(head, commit_oid);
  }

  void create_tag(const std::string & name, const std::string & oid)
  {
    std::string tag_path=DATA::LAB_GIT_DIR+"/refs/tags/"+name;
    DATA::update_ref(tag_path,oid);
  }

  // get the ref path
  std::string get_ref_path(const std::string &name)
  {
    std::vector<std::string> refs_to_try;
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/" + name);
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/refs/" + name);
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/refs/tags/" + name);
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/refs/heads/" + name);

    for (const auto &str : refs_to_try)
    {
      std::string res = DATA::get_ref(str);
      if (res != "")
        return res;
    }
    // failed
    return "";
  }

  // in: name(oid or ref)
  // out:oid
  std::string get_oid(const std::string& name)
  {
    std::string might_oid = get_ref_path(name);
    if (might_oid == "")
    {
      // name is oid
      return name;
    }
    // name is ref
    return might_oid;
  }
  // in: set of refs` oid
  // out: visited commits` oid
  std::vector<std::string> iter_commits_and_parents(const std::vector<std::string> &ref_oids_)
  {
    std::set<std::string> ref_oids;
    std::set<std::string> visited;
    std::vector<std::string> res;
    
    // ref_oids init
    ref_oids.insert(ref_oids_.begin(),ref_oids_.end());
    // refs->commits
    while(!ref_oids.empty()){
      std::string oid=*ref_oids.begin();
      ref_oids.erase(ref_oids.begin());
      if(oid==""||visited.count(oid)>0)
        continue;
      visited.insert(oid);
      res.push_back(oid);
      
      commit_ctx ctx = get_commit(oid);
      ref_oids.insert(ctx.parent);
    }
    return res;
  }

  void create_branch(const std::string &name, const std::string &oid)
  {
    std::string path=DATA::LAB_GIT_DIR+"/"+"refs/heads/"+name;
    
    DATA::update_ref(path,oid);
  }
}
