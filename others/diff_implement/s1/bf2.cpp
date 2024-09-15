// 暴力破解法
#include<iostream>
#include<string>
#include<vector>
#include<set>

using namespace std;

// 2+4+6+...+2(n-1)=>n(n-1)=>O(n^2)
void iter_all_subsequence(const std::string& src, std::set<std::string>& subs)
{
  if (src == "")
    return;
  std::set<std::string> next_subs;

  next_subs.insert(src.substr(0, 1));
  for (const auto& iter : subs) {
    std::string tmp = iter + src.substr(0, 1);
    next_subs.insert(tmp);
  }

  iter_all_subsequence(src.substr(1), next_subs);

  for (const auto& iter : next_subs) {
    subs.insert(iter);
  }

}

// 2O(n^2)+O(n^2)=>O(n^2)
std::string get_LCS(const std::string& str1,const std::string& str2) 
{
  std::set<std::string> set1;
  iter_all_subsequence(str1,set1);

  std::string maxer;
  for(const std::string& iter:set1){
    if(str2.find(iter)!=std::string::npos){
      if(iter.size()>maxer.size())
        maxer=iter;
    }
  }
  return maxer;
}
// 实际测试好像没有区别
std::string get_LCS_v2(const std::string& str1,const std::string& str2) 
{
  std::set<std::string> set1;
  iter_all_subsequence(str1,set1);

  std::string maxer;
  for(const std::string& iter:set1){
    if(iter.size()>maxer.size())
      if(str2.find(iter)!=std::string::npos)
        maxer=iter;
  }
  return maxer;
}


int main()
{
  std::string input1 = "console.log('hello-world');";
  std::string input2 = "console.log('hi-js');";
  std::string output=get_LCS(input1,input2);
  std::cout<<output<<std::endl;
  return 0;
}







