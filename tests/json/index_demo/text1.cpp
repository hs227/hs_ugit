/*
  吓到我了，这易用性。
*/



#include"../../../src/include/nlohmann/json.hpp"
#include<fstream>
#include<iostream>
#include<set>
#include<iomanip>
using namespace std;
using json=nlohmann::json;

#define PRINT(X) \
  std::cout<< #X ":"<<X<<std::endl

struct index_metadata{
  std::string startTime;
};
void to_json(json& j,const index_metadata& m){
  j=json{{"startTime",m.startTime}};
}
void from_json(const json& j,index_metadata& m){
  j.at("startTime").get_to(m.startTime);
}

struct index_entry{
  std::string type;
  std::string path;
  std::string SHA1;
  std::set<index_entry> entries;

  bool operator<(const index_entry& rhs) const{
    return this->path<rhs.path;
  }
};
void to_json(json& j,const index_entry& e){
  j=json{{"type",e.type},{"path",e.path},{"SHA1",e.SHA1},{"entries",e.entries}};
}
void from_json(const json& j,index_entry& e){
  j.at("type").get_to(e.type);
  j.at("path").get_to(e.path);
  j.at("SHA1").get_to(e.SHA1);
  j.at("entries").get_to(e.entries);
}

struct index_context{
  index_metadata metadata;
  std::set<index_entry> entries;
};
void to_json(json& j,const index_context& i){
  j=json{{"metadata",i.metadata},{"entries",i.entries}};
}
void from_json(const json& j,index_context& i){
  j.at("metadata").get_to(i.metadata);
  j.at("entries").get_to(i.entries);
}


// void print_index_entries(const std::set<index_entry>& entries)
// {
//   for(const index_entry& entry: entries){
//     if(entry.type=="blob"){
//       std::cout<<entry.SHA1<<" "<<entry.path<<std::endl;
//     }else if(entry.type=="tree"){
//       print_index_entries(entry.entries);
//     }
//   }
// }




int main()
{
  // 1. Input
  json j;
  std::ifstream ifs("index_o_0.json");
  ifs>>j;
  ifs.close();

  // 2. Get and Print
  index_context index_ctx;
  index_ctx=j.get<index_context>();
  //print_index_entries(index_ctx.entries);
  

  // 3. Output new json
  json j_out;
  j_out["metadata"]["startTime"]="time1";
  //j_out["entries"]=index_ctx.entries;
  j_out["entries"]={};

  std::ofstream out("index_o_0.json");
  out<<std::setw(2)<<j_out<<std::endl;
  out.close();
  

  return 0;
}

