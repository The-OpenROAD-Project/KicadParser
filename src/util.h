#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include "tree.h"
#include "mymath.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
void ss_reset(std::stringstream &ss, const std::string &s);
void get_value(std::stringstream &ss, std::vector<tree>::iterator t, int &x);
void get_value(std::stringstream &ss, std::vector<tree>::iterator t, double &x);
void get_2d(std::stringstream &ss, std::vector<tree>::iterator t, double &x, double &y);
void get_rect(std::stringstream &ss, std::vector<tree>::iterator t, double &x1, double &y1, double &x2, double &y2);


#endif
