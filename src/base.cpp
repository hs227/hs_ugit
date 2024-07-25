#include"base.h"
#include<filesystem>


#include"data.h"
#include"base_other.h"


namespace BASE
{

  std::string write_tree(std::string directory)
  {
    // 遍历目录

    // structure version
    // std::string oid;
    // iter_files(directory,
    //            file_nothing, file_nothing, 
    //            is_ignore,
    //            file_hash, file_entries_e,
    //            (void*)&oid);
    // return oid;

    // compact version
    std::string oid=write_tree_compact(directory);
    return oid;
  }
}
