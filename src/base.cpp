#include "base.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include<functional>

#include "data.h"
#include "base_other.h"

namespace BASE
{
  // in: dir_path
  // out: tree_object_oid
  std::string write_tree(std::string directory)
  {
    // 遍历目录
    // compact version
    std::string oid = write_tree_compact(directory);
    return oid;
  }

  // clean the dir`s files
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
  // in: tree_oid
  // out:entries
  void iter_tree_entries(std::set<wt_iter_node> &entries, const std::string &tree_oid)
  {
    std::string content = DATA::get_object(tree_oid, "tree");
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

  // get all the blob in tree_object
  // in:tree_oid,base_path(for recur)
  // out:results
  void get_tree(std::set<gt_iter_node> &results,
                const std::string &tree_oid,
                const std::string &base_path)
  {
    results.clear();
    std::set<wt_iter_node> entries;
    iter_tree_entries(entries, tree_oid);

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
  // fill the workshop by the files in tree
  // in: tree_oid
  // side: workshop flush
  void read_tree(const std::string &tree_oid)
  {
    empty_current_directory();

    std::set<gt_iter_node> entries;
    get_tree(entries, tree_oid); // get all blob in tree
    std::string cur_path = DATA::CUR_DIR;
    // iterate the tree and create each file
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
  // commit with msg
  // in:msg(comment)
  // out: commit_object_oid
  std::string commit(const std::string &msg)
  {
    std::string commit_data = "tree " + write_tree(DATA::CUR_DIR + "/") + "\n";
    std::string head_path = DATA::HEAD_PATH;
    DATA::RefValue head_value = DATA::get_ref(head_path, false);

    // parent commit
    std::string parent_oid = DATA::get_ref(head_path).value;
    if (parent_oid != "")
      commit_data += "parent " + parent_oid + "\n";

    commit_data += "\n";
    commit_data += msg + "\n";
    std::string commit_oid = DATA::hash_object(commit_data, "commit");

    if (head_value.is_symbolic)
    {
      std::string deref_path = head_value.value;
      DATA::update_ref(deref_path, DATA::RefValue(false, commit_oid));
    }
    else
    {
      DATA::update_ref(head_path, DATA::RefValue(false, commit_oid));
    }
    return commit_oid;
  }
  // oid -> commit_ctx
  // in: commit_oid
  // out: commit_ctx
  commit_ctx get_commit(const std::string &commit_oid)
  {
    std::string commit_data = DATA::get_object(commit_oid, "commit");
    commit_ctx res;
    if (commit_data == "")
      return commit_ctx();
    std::stringstream ss(commit_data);
    std::string tag;
    while (ss)
    {
      ss >> tag;
      if (tag == "tree")
      {
        ss >> res.tree;
      }
      else if (tag == "parent")
      {
        ss >> res.parent;
      }
      else
      {
        res.msg = tag;
        break;
      }
    }
    return res;
  }
  // in:value(name or oid)
  void checkout(const std::string &value)
  {
    std::string commit_oid = get_oid(value);
    // workshop flush
    commit_ctx cxt = get_commit(commit_oid);
    if (cxt.tree == "")
      return;
    std::string &tree_oid = cxt.tree;
    read_tree(tree_oid);

    // set HEAD
    std::string head_path = DATA::LAB_GIT_DIR + "/" + "HEAD";
    DATA::RefValue head_value;
    if (is_branch(value))
    {
      head_value = DATA::RefValue(true, "refs/heads/" + value);
    }
    else
    {
      head_value = DATA::RefValue(false, commit_oid);
    }

    DATA::update_ref(head_path, head_value, false);
  }
  // create a ref called tag
  // in: refname,oid
  // side:create a tag in 'refs/tags/'
  void create_tag(const std::string &refname, const std::string &oid)
  {
    std::string tag_path = DATA::LAB_GIT_DIR + "/refs/tags/" + refname;
    DATA::update_ref(tag_path, DATA::RefValue(false, oid));
  }

  // get the intact_refname (the intact ref_path)
  // in: refname
  // out: intact_refname
  std::string get_ref_path(const std::string &refname)
  {
    std::vector<std::string> refs_to_try;
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/" + refname);
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/refs/" + refname);
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/refs/tags/" + refname);
    refs_to_try.push_back(DATA::LAB_GIT_DIR + "/refs/heads/" + refname);

    for (const auto &str : refs_to_try)
    {
      std::string value = DATA::get_ref(str, false).value;
      if (value != "")
        return str;
    }
    // failed
    return "";
  }

  // in: name(oid or ref)
  // out:oid
  std::string get_oid(const std::string &name)
  {
    std::string refname = get_ref_path(name);
    if (refname == "")
    {
      // name is oid
      return name;
    }
    // name is ref
    return DATA::get_ref(refname, true).value;
  }
  // in: set of refs` oid
  // out: visited commits` oid
  std::vector<std::string> iter_commits_and_parents(const std::vector<std::string> &ref_oids_)
  {
    std::set<std::string> ref_oids;
    std::set<std::string> visited;
    std::vector<std::string> res;

    // ref_oids init
    ref_oids.insert(ref_oids_.begin(), ref_oids_.end());
    // refs->commits
    while (!ref_oids.empty())
    {
      std::string oid = *ref_oids.begin();
      ref_oids.erase(ref_oids.begin());
      if (oid == "" || visited.count(oid) > 0)
        continue;
      visited.insert(oid);
      res.push_back(oid);

      commit_ctx ctx = get_commit(oid);
      ref_oids.insert(ctx.parent);
    }
    return res;
  }
  // create a ref called branch
  // in: branchname,oid
  // side: create a branch in 'refs/heads/'
  void create_branch(const std::string &branchname, const std::string &oid)
  {
    std::string path = DATA::LAB_GIT_DIR + "/" + "refs/heads/" + branchname;

    DATA::update_ref(path, DATA::RefValue(false, oid));
  }

  bool is_branch(const std::string &branchname)
  {
    std::string path = DATA::LAB_GIT_DIR + "/" + "refs/heads/" + branchname;
    return !DATA::get_ref(path).value.empty();
  }

  void init()
  {
    DATA::init();
    // set the HEAD
    DATA::update_ref(DATA::HEAD_PATH, DATA::RefValue(true, "refs/heads/master"));
  }

  std::vector<std::string> iter_branch_names(void)
  {
    std::vector<std::string> branch_names;
    std::vector<DATA::RefValue> branch_values;
    DATA::iter_refs(branch_names, branch_values, "heads");
    return branch_names;
  }
  // in:value(name or oid)
  void reset(const std::string &value)
  {
    std::string commit_oid = get_oid(value);
    // workshop flush
    commit_ctx cxt = get_commit(commit_oid);
    if (cxt.tree == "")
      return;
    std::string &tree_oid = cxt.tree;
    read_tree(tree_oid);

    // set HEAD
    std::string head_path = DATA::LAB_GIT_DIR + "/" + "HEAD";
    DATA::RefValue head_value;
    if (is_branch(value)){
      head_value = DATA::RefValue(true, "refs/heads/" + value);
    }else{
      head_value = DATA::RefValue(false, commit_oid);
    }

    DATA::update_ref(head_path, head_value, true);
  }
}
