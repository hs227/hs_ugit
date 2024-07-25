#include"data.h"
#include<iostream>
#include<filesystem>
#include<fstream>

#include"sha.h"
#include"base.h"

namespace DATA{

  std::map<std::string, obj_type> type_map = {{"blob", obj_type::blob}};

  void init()
  {
    // .ugit
    if (!std::filesystem::exists(DATA::LAB_GIT_DIR)){
      // .ugit need to be created
      bool flag = std::filesystem::create_directories(DATA::LAB_GIT_DIR);
      if (flag){
        printf("Initialized empty ugit repository in \"%s\"\n", std::filesystem::absolute(DATA::LAB_GIT_DIR).string().c_str());
      }
    }else{
      std::cout << std::filesystem::absolute(DATA::LAB_GIT_DIR) << " already exists\n";
    }

    // objects
    std::string obj_dir = OBJECTS_DIR;
    if (!std::filesystem::exists(obj_dir))
    {
      bool flag = std::filesystem::create_directory(obj_dir);
      if (!flag)
      {
        std::cout << ".ugit/objects create failed" << std::endl;
      }
    }
  }

  std::string hash_object(const std::string &file,const std::string type)
  {
    std::filesystem::path filename = CUR_DIR+"/"+file;

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
    std::string buf; //object 
    buf = type + '\x00' + content;


    // SHA1作为object的文件名
    std::string oid = SHA::hash_str(buf);
    if (oid.empty())
    {
      std::cout << "hash_object:hash { \"" << filename << "\" failed" << std::endl;
      return "";
    }
    std::filesystem::path res_path = OBJECTS_DIR + "/" + oid;


    // 创建object文件(binary)
    std::ofstream out(res_path,std::ios::binary);
    if(!out.is_open()){
      std::cout<<"hash_object:out stream failed"<<std::endl;
      return "";
    }
    out.write(buf.data(),buf.size());
    out.close();

    return oid;
  }

  std::string cat_file(const std::string &oid, const std::string expected)
  {
    std::filesystem::path filename=OBJECTS_DIR+"/"+oid;

    // 获取文件内容
    if(!std::filesystem::exists(filename)){
      return "";
    }
    std::ifstream in(filename,std::ios::binary);
    if(!in.is_open()){
      return "";
    }
    std::string buf;
    in>>buf;
    in.close();
    
    //解析提取文件
    std::string type;
    std::string content;
    size_t split_pos=buf.find('\x00');
    type=buf.substr(0,split_pos);
    content=buf.substr(split_pos+1);

    if(!expected.empty()&&type!=expected){
      std::cout<<"cat-file: expected type wrong. [e="
      << expected<<",t="<<type<<"]"<<std::endl;
    }

    return content;
  }

}
