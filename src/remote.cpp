#include"remote.h"

#include"data.h"

namespace REMOTE{

  void fetch(const std::string& remote_path)
  {
    printf("Will fetch the following refs:\n");
    DATA::change_git_dir(remote_path);
    std::vector<std::string> ref_names;
    std::vector<DATA::RefValue> ref_values;
    DATA::iter_refs(ref_names,ref_values,"refs/heads");
    for(const auto& ref_name:ref_names){
      std::cout<<"- "<<ref_name<<std::endl;
    }

  }





}

