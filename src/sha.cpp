#include"sha.h"

#include <filesystem>
#include "include/sha1.hpp"

namespace SHA{


std::string hash_str(const std::string &in)
{
  SHA1 checksum;
  checksum.update(in);
  const std::string hash = checksum.final();
  return hash;
}

std::string hash_file(const std::string &filename)
{
  if (!std::filesystem::exists(filename))
  {
    return "";
  }
  std::string res = SHA1::from_file(filename);
  return res;
}

}