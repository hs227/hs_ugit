#include"data.h"
#include<iostream>
#include<filesystem>
#include<fstream>

#include"sha.h"
#include"base.h"
#include"data_other.h"



namespace DATA
{

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

  void set_HEAD(const std::string &oid){
    const std::string& path = HEAD_PATH;
    std::ofstream out(path,std::ios::binary);
    if(!out.is_open()){
      std::cout<<"set_HEAD: HEAD open failed\n";
      return;
    }

    out.write(oid.data(),oid.size());
    out.close();
    return;
  }

  std::string get_HEAD()
  {
    const std::string& path=HEAD_PATH;

    if(!std::filesystem::exists(path)){
      return "";
    }
    // HEAD 存在
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
    {
      std::cout << "get_HEAD failed\n";
      return "";
    }

    std::string data;
    in >> data;
    in.close();
    return data;
  }

  std::string hash_object(const std::string &data_, const std::string type)
  {
    std::string data;
    if(type=="blob"){
      data=hash_blob_obj(data_);
    }else if(type=="tree"){
      data=hash_tree_obj(data_);
    }else if(type=="commit"){
      data=hash_commit_obj(data_);
    }

    // SHA1作为object的文件名
    std::string oid = SHA::hash_str(data);
    if (oid.empty())
    {
      std::cout << "hash_object:hash failed ["<<type<<"]" << std::endl;
      return "";
    }
    std::filesystem::path res_path = OBJECTS_DIR + "/" + oid;


    // 创建object文件(binary)
    std::ofstream out(res_path,std::ios::binary);
    if(!out.is_open()){
      std::cout<<"hash_object:out stream failed"<<std::endl;
      return "";
    }
    out.write(data.data(),data.size());
    out.close();

    return oid;
  }

  std::string get_object(const std::string &oid, const std::string expected)
  {
    std::filesystem::path filename=OBJECTS_DIR+"/"+oid;

    // 获取文件内容
    if(!std::filesystem::exists(filename)){
      return "";
    }
    std::ifstream in(filename,std::ios::binary|std::ios::ate);
    if(!in.is_open()){
      return "";
    }
    size_t len=in.tellg();
    in.seekg(0);

    std::string buf;
    buf.resize(len);
    in.read(buf.data(),len);
    in.close();
    
    //解析提取文件
    std::string type;
    std::string content;
    size_t split_pos=buf.find(' ');
    type=buf.substr(0,split_pos);
    content=buf.substr(split_pos+1);

    if(!expected.empty()&&type!=expected){
      std::cout<<"cat-file: expected type wrong. [e="
      << expected<<",t="<<type<<"]"<<std::endl;
    }

    return content;
  }
}
