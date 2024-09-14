#ifndef BASE_OTHER_H
#define BASE_OTHER_H

#include <iostream>
#include <filesystem>
#include <set>
#include<functional>

#include "data.h"

inline static bool is_ignore(const std::string& path){
  // filter the .ugit
  if (path.find(".ugit")!=std::string::npos)
    return true;
  // TODO support the .ugitignore
  return false;
}

// ignore the specific files when iterate the workshop
inline static bool is_ignore(void *entry_)
{
  std::filesystem::directory_entry *entry = (std::filesystem::directory_entry *)entry_;
  return is_ignore(entry->path().string());
}



// iterate the workshop`s files to write a 'tree' object
// in: dir_path
// out: 'tree'_object`s oid
// side: create a tree_object and (blobs and trees)
static inline std::string write_tree_compact(const std::string &path)
{
  std::set<BASE::wt_iter_node> entries;


  for(const auto& entry:std::filesystem::directory_iterator(path)){
    std::string oid;
    std::string type;
    if(is_ignore((void*)&entry))
      continue;

    if (entry.is_regular_file()){
      // 普通文件
      type = "blob";
      oid = DATA::hash_object(entry.path().string(), "blob");
    }else{
      // 目录
      type = "tree";
      oid =  write_tree_compact(entry.path().string());
    }
    entries.insert({
        type,oid,entry.path().filename().string(),});
  }

  std::string tree_data;
  for (const auto &entry : entries)
  {
    tree_data += entry.type + " " + entry.oid + " " + entry.name + "\n";
  }

  std::string res_oid = DATA::hash_object(tree_data, "tree");
  return res_oid;
}

static inline std::string write_tree_index_recur(std::set<DATA::index_entry>& index_entries,const std::string& path)
{
  std::set<BASE::wt_iter_node> wt_entries;

  for(const auto& index_entry:index_entries){
    BASE::wt_iter_node wt_entry;
    wt_entry.name=index_entry.path;
    wt_entry.type=index_entry.type;
    if(index_entry.type=="blob"){
      // file
      wt_entry.oid=index_entry.SHA1;
    }else{
      // dir
      wt_entry.oid=write_tree_index_recur(((DATA::index_entry&)index_entry).entries,path+index_entry.path+"/");
      ((DATA::index_entry&)index_entry).SHA1=wt_entry.oid;
    }
    wt_entries.insert(wt_entry);
  }

  std::string tree_data;
  for (const auto &entry : wt_entries)
  {
    tree_data += entry.type + " " + entry.oid + " " + entry.name + "\n";
  }

  std::string tree_oid = DATA::hash_object(tree_data, "tree");
  return tree_oid;
}


static inline std::string write_tree_index()
{
  DATA::index_context index_ctx =DATA::get_index();
  std::string tree_oid=write_tree_index_recur(index_ctx.entries,DATA::CUR_DIR+"/");
  // update trees` oid
  DATA::put_index(index_ctx);
  return tree_oid;
}



static inline void add_dir_to_index_recur_add(std::set<DATA::index_entry>& index_entries,const std::string& dirname)
{
  const std::string dirpath=DATA::CUR_DIR+"/"+dirname;
  for(const auto& entry:std::filesystem::directory_iterator(dirpath)){
    DATA::index_entry index_entry;

    if(is_ignore(entry.path().string()))
      continue;

    if(entry.is_regular_file()){
      index_entry.type="blob";
      index_entry.path=entry.path().filename().string();
      index_entry.SHA1=DATA::hash_object(entry.path().string(),"blob");
    }else{
      index_entry.type="tree";
      index_entry.path=entry.path().filename().string();
      index_entry.SHA1="U";
      add_dir_to_index_recur_add(index_entry.entries,entry.path().string());
    }
    index_entries.insert(index_entry);
  }
  return;
}


static inline void add_dir_to_index_climb_tree(std::set<DATA::index_entry>& index_entries,const std::string& filepath,const std::string& whole_path)
{
  size_t pos=filepath.find('/');
  if(pos!=std::string::npos){
    // indirect dir
    std::string dirname=filepath.substr(0,pos);
    std::string next_filepath=filepath.substr(pos+1);
    DATA::index_entry next_entry{"tree",dirname,"U",{}};
    add_dir_to_index_climb_tree(next_entry.entries,next_filepath,whole_path);
    index_entries.insert(next_entry);
  }else{
    // direct dir
    std::string dirname=filepath;
    if(dirname=="."){
      add_dir_to_index_recur_add(index_entries,whole_path);      
    }else{
      DATA::index_entry next_entry{"tree",dirname,"U",{}};
      add_dir_to_index_recur_add(next_entry.entries,whole_path);
      index_entries.insert(next_entry);
    }
  }
}


