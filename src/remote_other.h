#ifndef REMOTE_OTHER_H
#define REMOTE_OTHER_H


#include<iostream>
#include<vector>
#include<set>
#include<algorithm>
#include"remote.h"
#include"data.h"
#include"base.h"

#include"base.h"
#include"remote.h"


inline static void get_remote_refs(std::vector<std::string> &ref_name, std::vector<DATA::RefValue> &ref_value,const std::string& remote_path,const std::string& prefix)
{
  DATA::change_git_dir(remote_path);
  DATA::iter_refs(ref_name,ref_value,prefix);
  DATA::change_git_dir("");
}


inline static std::set<std::string> objects_set_diff(
  const std::set<std::string>& remote_objects,
  const std::set<std::string>& local_objects)
{
  std::set<std::string> res=remote_objects;
  for(const auto& local: local_objects){
    std::set<std::string>::iterator finder;
    if((finder=res.find(local))!=res.end()){
      res.erase(finder);
    }
  }
  return res;
}

inline static std::set<std::string> push_objects_filter(
  const std::string& remote_ref,
  const std::string& local_ref)
{ 
  // only the refs that we are working in local needed to be pushed 
  std::vector<std::string> needed_remote_ref;
  if(DATA::object_exists(remote_ref)){
    needed_remote_ref.push_back(remote_ref);
  }else{
    return {};
  }
  // find the missing objects needed
  std::set<std::string> remote_objects;
  std::set<std::string> local_objects;
  auto tmp1=BASE::iter_objects_in_commits(needed_remote_ref);
  auto tmp2=BASE::iter_objects_in_commits({local_ref});
  remote_objects.insert(tmp1.begin(),tmp1.end());
  local_objects.insert(tmp2.begin(),tmp2.end());
  std::set<std::string> objects_to_push=objects_set_diff(remote_objects,local_objects);
    
  return objects_to_push;
}

inline static std::string get_remote_ref(const std::string& remote_path,const std::string local_ref_name)
{
  std::filesystem::path local_ref_name_tmp=local_ref_name.substr(local_ref_name.find("refs/")+5);
  std::vector<std::string> remote_ref_names;
  std::vector<DATA::RefValue> remote_ref_values;
  get_remote_refs(remote_ref_names,remote_ref_values,remote_path,REMOTE::REMOTE_REFS_BASE);
  for(size_t i=0;i<remote_ref_names.size();++i){
    std::filesystem::path remote_ref_name_tmp=remote_ref_names[i].substr(remote_ref_names[i].find("refs/")+5);

    if(local_ref_name_tmp.filename()==remote_ref_name_tmp.filename()&&
       local_ref_name_tmp.parent_path()==remote_ref_name_tmp.parent_path()){
      return remote_ref_values[i].value;
    }
  }
  return "";
}

#endif
