#ifndef REMOTE_H
#define REMOTE_H

#include<iostream>
#include<string>


namespace REMOTE{
  extern const std::string REMOTE_REFS_BASE;
  extern const std::string LOCAL_REFS_BASE;
  void fetch(const std::string& remote_path);



}

#endif
