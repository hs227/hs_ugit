// Myers的动态规划法基于：（s1/pre_process）
#include<iostream>
#include<string>
#include<vector>
#include<deque>
#include<set>
#include<filesystem>
#include<fstream>
#include<sstream>

#include"include/CLI11.hpp"


using namespace std;


using Node = pair<int, int>;

#define LEFT    4
#define UP      8
#define END     5
#define LEFT_UP 7

// input:filepath
// output:file_content
std::string read_file(const std::string& filepath)
{
  std::string content;
  std::ifstream ifs(filepath, std::ios::ate | std::ios::binary);
  if (!ifs.is_open()) {
    return "";
  }
  content.resize(ifs.tellg());
  ifs.seekg(0);
  ifs.read((char*)content.data(), content.size());
  ifs.close();

  return content;
}
// input:src
// output:splited_lines
// in Window('\r\n')
std::vector<std::string> split_line(const std::string& src)
{
  std::vector<std::string> lines;
  std::stringstream ss(src);
  std::string line;
  while (std::getline(ss, line)) {
    if (line[line.size() - 1] == '\r') {
      line = line.substr(0, line.size() - 1);
    }
    lines.push_back(line);
  }
  return lines;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& v)
{
  std::string content;
  for (const auto& entry : v) {
    content += entry + "\n";
  }
  out << content;
  return out;
}

vector<string>& operator+(vector<string>& lhs, const vector<string>& rhs) {
  for (const auto& entry : rhs) {
    lhs.push_back(entry);
  }
  return lhs;
}

void to_vector(const deque<string>& from, vector<string>& to) {
  to.clear();
  for (const auto& entry : from) {
    to.push_back(entry);
  }
}

vector<string> subvector(vector<string>& vec, size_t left, size_t right)
{
  vector<string> output;
  const size_t vec_size = vec.size();
  for (; left < right && left < vec_size; ++left) {
    output.push_back(vec[left]);
  }
  return output;
}

std::string CommonPrefix(const std::string& str1, const std::string& str2)
{
  std::string output;
  const size_t min_size = min(str1.size(), str2.size());
  for (size_t i = 0; i < min_size; ++i) {
    if (str1[i] != str2[i])
      break;
    output += str1[i];
  }
  return output;
}

std::string CommonSuffix(const std::string& str1, const std::string& str2)
{
  string::const_reverse_iterator iter1 = str1.rbegin();
  string::const_reverse_iterator iter2 = str2.rbegin();
  string output;
  for (; iter1 != str1.rend() && iter2 != str2.rend(); iter1++, iter2++) {
    if (*iter1 != *iter2)
      break;
    output = *iter1 + output;
  }
  return output;
}

vector<string> CommonPrefix_lines(const vector<string>& lines1, const vector<string>& lines2)
{
  vector<string> outputs;
  const size_t min_size = min(lines1.size(), lines2.size());
  for (size_t i = 0; i < min_size; ++i) {
    if (lines1[i] != lines2[i])
      break;
    outputs.push_back(lines1[i]);
  }
  return outputs;
}

vector<string> CommonSuffix_lines(const vector<string>& lines1, const vector<string>& lines2)
{
  vector<string>::const_reverse_iterator iter1 = lines1.rbegin();
  vector<string>::const_reverse_iterator iter2 = lines2.rbegin();
  deque<string> output;
  for (; iter1 != lines1.rend() && iter2 != lines2.rend(); iter1++, iter2++) {
    if (*iter1 != *iter2)
      break;
    output.push_front(*iter1);
  }
  vector<string> output_v;
  to_vector(output, output_v);
  return output_v;
}


