// https://www.cnblogs.com/linuxAndMcu/p/14503341.html 
// 四、示例1

/*
{
	"pi":3.1415,
	"happy":true
}
*/

#include"../../../src/include/nlohmann/json.hpp"
#include<fstream>
#include<iostream>
using namespace std;
using json=nlohmann::json;

#define PRINT(X) \
  std::cout<< #X ":"<<X<<std::endl


int main()
{
  json j;
  ifstream jfile("test1.json");
  jfile>>j;
  jfile.close();
  float pi=j.at("pi");
  bool happy=j.at("happy");
  PRINT(pi);
  PRINT(bool(happy));

  return 0;
}





