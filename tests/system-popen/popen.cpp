#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string>

// linus:popen
// windows:_popen

// popen和pclose是C封装的管道操作

std::string cmd(const char* cmd,const int buffer)
{
  FILE* f=_popen(cmd,"r");
  char* _str=(char*)alloca(buffer);
  std::string cmd_str;
  while(fgets(_str,buffer,f)!=NULL){
    cmd_str+=_str;
  }
  _pclose(f);
  return cmd_str;
}


// char type
// int main(){
//   const char *cmd = "dir D:\\1GAMES";
//   const char* mode="r";// 基本都使用读模式
//   FILE* f=_popen(cmd,mode);
//   char buf[1024];
//   while(fgets(buf,1024,f)!=NULL){
//     std::cout << buf << std::endl;
//   }
//   fclose(f);
//   return 0;
// }

// string type
int main()
{
  std::string res=cmd("dir D:\\1GAMES",1024);
  std::cout<<res<<std::endl;
  return 0;
}
