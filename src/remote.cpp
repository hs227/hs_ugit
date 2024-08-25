#include"remote.h"

#include<filesystem>
#include"data.h"
#include"data.h"

namespace REMOTE{
  // using by remote to find remote_branches 
  const std::string REMOTE_REFS_BASE="refs/heads";
  // using by local to find local_remote
  const std::string LOCAL_REFS_BASE="refs/remote";

  void get_remote_refs(std::vector<std::string> &ref_name, std::vector<DATA::RefValue> &ref_value,const std::string& remote_path,const std::string& prefix)
  {
    DATA::change_git_dir(remote_path);
    DATA::iter_refs(ref_name,ref_value,prefix);
  }

  void fetch(const std::string& remote_path)
  {
    // Get refs from server
    std::vector<std::string> ref_names;
    std::vector<DATA::RefValue> ref_values;
    get_remote_refs(ref_names,ref_values,remote_path,REMOTE_REFS_BASE);
    
    const std::string local_ref_path=DATA::DEFAULT_CUR_DIR+"/"+DATA::GIT_DIR+"/";
    // Update local refs to match server
    for(size_t i=0;i<ref_names.size();++i){
      std::cout<<"- "<<ref_names[i]<<std::endl;
      std::string ref_path=std::filesystem::path(ref_names[i]).filename().string();
      ref_path=DATA::DEFAULT_CUR_DIR+"/"+DATA::GIT_DIR+"/"+LOCAL_REFS_BASE+"/"+ref_path;
      std::cout<<"+ "<<ref_path<<std::endl;
      DATA::update_ref(ref_path,ref_values[i]);
    }

  }

  




}

