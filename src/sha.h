#ifndef SHA_H
#define SHA_H


#include<iostream>

namespace SHA
{
  std::string hash_str(const std::string &in);
  std::string hash_file(const std::string &filename);

}

#endif
