// 基于行的Myers差分算法()
#include<iostream>
#include<string>
#include<vector>
#include<deque>
#include<set>
#include<filesystem>
#include<fstream>
#include<sstream>
#include<map>

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
// deque turn to vector
void to_vector(const deque<string>& from, vector<string>& to) {
  to.clear();
  for (const auto& entry : from) {
    to.push_back(entry);
  }
}
// to get subvector ,this function just like substr
vector<string> subvector(const vector<string>& vec, size_t left, size_t right)
{
  vector<string> output;
  const size_t vec_size = vec.size();
  for (; left < right && left < vec_size; ++left) {
    output.push_back(vec[left]);
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

struct MNode {
  int x, y;
  size_t direct;
  int from_x, from_y;
};


// check if the point already be touched;
// and those touched earlier means smaller 'd'.
// Update: When those touched earlier happen in same 'd',depends on which closer to the END
// Update: Remeber 'LEFT' is better than 'UP'
bool is_point_occupied(const MNode& node, vector<map<int, MNode>>& d_dict, int d)
{
  int k = node.x - node.y;
  if (d_dict[d + 1].count(k) > 0) {
    //如果是同一点，需要考虑UP和LEFT的有限度
    if (d_dict[d + 1][k].x == node.x && d_dict[d + 1][k].y == node.y) {
      if (!(d_dict[d + 1][k].direct == LEFT && node.direct == UP))
        return true;
    }
    //如果是同一斜度，选择x大的
    if (d_dict[d + 1][k].x > node.x)
      return true;
  }

  for (; d >= 0; --d) {
    // if(d_dict[d].count(k)!=0)
    //   return true;
    for (const auto& entry : d_dict[d]) {
      if (entry.second.x == node.x && entry.second.y == node.y)
        return true;
    }
  }
  return false;
}

bool to_final_place(MNode& node, const vector<string>& lines1, const vector<string>& lines2,
  vector<map<int, MNode>>& d_dict, size_t d)
{
  bool successed_flag = false;
  int k = node.x - node.y;

  // 超出地图边缘
  if (node.x > lines1.size() || node.y > lines2.size()) {
    return false;
  }
  // 到达地图边缘，没有diagonal了
  if (node.x == lines1.size() || node.y == lines2.size()) {
    goto ADD_NEW;
  }
  node.from_x = node.x;
  node.from_y = node.y;
  while (lines1[node.x] == lines2[node.y])
  {
    node.x++;
    node.y++;
    // 到达地图边缘，没有diagonal了
    if (node.x == lines1.size() || node.y == lines2.size()) {
      break;
    }
  }

ADD_NEW:
  if (is_point_occupied(node, d_dict, d)) {
    // already touched
    return false;
  }
  d_dict[d + 1][k] = node;

  return true;
}

bool is_to_the_end(MNode& node, const vector<string>& lines1, const vector<string>& lines2)
{
  return node.x == lines1.size() && node.y == lines2.size();
}

std::vector<std::string> lines_get_Myers(const vector<string>& lines1, const vector<string>& lines2)
{
  const size_t d_size = lines1.size() + lines2.size();
  vector<map<int, MNode>> d_dict(d_size);//<k,MNode>
  d_dict[0][0] = { 0,0,END };
  size_t d = 0;
  size_t k = 0;
  for (; d < d_size; ++d) {
    for (const auto& entry : d_dict[d]) {
      // each point have two choices:go Right or Go Down
      MNode rightone{ entry.second.x + 1,entry.second.y,LEFT };
      MNode downone{ entry.second.x,entry.second.y + 1,UP };
      // when encounter diagonal always use it
      if (to_final_place(rightone, lines1, lines2, d_dict, d) &&
        is_to_the_end(rightone, lines1, lines2)) {
        k = rightone.x - rightone.y;
        d += 1;
        goto finished;
      }
      if (to_final_place(downone, lines1, lines2, d_dict, d) &&
        is_to_the_end(downone, lines1, lines2)) {
        k = downone.x - downone.y;
        d += 1;
        goto finished;
      }
    }
  }


finished:
  size_t direct = !END;
  deque<string> output;
  for (; direct != END;) {
    MNode node = d_dict[d][k];
    string new_line;
    direct = node.direct;
    while (
      (node.x-1>=0 && node.y-1>=0)&&
      (lines1[node.x - 1] == lines2[node.y - 1]) &&
      (node.x != node.from_x && node.y != node.from_y)) {
      new_line = " " + lines1[node.x - 1];
      output.push_front(new_line);
      node.x--; node.y--;
    }

    switch (direct) {
    case UP:
      new_line = "+" + lines2[node.y - 1];
      output.push_front(new_line);
      d -= 1;
      k += 1;
      break;
    case LEFT:
      new_line = "-" + lines1[node.x - 1];
      output.push_front(new_line);
      d -= 1;
      k -= 1;
      break;
    default:
      break;
    }
  }

  vector<string> output_v;
  to_vector(output, output_v);
  return output_v;
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

  std::string file1;
  std::string file2;
  bool is_lined = false;// unused

  app.add_option("file1", file1, "add first file")->check(CLI::ExistingFile)->required();
  app.add_option("file2", file2, "add second file")->check(CLI::ExistingFile)->required();
  app.add_flag("--lined", is_lined, "is lined here")->required();

  CLI11_PARSE(app, argc, argv);

  file1 = read_file(file1);
  file2 = read_file(file2);

  // opt2：基于行的diff
  diff_lined(file1, file2);


  return 0;
}




int main(int argc, char* argv[])
{
  cli_parse(argc, argv);
  return 0;
}


// 基于行的Myers差分算法()
#include<iostream>
#include<string>
#include<vector>
#include<deque>
#include<set>
#include<filesystem>
#include<fstream>
#include<sstream>
#include<map>

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
// deque turn to vector
void to_vector(const deque<string>& from, vector<string>& to) {
  to.clear();
  for (const auto& entry : from) {
    to.push_back(entry);
  }
}
// to get subvector ,this function just like substr
vector<string> subvector(const vector<string>& vec, size_t left, size_t right)
{
  vector<string> output;
  const size_t vec_size = vec.size();
  for (; left < right && left < vec_size; ++left) {
    output.push_back(vec[left]);
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

struct MNode {
  int x, y;
  size_t direct;
  int from_x, from_y;
};


// check if the point already be touched;
// and those touched earlier means smaller 'd'.
// Update: When those touched earlier happen in same 'd',depends on which closer to the END
// Update: Remeber 'LEFT' is better than 'UP'
bool is_point_occupied(const MNode& node, vector<map<int, MNode>>& d_dict, int d)
{
  int k = node.x - node.y;
  if (d_dict[d + 1].count(k) > 0) {
    //如果是同一点，需要考虑UP和LEFT的有限度
    if (d_dict[d + 1][k].x == node.x && d_dict[d + 1][k].y == node.y) {
      if (!(d_dict[d + 1][k].direct == LEFT && node.direct == UP))
        return true;
    }
    //如果是同一斜度，选择x大的
    if (d_dict[d + 1][k].x > node.x)
      return true;
  }

  for (; d >= 0; --d) {
    // if(d_dict[d].count(k)!=0)
    //   return true;
    for (const auto& entry : d_dict[d]) {
      if (entry.second.x == node.x && entry.second.y == node.y)
        return true;
    }
  }
  return false;
}

bool to_final_place(MNode& node, const vector<string>& lines1, const vector<string>& lines2,
  vector<map<int, MNode>>& d_dict, size_t d)
{
  bool successed_flag = false;
  int k = node.x - node.y;

  // 超出地图边缘
  if (node.x > lines1.size() || node.y > lines2.size()) {
    return false;
  }
  // 到达地图边缘，没有diagonal了
  if (node.x == lines1.size() || node.y == lines2.size()) {
    goto ADD_NEW;
  }
  node.from_x = node.x;
  node.from_y = node.y;
  while (lines1[node.x] == lines2[node.y])
  {
    node.x++;
    node.y++;
    // 到达地图边缘，没有diagonal了
    if (node.x == lines1.size() || node.y == lines2.size()) {
      break;
    }
  }

ADD_NEW:
  if (is_point_occupied(node, d_dict, d)) {
    // already touched
    return false;
  }
  d_dict[d + 1][k] = node;

  return true;
}

bool is_to_the_end(MNode& node, const vector<string>& lines1, const vector<string>& lines2)
{
  return node.x == lines1.size() && node.y == lines2.size();
}

std::vector<std::string> lines_get_Myers(const vector<string>& lines1, const vector<string>& lines2)
{
  const size_t d_size = lines1.size() + lines2.size();
  vector<map<int, MNode>> d_dict(d_size);//<k,MNode>
  d_dict[0][0] = { 0,0,END };
  size_t d = 0;
  size_t k = 0;
  for (; d < d_size; ++d) {
    for (const auto& entry : d_dict[d]) {
      // each point have two choices:go Right or Go Down
      MNode rightone{ entry.second.x + 1,entry.second.y,LEFT };
      MNode downone{ entry.second.x,entry.second.y + 1,UP };
      // when encounter diagonal always use it
      if (to_final_place(rightone, lines1, lines2, d_dict, d) &&
        is_to_the_end(rightone, lines1, lines2)) {
        k = rightone.x - rightone.y;
        d += 1;
        goto finished;
      }
      if (to_final_place(downone, lines1, lines2, d_dict, d) &&
        is_to_the_end(downone, lines1, lines2)) {
        k = downone.x - downone.y;
        d += 1;
        goto finished;
      }
    }
  }


finished:
  size_t direct = !END;
  deque<string> output;
  for (; direct != END;) {
    MNode node = d_dict[d][k];
    string new_line;
    direct = node.direct;
    while (
      (node.x-1>=0 && node.y-1>=0)&&
      (lines1[node.x - 1] == lines2[node.y - 1]) &&
      (node.x != node.from_x && node.y != node.from_y)) {
      new_line = " " + lines1[node.x - 1];
      output.push_front(new_line);
      node.x--; node.y--;
    }

    switch (direct) {
    case UP:
      new_line = "+" + lines2[node.y - 1];
      output.push_front(new_line);
      d -= 1;
      k += 1;
      break;
    case LEFT:
      new_line = "-" + lines1[node.x - 1];
      output.push_front(new_line);
      d -= 1;
      k -= 1;
      break;
    default:
      break;
    }
  }

  vector<string> output_v;
  to_vector(output, output_v);
  return output_v;
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

  std::string file1;
  std::string file2;
  bool is_lined = false;// unused

  app.add_option("file1", file1, "add first file")->check(CLI::ExistingFile)->required();
  app.add_option("file2", file2, "add second file")->check(CLI::ExistingFile)->required();
  app.add_flag("--lined", is_lined, "is lined here")->required();

  CLI11_PARSE(app, argc, argv);

  file1 = read_file(file1);
  file2 = read_file(file2);

  // opt2：基于行的diff
  diff_lined(file1, file2);


  return 0;
}




int main(int argc, char* argv[])
{
  cli_parse(argc, argv);
  return 0;
}


