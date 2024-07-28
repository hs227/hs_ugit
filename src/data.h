#ifndef DATA_H
#define DATA_H

#include<iostream>
#include<map>
#include<vector>

#define BOOL_STR(x) \
  ((x) ? "True" : "False")

namespace DATA{
  const std::string CUR_DIR="../lab_space";// for this project 
  const std::string GIT_DIR = ".ugit";
  const std::string LAB_GIT_DIR=CUR_DIR+"/"+GIT_DIR;
  //const std::string REFERENCE_DIR=LAB_GIT_DIR+"/"+"refs"+"/"+"tags";
  const std::string OBJECTS_DIR=LAB_GIT_DIR+"/objects";
  const std::string HEAD_PATH=LAB_GIT_DIR+"/"+"HEAD";

  struct RefValue
  {
    bool is_symbolic;
    std::string value;// true:ref;false:oid
    RefValue()
      :value(""),is_symbolic(false)
    {}
    RefValue(bool is_symbolic_,const std::string& value_)
      :is_symbolic(is_symbolic_),value(value_)
    {}
  };



  void init();
  void update_ref(const std::string &, const RefValue &);
  RefValue get_ref(const std::string &);
  std::string hash_object(const std::string &, const std::string = "blob");
  std::string get_object(const std::string&,const std::string="blob");
  void iter_refs(std::vector<std::string>& ref_name,std::vector<std::string>& ref_oid);
}

#endif