static inline void add_dir_to_index(std::set<DATA::index_entry>& index_entries,const std::string& dirpath)
{
  // climb the dir tree
  // then add dirs and files
  add_dir_to_index_climb_tree(index_entries,dirpath,dirpath);  
  
}

static inline std::string read_file(const std::string& filepath)
{
  std::string content;
  if(!std::filesystem::exists(filepath)||!std::filesystem::is_regular_file(filepath))
    return "";

  std::ifstream ifs(filepath,std::ios::ate|std::ios::binary);
  content.resize(ifs.tellg());
  ifs.seekg(0);
  ifs.read(content.data(),content.size());
  ifs.close();

  return content;
}

// add a file to index_context
// in: index_ctx,filepath
static inline void add_file_to_index(std::set<DATA::index_entry>& index_entries,const std::string& filepath,const std::string& whole_path)
{
  size_t pos=filepath.find('/');
  if(pos!=std::string::npos){
    // dir
    std::string dirname=filepath.substr(0,pos);
    std::string next_filepath=filepath.substr(pos+1);
    DATA::index_entry next_entry{"tree",dirname,"U",{}};
    add_file_to_index(next_entry.entries,
      next_filepath,whole_path);
    index_entries.insert(next_entry);
  }else{
    // file
    std::string filename=filepath;
    std::string content=read_file(whole_path);
    std::string oid=DATA::hash_object(content,"blob");
    DATA::index_entry new_entry{"blob",filename,oid,{}};
    index_entries.insert(new_entry);
  }

  
}
// just build the index not list like tree
static inline void get_tree_index(std::set<DATA::index_entry>& index_entries,const std::string& tree_oid)
{
  std::set<BASE::wt_iter_node> wt_entries;
  iter_tree_entries(wt_entries,tree_oid);
  for(const auto& wt_entry:wt_entries){
    DATA::index_entry index_entry;
    
    index_entry.type=wt_entry.type;
    index_entry.SHA1=wt_entry.oid;
    index_entry.path=wt_entry.name;
    if(wt_entry.type=="tree"){
      // tree
      get_tree_index(index_entry.entries,index_entry.SHA1);
    }
    index_entries.insert(index_entry);
  }
}


static inline void read_tree_index_add_file(std::set<DATA::index_entry>& index_entries,const std::string& filepath,const std::string& whole_path,const std::string& content)
{
    size_t pos=filepath.find('/');
  if(pos!=std::string::npos){
    // dir
    std::string dirname=filepath.substr(0,pos);
    std::string next_filepath=filepath.substr(pos+1);
    DATA::index_entry next_entry{"tree",dirname,"U",{}};
    add_file_to_index(next_entry.entries,
      next_filepath,whole_path);
    index_entries.insert(next_entry);
  }else{
    // file
    std::string filename=filepath;
    std::string oid=DATA::hash_object(content,"blob");
    DATA::index_entry new_entry{"blob",filename,oid,{}};
    index_entries.insert(new_entry);
  }

}
// for checkout_index
// create files and dirs in workshop
static inline void checkout_index_recur(const std::set<DATA::index_entry>& index_entries,const std::string& dirpath)
{
  for(const auto& index_entry:index_entries){
    if(index_entry.type=="blob"){
      // blob
      std::string path=dirpath+index_entry.path;
      std::string object_path=DATA::OBJECTS_DIR+"/"+index_entry.SHA1;
      std::string content;
      std::ifstream input(object_path,std::ios::binary|std::ios::ate);
      content.resize(input.tellg());
      input.seekg(0);
      input.read(content.data(),content.size());
      input.close();
      std::ofstream output(path,std::ios::binary|std::ios::trunc);
      output.write(content.data(),content.size());
      output.close();

    }else{
      // tree
      std::string path=dirpath+index_entry.path+"/";
      std::filesystem::create_directory(path);
      checkout_index_recur(index_entry.entries,path);
    }

  }
}






#endif
