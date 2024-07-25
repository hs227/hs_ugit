#ifndef DATA_OTHER_HPP
#define DATA_OTHER_HPP

#include<iostream>
#include<filesystem>
#include<fstream>

#include"data.h"

// for the prepare hash obj"blob"
inline static std::string hash_blob_obj(const std::string &file)
{
  std::filesystem::path filename = DATA::CUR_DIR + "/" + file;

  // 获得文件内容
  // 二进制打开|指针指向文件末尾
  std::ifstream in(filename, std::ios::binary | std::ios::ate);
  if (!in.is_open())
  {
    std::cout << "hash_object:in stream failed" << std::endl;
    return "";
  }
  size_t len = in.tellg();    // 文件总大小
  in.seekg(0, std::ios::beg); // 指针指向文件开头
  std::string content;
  content.resize(len);
  in.read(content.data(), len);
  in.close();
  std::string buf; // object
  buf = "blob" + '\x00' + content;

  return buf;
}
// for the prepare hash obj"tree"
inline static std::string hash_tree_obj(const std::string &content)
{
  std::string buf; // object
  buf = "tree" + '\x00'  + content;

  return buf;
}

#endif
