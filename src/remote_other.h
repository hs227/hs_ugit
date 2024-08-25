#ifndef REMOTE_OTHER_H
#define REMOTE_OTHER_H


#include<iostream>
#include<vector>
#include<set>
#include<algorithm>
#include"remote.h"
#include"data.h"
#include"base.h"

inline static std::set<std::string> push_objects_filter(
  const std::vector<DATA::RefValue>&remote_ref_values,
  const std::string& local_ref)
{ 
  // only the refs that we are working in local needed to be pushed 
  std::vector<std::string> needed_remote_ref;
  for(const auto& ref_value:remote_ref_values){
    if(DATA::object_exists(ref_value.value)){
      needed_remote_ref.push_back(ref_value.value);
    }
  }
  // find the missing objects needed
  std::set<std::string> remote_objects;
  std::set<std::string> local_objects;
  auto tmp1=BASE::iter_objects_in_commits(needed_remote_ref);
  auto tmp2=BASE::iter_objects_in_commits({local_ref});
  remote_objects.insert(tmp1.begin(),tmp1.end());
  local_objects.insert(tmp2.begin(),tmp2.end());
  std::set<std::string> objects_to_push;
  std::set_difference(remote_objects.begin(),remote_objects.end(),
    local_objects.begin(),local_objects.end(),
    objects_to_push,objects_to_push.begin());  
    
  return objects_to_push;
}



#endif
