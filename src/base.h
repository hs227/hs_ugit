#ifndef BASE_H
#define BASE_H

#include <iostream>




namespace BASE
{
  struct commit_cxt
  {
    std::string tree;
    std::string parent;
    std::string msg;
    commit_cxt()
        : tree(""), parent(""), msg("")
    {}
  };

  std::string write_tree(std::string =".");
  void read_tree(const std::string& );
  std::string commit(const std::string&);
  commit_cxt get_commit(const std::string&);
  void checkout(const std::string&);
}

#endif
