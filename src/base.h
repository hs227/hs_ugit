#ifndef BASE_H
#define BASE_H

#include <iostream>
#include <set>
#include <vector>



namespace BASE
{
  struct commit_ctx
  {
    std::string tree;
    std::string parent;
    std::string msg;
    commit_ctx()
        : tree(""), parent(""), msg("")
    {}
  };

  std::string write_tree(std::string =".");
  void read_tree(const std::string& );
  std::string commit(const std::string&);
  commit_ctx get_commit(const std::string&);
  void checkout(const std::string&);
  void create_tag(const std::string&,const std::string&);
  std::string get_ref_path(const std::string &name);
  std::string get_oid(const std::string &);
  std::vector<std::string> iter_commits_and_parents(const std::vector<std::string> &);
  void create_branch(const std::string& name,const std::string& oid);
}

#endif
