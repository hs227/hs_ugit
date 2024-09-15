// 动态规划法 LongestCommonSequence
#include<iostream>
#include<string>
#include<vector>
#include<set>


using namespace std;


using Node = pair<int, int>;

#define LEFT    4
#define UP      8
#define STILL   5
#define LEFT_UP 7

// O(m*n)
std::string get_LCS(const std::string& str1, const std::string& str2)
{
  const std::string strm = " " + str1, strn = " " + str2;
  const size_t M = strm.size() , N = strn.size();
  vector<vector<Node>> mat(M, vector<Node>(N));

  for (size_t m_ = 0; m_ < M; ++m_) mat[m_][0] = { 0,STILL };
  for (size_t n_ = 0; n_ < N; ++n_) mat[0][n_] = { 0,STILL };

  for (size_t m_ = 1; m_ < M; ++m_) {
    for (size_t n_ = 1; n_ < N; ++n_) {
      if (strm[m_] == strn[n_]) {
        // dp[m][n]=dp[m-1][n-1]+1
        mat[m_][n_].first = mat[m_ - 1][n_ - 1].first + 1;
        mat[m_][n_].second = LEFT_UP;
      }
      else {
        // dp[m][n]=max(dp[m-1][n],dp[m][n-1])
        if (mat[m_][n_ - 1].first > mat[m_ - 1][n_].first) {
          mat[m_][n_].first = mat[m_][n_ - 1].first;
          mat[m_][n_].second = LEFT;
        }
        else {
          mat[m_][n_].first = mat[m_ - 1][n_].first;
          mat[m_][n_].second = UP;
        }
      }
    }
  }

  int direct = 0;
  std::string output;
  for (size_t m_ = M - 1, n_ = N - 1; direct != STILL;) {
    direct = mat[m_][n_].second;
    switch (direct) {
      case UP:
        output = "<D>" + output;
        output = strm[m_] + output;
        m_ -= 1;
        break;
      case LEFT:
        output = "<N>" + output;
        output = strn[n_] + output;
        n_ -= 1;
        break;
      case LEFT_UP:
        output = strm[m_] + output;
        m_ -= 1; n_ -= 1;
        break;
      default:
        break;
    }
  }
  return output;
}


int main()
{
  std::string input1 = "console.log('hello-world');";
  std::string input2 = "console.log('hi-js');";
  std::string output = get_LCS(input1, input2);
  std::cout << output << std::endl;
  return 0;
}