// O(m*n)
std::string line_get_LCS(const std::string& str1, const std::string& str2)
{
  const std::string strm = " " + str1, strn = " " + str2;
  const size_t M = strm.size(), N = strn.size();
  vector<vector<Node>> mat(M, vector<Node>(N));

  for (size_t m_ = 0; m_ < M; ++m_) mat[m_][0] = { 0,UP };
  for (size_t n_ = 0; n_ < N; ++n_) mat[0][n_] = { 0,LEFT };
  mat[0][0] = { 0,END };

  for (size_t m_ = 1; m_ < M; ++m_) {
    for (size_t n_ = 1; n_ < N; ++n_) {
      if (strm[m_] == strn[n_]) {
        // dp[m][n]=dp[m-1][n-1]+1
        mat[m_][n_].first = mat[m_ - 1][n_ - 1].first + 1;
        mat[m_][n_].second = LEFT_UP;
      }
      else {
        // dp[m][n]=max(dp[m-1][n],dp[m][n-1])
        if (mat[m_][n_ - 1].first >= mat[m_ - 1][n_].first) {
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
  for (size_t m_ = M - 1, n_ = N - 1; direct != END;) {
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

// O(m*n)
std::string line_get_Myers(const std::string& str1, const std::string& str2)
{
  const std::string strm = " " + str1, strn = " " + str2;
  const size_t M = strm.size(), N = strn.size();
  vector<vector<Node>> mat(M, vector<Node>(N));

  for (size_t m_ = 0; m_ < M; ++m_) mat[m_][0] = { 0,UP };
  for (size_t n_ = 0; n_ < N; ++n_) mat[0][n_] = { 0,LEFT };
  mat[0][0] = { 0,END };

  for (size_t m_ = 1; m_ < M; ++m_) {
    for (size_t n_ = 1; n_ < N; ++n_) {
      if (strm[m_] == strn[n_]) {
        // dp[m][n]=dp[m-1][n-1]
        mat[m_][n_].first = mat[m_ - 1][n_ - 1].first;
        mat[m_][n_].second = LEFT_UP;
      }
      else {
        // dp[m][n]=max(dp[m-1][n],dp[m][n-1])+1
        if (mat[m_][n_ - 1].first < mat[m_ - 1][n_].first) {
          mat[m_][n_].first = mat[m_][n_ - 1].first + 1;
          mat[m_][n_].second = LEFT;
        }
        else {
          mat[m_][n_].first = mat[m_ - 1][n_].first + 1;
          mat[m_][n_].second = UP;
        }
      }
    }
  }

  int direct = 0;
  std::string output;
  for (size_t m_ = M - 1, n_ = N - 1; direct != END;) {
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


std::vector<std::string> lines_get_LCS(const vector<std::string>& lines1, const vector<std::string>& lines2)
{
  std::vector<string> linesm = { "" }; linesm = linesm + lines1;
  std::vector<string> linesn = { "" }; linesn = linesn + lines2;
  const size_t M = linesm.size(), N = linesn.size();
  vector<vector<Node>> mat(M, vector<Node>(N));

  for (size_t m_ = 0; m_ < M; ++m_) mat[m_][0] = { 0,UP };
  for (size_t n_ = 0; n_ < N; ++n_) mat[0][n_] = { 0,LEFT };
  mat[0][0] = { 0,END };

  for (size_t m_ = 1; m_ < M; ++m_) {
    for (size_t n_ = 1; n_ < N; ++n_) {
      if (linesm[m_] == linesn[n_]) {
        // dp[m][n]=dp[m-1][n-1]+1
        mat[m_][n_].first = mat[m_ - 1][n_ - 1].first + 1;
        mat[m_][n_].second = LEFT_UP;
      }
      else {
        // dp[m][n]=max(dp[m-1][n],dp[m][n-1])
        if (mat[m_][n_ - 1].first >= mat[m_ - 1][n_].first) {
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
  deque<string> output;
  for (size_t m_ = M - 1, n_ = N - 1; direct != END;) {
    std::string new_line;
    direct = mat[m_][n_].second;
    switch (direct) {
    case UP:
      new_line = "-" + linesm[m_];
      output.push_front(new_line);
      m_ -= 1;
      break;
    case LEFT:
      new_line = "+" + linesn[n_];
      output.push_front(new_line);
      n_ -= 1;
      break;
    case LEFT_UP:
      new_line = " " + linesm[m_];
      output.push_front(new_line);
      m_ -= 1; n_ -= 1;
      break;
    default:
      break;
    }
  }
  vector<string> output_v;
  to_vector(output, output_v);
  return output_v;
}
// dp[]=d
std::vector<std::string> lines_get_Myers(const vector<string>& lines1, const vector<string>& lines2)
{
  std::vector<string> linesm = { "" }; linesm = linesm + lines1;
  std::vector<string> linesn = { "" }; linesn = linesn + lines2;
  const size_t M = linesm.size(), N = linesn.size();
  vector<vector<Node>> mat(M, vector<Node>(N));

  for (size_t m_ = 0; m_ < M; ++m_) mat[m_][0] = { 0,UP };
  for (size_t n_ = 0; n_ < N; ++n_) mat[0][n_] = { 0,LEFT };
  mat[0][0] = { 0,END };

  for (size_t m_ = 1; m_ < M; ++m_) {
    for (size_t n_ = 1; n_ < N; ++n_) {
      if (linesm[m_] == linesn[n_]) {
        // dp[m][n]=dp[m-1][n-1]
        mat[m_][n_].first = mat[m_ - 1][n_ - 1].first;
        mat[m_][n_].second = LEFT_UP;
      }
      else {
        // dp[m][n]=max(dp[m-1][n],dp[m][n-1])+1
        if (mat[m_][n_ - 1].first < mat[m_ - 1][n_].first) {
          mat[m_][n_].first = mat[m_][n_ - 1].first + 1;
          mat[m_][n_].second = LEFT;
        }
        else {
          mat[m_][n_].first = mat[m_ - 1][n_].first + 1;
          mat[m_][n_].second = UP;
        }
      }
    }
  }

  int direct = 0;
  deque<string> output;
  for (size_t m_ = M - 1, n_ = N - 1; direct != END;) {
    std::string new_line;
    direct = mat[m_][n_].second;
    switch (direct) {
    case UP:
      new_line = "-" + linesm[m_];
      output.push_front(new_line);
      m_ -= 1;
      break;
    case LEFT:
      new_line = "+" + linesn[n_];
      output.push_front(new_line);
      n_ -= 1;
      break;
    case LEFT_UP:
      new_line = " " + linesm[m_];
      output.push_front(new_line);
      m_ -= 1; n_ -= 1;
      break;
    default:
      break;
    }
  }
  vector<string> output_v;
  to_vector(output, output_v);
  return output_v;

}

std::string diff(const std::string& input1_, const std::string& input2_)
{
  std::string input1(input1_);
  std::string input2(input2_);
  std::string output;
  if (input1 == input2) {
    // opt1:全文Equality 
    output = input1;
    std::cout << output << std::endl;
    return output;
  }
  // opt3: Common Prefix/Suffix
  std::string com_prefix = CommonPrefix(input1, input2);
  std::string com_suffix = CommonSuffix(input1, input2);
  size_t head = com_prefix.size();
  size_t in1_tail = input1.size() - com_prefix.size() - com_suffix.size();
  size_t in2_tail = input2.size() - com_prefix.size() - com_suffix.size();
  input1 = input1.substr(head, in1_tail);
  input2 = input2.substr(head, in2_tail);

  output = line_get_LCS(input1, input2);
  output = com_prefix + output + com_suffix;

  std::cout << output << std::endl;
  return output;
}
// opt2：基于行的diff
string diff_lined(const string& input1_, const string& input2_)
{
  string input1(input1_);
  string input2(input2_);
  string output;
  if (input1 == input2) {
    // opt1:全文Equality 
    output = input1;
    std::cout << output << std::endl;
    return output;
  }
  vector<string> lines1 = split_line(input1);
  vector<string> lines2 = split_line(input2);

  // opt3: Common Prefix/Suffix
  vector<string> com_prefix = CommonPrefix_lines(lines1, lines2);
  vector<string> com_suffix = CommonSuffix_lines(lines1, lines2);
  size_t head = com_prefix.size();
  size_t in1_tail = lines1.size() - com_suffix.size();
  size_t in2_tail = lines2.size() - com_suffix.size();
  lines1 = subvector(lines1, head, in1_tail);
  lines2 = subvector(lines2, head, in2_tail);


  vector<string> output_v = lines_get_Myers(lines1, lines2);
  for (const string& str : com_prefix)
    output += " " + str + "\n";
  for (const string& str : output_v)
    output += str + "\n";
  for (const string& str : com_suffix)
    output += " " + str + "\n";
  std::cout << output << std::endl;
  return output;
}

int cli_parse(int argc, char* argv[])
{
  CLI::App app("diff");

  //app.add_option();

  std::string file1;
  std::string file2;
  bool is_lined = false;

  app.add_option("file1", file1, "add first file")->check(CLI::ExistingFile)->required();
  app.add_option("file2", file2, "add second file")->check(CLI::ExistingFile)->required();
  app.add_flag("--lined", is_lined);

  CLI11_PARSE(app, argc, argv);

  file1 = read_file(file1);
  file2 = read_file(file2);

  if (is_lined) {
    // opt2：基于行的diff
    diff_lined(file1, file2);
  }
  else {
    diff(file1, file2);
  }


  return 0;
}




int main(int argc, char* argv[])
{
  cli_parse(argc, argv);
  return 0;
}






