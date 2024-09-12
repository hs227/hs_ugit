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
    std::vector<std::string> parents;
    std::string msg;
    commit_ctx()
        : tree(""), msg("")
    {}
  };

  // write_tree_iter_node
  // tree`s entry
  struct wt_iter_node
  {
    std::string type;
    std::string oid;
    std::string name;

    bool operator<(const wt_iter_node &rhs) const {
      return name.compare(rhs.name) <= 0;
    }
  };

  // get_tree_iter_node
  // the link between object and file
  struct gt_iter_node
  {
    std::string src; // oid in objects
    std::string dst; // path in workshop

    bool operator<(const gt_iter_node &rhs) const{
      return dst.compare(rhs.dst) <= 0;
    }
  };

  std::string write_tree(const std::string& directory="");
  void iter_tree_entries(std::set<wt_iter_node> &entries, const std::string &tree_oid);
  void get_tree(std::set<gt_iter_node> &results, const std::string &tree_oid, const std::string &base_path = "");
  void read_tree(const std::string& tree_oid,const bool is_update_workshop);
  std::string commit(const std::string&);
  commit_ctx get_commit(const std::string&);
  void checkout(const std::string&);
  void create_tag(const std::string&,const std::string&);
  std::string get_ref_path(const std::string &name);
  std::string get_oid(const std::string &);
  std::vector<std::string> iter_commits_and_parents(const std::vector<std::string> &);
  void create_branch(const std::string& name,const std::string& oid);
  bool is_branch(const std::string&);
  void init();
  std::vector<std::string> iter_branch_names();
  void reset(const std::string&);
  std::string get_working_tree();
  void merge(const std::string& other_oid);
  std::string get_merge_base(const std::string& c_oid1,const std::string& c_oid2);
  std::vector<std::string> iter_objects_in_commits(const std::vector<std::string>&);
  bool is_ancestor_of(const std::string& commit_oid,const std::string& maybe_ancestor);
  void add(const std::vector<std::string>& filenames);
}

#endif
