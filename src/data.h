#ifndef DATA_H
#define DATA_H

#include<iostream>
#include<map>
#include<vector>
#include<set>
#include"include/nlohmann/json.hpp"

#define BOOL_STR(x) \
  ((x) ? "True" : "False")

namespace DATA{
  // PATH
  extern const std::string DEFAULT_CUR_DIR;
  extern std::string CUR_DIR;// for this project 
  extern std::string GIT_DIR;
  extern std::string LAB_GIT_DIR;
  extern std::string OBJECTS_DIR;
  extern std::string HEAD_PATH;
  extern std::string INDEX_PATH;
  extern std::string MHEAD_PATH;

  // RERVALUE
  struct RefValue
  {
    bool is_symbolic;
    std::string value;// true:ref;false:oid
    RefValue()
      : is_symbolic(false), value("")
    {}
    RefValue(bool is_symbolic_,const std::string& value_)
      : is_symbolic(is_symbolic_), value(value_)
    {}
  };

  // JSON
  extern size_t JSON_WIDTH;
  struct index_metadata{
    std::string startTime;
  };
  struct index_entry{
    std::string type;
    std::string path;
    std::string SHA1;
    std::set<index_entry> entries;

    bool operator<(const index_entry& rhs) const{
      return this->path<rhs.path;
    }
  };
  struct index_context{
    index_metadata metadata;
    std::set<index_entry> entries;
  };


  void init();
  void update_ref(const std::string &ref, const RefValue &value,bool deref=true);
  RefValue get_ref(const std::string &ref,bool deref=true);
  std::string hash_object(const std::string &, const std::string = "blob");
  std::string get_object(const std::string&,const std::string="blob");
  void iter_refs(std::vector<std::string> &ref_name, std::vector<RefValue> &ref_value, const std::string prefix="", bool deref = true);
  void remove_ref(const std::string& ref,bool deref=true);
  void change_git_dir(const std::string& new_path="");
  bool object_exists(const std::string& oid);
  bool fetch_object_if_missing(const std::string& oid,std::string remote_git_dir);
  void push_object(const std::string& oid,std::string remote_git_dir);
  void init_index();
  index_context get_index();
  void put_index(const index_context& index_ctx);
}

#endif
