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

  // create the reference
  // in:ref_name,oid
  // side:set the ref(HEAD or tags) 
  void update_ref(const std::string& ref,const std::string& oid)
  {
    std::string path=get_ref(ref);
    if(path==""){
      std::cout<<"update_ref path failed. \n";
      return;
    }
    
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    
    std::ofstream out(path,std::ios::binary);
    if(!out.is_open()){
      std::cout<<"update_ref: outfile open failed\n";
      return;
    }

    out.write(oid.data(),oid.size());
    out.close();
    return;
  }
  // ref->oid
  // in: ref
  // out:oid
  std::string get_ref(const std::string& ref)
  {
    std::string path = ref;

    if(!std::filesystem::exists(path)){
      return "";
    }
    // ref 存在
    std::ifstream in(path,std::ios::binary);
    if(!in.is_open()){
      std::cout<<"get_ref failed\n";
      return "";
    }
    std::string data;
    in>>data;
    in.close();
    return data;
  }


  // in:content,type
  // out:oid
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
  // in: oid,type
  // out: object_content
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
    size_t split_pos=buf.find('\n');
    type=buf.substr(0,split_pos);
    content=buf.substr(split_pos+1);

    if(!expected.empty()&&type!=expected){
      std::cout<<"cat-file: expected type wrong. [e="
      << expected<<",t="<<type<<"]"<<std::endl;
    }

    return content;
  }

  void iter_refs(std::vector<std::string> &ref_name, std::vector<std::string> &ref_oid)
  {
    std::vector<std::string> refs;
    refs.push_back(LAB_GIT_DIR + "/" + "HEAD");

    const std::string refs_dir=LAB_GIT_DIR+"/"+"refs/";
    iter_refs_recur(refs_dir,refs);

    for(const auto& refname:refs){
      ref_name.push_back(refname);// name
      ref_oid.push_back(get_ref(refname));// oid
    }

  }
}
