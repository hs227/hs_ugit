#include"data.h"
#include<iostream>
#include<filesystem>

namespace DATA{

void init()
{
  if (!std::filesystem::exists(DATA::LAB_GIT_DIR))
  {
    // .ugit need to be created
    bool flag = std::filesystem::create_directories(DATA::LAB_GIT_DIR);
    //std::cout << BOOL_STR(flag) << std::endl;
    if(flag){
      printf("Initialized empty ugit repository in \"%s\"\n", std::filesystem::absolute(DATA::LAB_GIT_DIR).string().c_str());
    }
  }
  else
  {
    std::cout << std::filesystem::absolute(DATA::LAB_GIT_DIR) << " already exists\n";
    // std::cout << DATA::LAB_GIT_DIR << " already exists\n";
  }
}




}
