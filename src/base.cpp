#include"base.h"
#include<filesystem>
#include<functional>

#include"data.h"

// dir iter funcs
inline static void file_print(void* entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry*)entry_;
  if (entry->is_regular_file()){
    // 普通文件
    std::cout << "File: " << entry->path() << std::endl;
  }else{
    // 目录
    std::cout << "Directory: " << entry->path() << std::endl;
  }
}
inline static void file_hash(void* entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry *)entry_;
  if (entry->is_regular_file()){
    // 普通文件
    DATA::hash_object(entry->path().string());
  }else{
    // 目录

  }
}

inline static bool is_ignore(void*entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry *)entry_;
  // filter the .ugit
  if (entry->path().filename() == ".ugit")
    return true;
  // TODO support the .ugitignore
  return false;
}
// dir iter
inline static void iter_files(const std::string &path, void func(void *) = file_print,bool filter(void*)=is_ignore)
{
  for (const auto &entry : std::filesystem::directory_iterator(path)){
    if(filter((void*)&entry))
      continue;
    func((void*)&entry);
    if(entry.is_directory()){
      // 递归遍历
      iter_files(entry.path().string(),func,filter);
    }
  }
}

namespace BASE
{

  void write_tree(std::string directory)
  {
    // 遍历目录
    iter_files(directory,file_hash);

    // TODO actually create the tree object
  }
}
