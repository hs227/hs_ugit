#include"data.h"
#include<iostream>
#include<filesystem>
#include<fstream>

#include"sha.h"

namespace DATA{

void init()
{
  // .ugit
  if (!std::filesystem::exists(DATA::LAB_GIT_DIR))
  {
    // .ugit need to be created
    bool flag = std::filesystem::create_directories(DATA::LAB_GIT_DIR);
    if(flag){


      printf("Initialized empty ugit repository in \"%s\"\n", std::filesystem::absolute(DATA::LAB_GIT_DIR).string().c_str());
    }
  }
  else
  {
    std::cout << std::filesystem::absolute(DATA::LAB_GIT_DIR) << " already exists\n";
    // std::cout << DATA::LAB_GIT_DIR << " already exists\n";
  }

  // objects
  std::string obj_dir = OBJECTS_DIR;
  if(!std::filesystem::exists(obj_dir)){
    bool flag=std::filesystem::create_directory(obj_dir);
    if(!flag){
      std::cout<<".ugit/objects create failed"<<std::endl;
    }
  }

}

std::string hash_object(const std::string &file)
{
  std::filesystem::path filename = CUR_DIR+"/"+file;
  // SHA1作为object的文件名
  std::string oid=SHA::hash_file(filename.string());
  if(oid.empty()){
    std::cout<<"hash_object:hash { \""<<filename<<"\" failed"<<std::endl;
    return "";
  }

  // 创建object文件(binary)
  std::filesystem::path res_path=OBJECTS_DIR+"/"+oid;
  // 二进制打开|指针指向文件末尾
  std::ifstream in(filename,std::ios::binary|std::ios::ate);
  if(!in.is_open()){
    std::cout << "hash_object:in stream failed" << std::endl;
    return "";
  }
  std::ofstream out(res_path,std::ios::binary);
  if(!out.is_open()){
    std::cout<<"hash_object:out stream failed"<<std::endl;
    return "";
  }
  
  size_t len = in.tellg();    // 文件总大小
  in.seekg(0, std::ios::beg); // 指针指向文件开头

  std::vector<char> buf(len);
  in.read(buf.data(),len);
  out.write(buf.data(),len);

  
  in.close();
  out.close();

  
  return oid;
}

std::string cat_file(const std::string & oid)
{
  std::filesystem::path filename=OBJECTS_DIR+"/"+oid;

  if(!std::filesystem::exists(filename)){
    return "";
  }
  std::ifstream in(filename,std::ios::binary);
  if(!in.is_open()){
    return "";
  }

  std::string res;
  in>>res;

  in.close();

  return res;
}

}
