// https://www.cnblogs.com/linuxAndMcu/p/14503341.html 
// 三、主要用法
#include"../../../src/include/nlohmann/json.hpp"
#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cassert>
#include<deque>
#include<list>
#include<map>
#include<set>
#include<unordered_map>
#include<unordered_set>
#include<forward_list>
#include<array>

using json=nlohmann::json;

#define PRINT(X) \
  std::cout<< #X ":"<<X<<std::endl



/* Aim
{
  "pi": 3.141,
  "happy": true,
  "name": "Niels",
  "nothing": null,
  "answer": {
    	"everything": 42
  },
  "list": [1, 0, 2],
  "object": {
	    "currency": "USD",
 	    "value": 42.99
  }
}
*/
int main1()
{
  // 3.1 构建json对象
  json content;
  content["pi"]=3.141;
  content["happy"]=true;
  content["name"]="Niels";
  content["nothing"]=NULL;
  content["answer"]["everything"]=42;
  content["list"]={1,0,2};
  content["object"]={{"currency","USD"},{"value",42.99}};

  // 3.2 获取并打印JSON元素值
  float pi=content.at("pi");
  std::string name=content.at("name");
  int everything=content.at("answer").at("everything");
  std::cout<<pi<<std::endl;
  std::cout<<name<<std::endl;
  std::cout<<everything<<std::endl;
  // list
  for(size_t i=0;i<3;++i)
    std::cout<<content.at("list").at(i)<<std::endl;
  // object
  std::cout<<content.at("object").at("currency")<<std::endl;
  std::cout<<content.at("object").at("value")<<std::endl;

  // 3.3 写JSON对象到.json文件
  std::ofstream file("pretty.json");
  file<<content<<std::endl;
  file.close();

  return 0;
}

int main2()
{
  // 3.4 string序列化和反序列化
  // 3.4.1 反序列化：从字节序列恢复JSON对象
  json j1="{\"happy\":true,\"pi\":3.141}"_json;
  auto j2=R"({"happy":true,"pi":3.141})"_json;
  // 或者
  std::string j3="{\"happy\":true,\"pi\":3.141}";
  json j4=json::parse(j3.c_str());
  std::cout<<j1.at("pi")<<std::endl;
  std::cout<<j2<<std::endl;
  std::cout<<j3<<std::endl;
  std::cout<<j4.at("pi")<<std::endl;
  // 3.4.2 序列化：从JSON对象转化为字节序列
  std::string j5=j4.dump();// return std::string
  std::cout<<j5<<std::endl;

  // 3.5 stream的序列化和反序列化
  std::cout<<"3.5 begin:"<<std::endl;
  json j6;
  //std::cin >> j6; // 从标准输入中反序列化json对象
  //std::cout << j6; // 将json对象序列化到标准输出中
  std::ifstream in("file.json");
  json j7;
  in>>j7;
  in.close();

  // 以易于查看的方式将json对象写入到本地文件
  std::ofstream out("pretty.json");
  out<<std::setw(2)<<j7<<std::endl;

  return 0;
}

// 3.6 任意类型转换
namespace ns{
  // 首先定义一个结构体
  struct person{
    std::string name;
    std::string address;
    int age;
  };
  void to_json(json& j,const person& p){
    j=json{{"name",p.name},{"address",p.address},{"age",p.age}};
  }
  void from_json(const json& j,person& p){
    j.at("name").get_to(p.name);
    j.at("address").get_to(p.address);
    j.at("age").get_to(p.age);
  }
}
int main3()
{
  // 3.6.1 最万能的方法
  ns::person p1={"Ned Flanders","744 Evergreen Terrace",60};
  // 从结构体转换到json对象
  json j1;
  j1["name"]=p1.name;
  j1["address"]=p1.address;
  j1["age"]=p1.age;
  // 从json对象转换到结构体
  ns::person p2{
    j1["name"].get<std::string>(),
    j1["address"].get<std::string>(),
    j1["age"].get<int>()};

  // 3.6.2 nlohman提供的方法(from-to)
  auto p3=j1.get<ns::person>();

  return 0;
}
/*
注意：
  1.函数to_json()和from_json()和你定义的数据类型在同一个命名空间中；
  2.当你要在另外的文件中使用这两个函数时，要正确的包含它所在的头文件；
  3.在from_json中要使用at()，因为当你读取不存在的名称时，它会抛出错误。
*/

int main4()
{
  // 3.7建议使用显示类型转换
  // String
  std::string s1="Hello,world!";
  json js=s1;
  auto s2=js.get<std::string>();
  // 不建议
  std::string s3=js;
  std::string s4;
  s4=js;

  // Blooleans
  bool b1=true;
  json jb=b1;
  auto b2=jb.get<bool>();
  // 不建议
  bool b3=jb;
  bool b4;
  b4=jb;

  // Numbers
  int i=42;
  json jn=i;
  auto f=jn.get<double>();
  // 不建议
  double f2=jn;
  double f3;
  f3=jn;

  return 0;
}

