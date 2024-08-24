#ifndef DATA_H
#define DATA_H

#include<iostream>
#include<map>
#include<vector>

#define BOOL_STR(x) \
  ((x) ? "True" : "False")

namespace DATA{
  std::string CUR_DIR="../lab_space";// for this project 
  std::string GIT_DIR = ".ugit";
  std::string LAB_GIT_DIR=CUR_DIR+"/"+GIT_DIR;
  std::string OBJECTS_DIR=LAB_GIT_DIR+"/objects";
  std::string HEAD_PATH=LAB_GIT_DIR+"/HEAD";
  std::string INDEX_PATH=LAB_GIT_DIR+"/index";
  std::string MHEAD_PATH=LAB_GIT_DIR+"/MHEAD";


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



  void init();
  void update_ref(const std::string &ref, const RefValue &value,bool deref=true);
  RefValue get_ref(const std::string &ref,bool deref=true);
  std::string hash_object(const std::string &, const std::string = "blob");
  std::string get_object(const std::string&,const std::string="blob");
  void iter_refs(std::vector<std::string> &ref_name, std::vector<RefValue> &ref_value, const std::string prefix="", bool deref = true);
  void remove_ref(const std::string& ref,bool deref=true);
  void change_git_dir(const std::string& new_path="");
}

#endif
