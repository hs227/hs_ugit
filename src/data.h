#ifndef DATA_H
#define DATA_H

#include<iostream>

#define BOOL_STR(x) \
  ((x) ? "True" : "False")

namespace DATA{
const std::string CUR_DIR="../lab_space/";// for this project 
const std::string GIT_DIR = ".ugit";
const std::string LAB_GIT_DIR=CUR_DIR+GIT_DIR;

void init();

}

#endif