// 3.8转换JSON到二进制格式
/*
尽管 JSON 格式非常常用，但是它的缺点也很明显，
它并不是一种紧凑的格式，不适合通过网络传输，或者写到本地，
常常需要将 json 对象就行二进制转换，
nlohmann 库支持多种二进制格式，包括 BSON，CBOR，MessagePack 和 UBJSON。
*/
int main5()
{
  // create a JSON value
  json j=R"({"compact":true,"schema":0})"_json;

  // serialize to BSON
  std::vector<std::uint8_t> v_bson=json::to_bson(j);
  // 0x1B, 0x00, 0x00, 0x00, 0x08, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0x00, 0x01, 0x10, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  // roundtrip
  json j_from_bson=json::from_bson(v_bson);

  // serialize to CBOR
  std::vector<std::uint8_t> v_cbor=json::to_cbor(j);
  // 0xA2, 0x67, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0xF5, 0x66, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00
  // roundtrip
  json j_from_cbor=json::from_cbor(v_cbor);

  // serialize to MessagePack
  std::vector<std::uint8_t> v_msgpack=json::to_msgpack(j);
  // 0x82, 0xA7, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0xC3, 0xA6, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00
  // roundtrip
  json j_from_msgpack=json::from_msgpack(v_msgpack);

  // serialize to UBJSON
  std::vector<std::uint8_t> v_ubjson=json::to_ubjson(j);
  // 0x7B, 0x69, 0x07, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0x54, 0x69, 0x06, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x69, 0x00, 0x7D
  // roundtrip
  json j_from_ubjson=json::from_ubjson(v_ubjson);

  // 写到本地
  std::ofstream ofs("tmp_3_8",std::ios::out|std::ios::binary);
  const auto msgpack=json::to_msgpack(j);
  ofs.write(reinterpret_cast<const char*>(msgpack.data()),msgpack.size());
  ofs.close();

  return 0;
}




// 3.9 从STL容器转换到JSON
int main()
{
  std::vector<int> c_vector{1,2,3,4};
  json j_vec(c_vector);
  PRINT(j_vec);
  // [1,2,3,4]

  std::deque<double> c_deque{1.2,2.3,3.4,4.5};
  json j_deque(c_deque);
  PRINT(j_deque);
  // [1.2,2.3,3.4,4.5]

  std::list<bool> c_list{true,true,false,true};
  json j_list(c_list);
  PRINT(j_list);
  // [true,true,false,true]

  std::forward_list<int64_t> c_flist{12345678909876, 23456789098765, 34567890987654, 45678909876543};
  json j_flist(c_flist);
  PRINT(j_flist);
  // [12345678909876,23456789098765,34567890987654,45678909876543]

  std::array<unsigned long,4> c_array{1,2,3,4};
  json j_array(c_array);
  PRINT(j_array);
  // [1,2,3,4]

  std::set<std::string> c_set{"one", "two", "three", "four", "one"};;
  json j_set(c_set); // only one entry for "one" is used
  PRINT(j_set);
  // ["four", "one", "three", "two"]

  std::unordered_set<std::string> c_uset{"one", "two", "three", "four", "one"};
  json j_uset(c_uset); // only one entry for "one" is used
  PRINT(j_uset);
  // maybe ["four","three","two","one"]

  std::multiset<std::string> c_mset{"one", "two", "one", "four"};
  json j_mset(c_mset); // both entries for "one" are used
  PRINT(j_mset);
  // maybe ["four","one","one","two"]

  std::unordered_multiset<std::string> c_umset{"one", "two", "one", "four"};
  json j_umset(c_umset); // both entries for "one" are used
  PRINT(j_umset);
  // maybe ["four","two","one","one"]

  std::map<std::string,int> c_map{{"one",1},{"two",2},{"three",3}};
  json j_map(c_map);
  PRINT(j_map);
  // {"one":1,"three":3,"two":2}

  std::unordered_map<const char*,double> c_umap{ {"one", 1.2}, {"two", 2.3}, {"three", 3.4} };
  json j_umap(c_umap);
  PRINT(j_umap);
  // {"one":1.2,"three":3.4,"two":2.3}

  std::multimap<std::string,bool> c_mmap{ {"one", true}, {"two", true}, {"three", false}, {"three", true} };
  json j_mmap(c_mmap);// only one entry for key "three" is used
  PRINT(j_mmap);
  // maybe {"one":true,"three":false,"two":true}

  std::unordered_multimap<std::string,bool> c_ummap{ {"one", true}, {"two", true}, {"three", false}, {"three", true} };
  json j_ummap(c_ummap);// only one entry for key "three" is used
  PRINT(j_ummap);
  // maybe {"one":true,"three":true,"two":true}






}






