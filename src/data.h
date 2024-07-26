#ifndef DATA_H
#define DATA_H

#include<iostream>
#include<map>

#define BOOL_STR(x) \
  ((x) ? "True" : "False")

namespace DATA{
  const std::string CUR_DIR="../lab_space";// for this project 
  const std::string GIT_DIR = ".ugit";
  const std::string LAB_GIT_DIR=CUR_DIR+"/"+GIT_DIR;
  const std::string OBJECTS_DIR=LAB_GIT_DIR+"/objects";

  enum class obj_type: int {blob=0};

  extern std::map<std::string,obj_type> type_map;



  void init();
  std::string hash_object(const std::string&,const std::string="blob");
  std::string get_object(const std::string&,const std::string="blob");
}

#endif
