// https://www.cnblogs.com/linuxAndMcu/p/14503341.html 
// 四、示例2

/*
{
  "output": {
    "width": 720,
    "height": 1080,
    "frameRate": 20,
    "crf": 31
  },
  "tracks": [
    {
      "name": "t1",
      "pieces": [
        {
          "file": "x.mp4",
          "startTime": 2,
          "endTime": 6
        },
        {
          "file": "y.mp4",
          "startTime": 9,
          "endTime": 13
        }
      ]
    },
    {
      "name": "t2",
      "pieces": [
        {
          "file": "z.mp4",
          "startTime": 0,
          "endTime": 10
        }
      ]
    }
  ]
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
  std::ifstream ifs("test2.json");
  ifs>>j;
  ifs.close();

  // output object
  PRINT(j["output"]["width"]);
  PRINT(j["output"]["height"]);
  PRINT(j["output"]["frameRate"]);
  PRINT(j["output"]["crf"]);

  // tracks object
  for(size_t i=0;i<j["tracks"].size();++i){
    PRINT(j["tracks"][i]["name"]);

    // 打印pieces子数组对象
    json j2=j["tracks"][i]["pieces"];
    for(size_t k=0;k<j2.size();++k){
      PRINT(j2[k]["file"]);
      PRINT(j2[k]["startTime"]);
      PRINT(j2[k]["endTime"]);
    }
  }
  
  return 0;
}



