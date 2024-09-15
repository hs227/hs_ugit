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

// O(n^2)
std::string get_LCS(const std::set<std::string>& set1, const std::set<std::string>& set2) 
{
  std::string maxer;
  for (const std::string& set1_en : set1) {
    for (const std::string& set2_en : set2) {
      if (set1_en == set2_en) {
        if (set2_en.size() > maxer.size())
          maxer = set2_en;
      }
    }
  }
  return maxer;
}

// 2O(n^2)+O(n^2)=>O(n^2)
int main() {
  std::set<std::string> iters1;
  std::string input1 = "console.log('hello-world');";
  iter_all_subsequence(input1, iters1);

  std::set<std::string> iters2;
  std::string input2 = "console.log('hi-js');";
  iter_all_subsequence(input2, iters2);


  //for (const std::string& iter : iters1) {
  //  std::cout << iter << std::endl;
  //}
  //std::cout << "--------------------------------------" << std::endl;
  //for (const std::string& iter : iters2) {
  //  std::cout << iter << std::endl;
  //}
    
  
  std::string LCS = get_LCS(iters1, iters2);
  std::cout << LCS << std::endl;

  return 0;
}

