#include"data.h"
#include<iostream>
#include<filesystem>
#include<fstream>

#include"sha.h"
#include"base.h"
#include"data_other.h"



namespace DATA
{
  // create the repository file
  // 1. '.ugit/'
  // 2. '.ugit/objects/'
  // set the .ugit DIR
  void init()
  {
    // .ugit
    if (!std::filesystem::exists(DATA::LAB_GIT_DIR)){
      // .ugit need to be created
      bool flag = std::filesystem::create_directories(DATA::LAB_GIT_DIR);
      if (flag)
        printf("Initialized empty ugit repository in \"%s\"\n", std::filesystem::absolute(DATA::LAB_GIT_DIR).string().c_str());
    }else{
      std::cout << std::filesystem::absolute(DATA::LAB_GIT_DIR) << " already exists\n";
    }

    // objects
    std::string obj_dir = OBJECTS_DIR;
    if (!std::filesystem::exists(obj_dir))
    {
      bool flag = std::filesystem::create_directory(obj_dir);
      if (!flag)
        std::cout << ".ugit/objects create failed" << std::endl;
    }

    // tmp
    std::string tmp_dir=LAB_GIT_DIR+"/"+"tmp";
    if(!std::filesystem::exists(tmp_dir)){
      bool flag=std::filesystem::create_directory(tmp_dir);
      if(!flag)
        std::cout<<".ugit/tmp create failed"<<std::endl;
    }


    // set GIT_DIR
    change_git_dir();
  }


  // in: ref_path
  // out: RefValue(false,oid)
  gri_res get_ref_internal(const std::string &ref, bool deref = true)
  {
    std::string path = ref;

    std::string content;

    if(std::filesystem::exists(path)){
      // ref 存在
      std::ifstream in(path, std::ios::binary | std::ios::ate);
      if (!in.is_open())
      {
        std::cout << "get_ref failed\n";
        return gri_res();
      }
      size_t content_len=in.tellg();
      in.seekg(0);
      content.resize(content_len);
      in.read(content.data(),content.size());
      in.close();
    }

    bool symbolic=!content.empty()&&
      content.find("ref:")==0;
    if(symbolic){
      content = DATA::LAB_GIT_DIR + "/" + content.substr(5);
      // still need to recur deeper
      if (deref){
        return get_ref_internal(content,true);
      }
    }
    return gri_res(path, RefValue(symbolic, content));
  }

  // create the reference
  // in:ref_name,refValue,deref(if deref)
  // side:set the ref(HEAD or tags)
  void update_ref(const std::string &ref, const RefValue &value,bool deref)
  {
    std::string path=get_ref_internal(ref,deref).path;
    if(path==""){
      std::cout<<"update_ref path failed. \n";
      return;
    }

    std::string out_data;
    if (value.is_symbolic){
      out_data = "ref: " + value.value;
    }else{
      out_data = value.value;
    }

    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream out(path,std::ios::binary);
    if(!out.is_open()){
      std::cout<<"update_ref: outfile open failed\n";
      return;
    }

    out.write(out_data.data(), out_data.size());
    out.close();
    return;
  }

  // ref->oid
  // in: ref
  // out:RefValue::oid
  RefValue get_ref(const std::string &ref, bool deref)
  {
    return get_ref_internal(ref,deref).value;
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
  // get all the refs` name and refs` oid
  // in: prefix(distinguish tag/branch),deref
  // output: ref_names,ref_oids
  void iter_refs(std::vector<std::string> &ref_name, std::vector<RefValue> &ref_value, const std::string prefix, bool deref)
  {
    std::vector<std::string> refs;
    // HEAD
    refs.push_back(HEAD_PATH);
    // MHEAD
    if(std::filesystem::exists(MHEAD_PATH))
      refs.push_back(MHEAD_PATH);
    

    const std::string refs_dir=LAB_GIT_DIR+"/"+"refs/";
    iter_refs_recur(refs_dir,refs);

    for(const auto& refname:refs){
      if(!prefix.empty()&&refname.find(prefix)==std::string::npos){
        // example: 
        // prefix="heads"-> get 'branch'
        // prefix="tags" -> get 'tag'
        continue;
      }
      ref_name.push_back(refname);// name
      ref_value.push_back(get_ref(refname,deref));// value
    }

  }

  void remove_ref(const std::string& ref,bool deref)
  {
    gri_res res=get_ref_internal(ref,deref);
    std::filesystem::path ref_path=res.path;
    // remove it
    if(std::filesystem::exists(ref_path)){
      std::filesystem::remove(ref_path);
    }
  }

  void change_git_dir(const std::string& new_path)
  {
    if(new_path==""){
      //使用默认路径
      CUR_DIR="../lab_space";// for this project 
      GIT_DIR = ".ugit";
      LAB_GIT_DIR=CUR_DIR+"/"+GIT_DIR;
      OBJECTS_DIR=LAB_GIT_DIR+"/objects";
      HEAD_PATH=LAB_GIT_DIR+"/HEAD";
      INDEX_PATH=LAB_GIT_DIR+"/index";
      MHEAD_PATH=LAB_GIT_DIR+"/MHEAD";
    }else{
      CUR_DIR=new_path;
      GIT_DIR = ".ugit";
      LAB_GIT_DIR=CUR_DIR+"/"+GIT_DIR;
      OBJECTS_DIR=LAB_GIT_DIR+"/objects";
      HEAD_PATH=LAB_GIT_DIR+"/HEAD";
      INDEX_PATH=LAB_GIT_DIR+"/index";
      MHEAD_PATH=LAB_GIT_DIR+"/MHEAD";
    }
  }

}
