// https://blog.csdn.net/yueni_zhao/article/details/131194012


#include<iostream>
#include<stdlib.h>
// #include<unistd.h>// unix
#include<Windows.h>// window
#include<string>

#define DEBUG_INFO(format, ...) \
  printf("%s - %d - %s :: " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

// 读取外部程序的输出
void test_01()
{
  FILE* read_fp;
  char buf[BUFSIZ+1];// 512+1
  memset(buf,0,sizeof(sizeof(buf)));
  DEBUG_INFO("BUFSIZ=%d",BUFSIZ);
  read_fp=_popen("dir .","r");
  if(read_fp==NULL){
    perror("popen failed\n");
    return;
  }
  int len=fread(buf,1,BUFSIZ,read_fp);
  if(len<=0){
    perror("fread failed\n");
    _pclose(read_fp);
    return;
  }
  DEBUG_INFO("len=%d,buf=%s",len,buf);
  _pclose(read_fp);
}
// 将输出送往popen
void test_02(void)
{
  FILE *write_fp;
  char buf[BUFSIZ + 1];
  memset(buf, 0, sizeof(buf));
  DEBUG_INFO("BUFSIZ = %d", BUFSIZ);
  sprintf(buf, "0123456789ABCDEFGHIJKLMN\n");

  DEBUG_INFO("len = %d,buf = %s", strlen(buf), buf);
  write_fp = _popen("od -c", "w");
  if (write_fp == NULL)
  {
    perror("popen:");
    return;
  }
  int len = fwrite(buf, sizeof(char), strlen(buf), write_fp);
  if (len <= 0)
  {
    perror("fwrite:");
    _pclose(write_fp);
    return;
  }

  _pclose(write_fp);
}

int main(int argc,char** agrv)
{
  test_01();
  DEBUG_INFO("hello world");
  return 0;
}













