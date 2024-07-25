#include"base.h"
#include<filesystem>
#include<functional>

#include"data.h"

// dir iter funcs
inline static void file_print(void* entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry*)entry_;
  if (entry->is_regular_file())
  {
    // 普通文件
    std::cout << "File: " << entry->path() << std::endl;
  }
  else
  {
    // 目录
    std::cout << "Directory: " << entry->path() << std::endl;
  }
}
// dir iter
inline static void iter_files(const std::string &path, void func(void *) = file_print)
{
  for (const auto &entry : std::filesystem::directory_iterator(path)){
    func((void*)&entry);
    if(entry.is_directory()){
      // 递归遍历
      iter_files(entry.path().string(),func);
    }
  }
}

namespace BASE
{

  void write_tree(std::string directory)
  {
    // 遍历目录
    iter_files(directory);

    // TODO actually create the tree object
  }
}
