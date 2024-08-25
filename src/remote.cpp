#include"remote.h"

#include"data.h"

namespace REMOTE{

  void get_remote_refs(std::vector<std::string> &ref_name, std::vector<DATA::RefValue> &ref_value,const std::string& remote_path,const std::string& prefix)
  {
    DATA::change_git_dir(remote_path);
    DATA::iter_refs(ref_name,ref_value,prefix);
  }

  void fetch(const std::string& remote_path)
  {
    printf("Will fetch the following refs:\n");
    std::vector<std::string> ref_names;
    std::vector<DATA::RefValue> ref_values;

    get_remote_refs(ref_names,ref_values,remote_path,"refs/heads");
    for(const auto& ref_name:ref_names){
      std::cout<<"- "<<ref_name<<std::endl;
    }

  }

  




}

