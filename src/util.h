#ifndef KICAD_PCB_UTIL_H
#define KICAD_PCB_UTIL_H

#include <cmath>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include "tree.h"
#include "point.h"

#define MIN_COPPER_LAYER_ID 0
#define MAX_COPPER_LAYER_ID 31

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
void ss_reset(std::stringstream &ss, const std::string &s);
void get_value(std::stringstream &ss, std::vector<Tree>::iterator t, int &x);
void get_value(std::stringstream &ss, std::vector<Tree>::iterator t, double &x);
void get_2d(std::stringstream &ss, std::vector<Tree>::iterator t, double &x, double &y);
void get_rect(std::stringstream &ss, std::vector<Tree>::iterator t, double &x1, double &y1, double &x2, double &y2);

namespace utilParser
{
// =====================================================
// filename --------------------------------------------
// =====================================================
inline std::string getFileDirName(const std::string filePathName)
{
    std::string retStr = filePathName;
    std::string::size_type pos = retStr.find_last_of("/\\");
    if (pos != std::string::npos)
        retStr = retStr.substr(0, pos);
    return retStr;
}
inline std::string getFileName(const std::string filePathName)
{
    std::string retStr = filePathName;
    std::string::size_type pos = retStr.find_last_of("/\\");
    if (pos != std::string::npos)
        retStr = retStr.substr(pos + 1);
    return retStr;
}
inline std::string getFileNameWoExtension(const std::string filePathName)
{
    std::string retStr = filePathName;
    std::string::size_type pos = retStr.find_last_of("/\\");
    if (pos != std::string::npos)
        retStr = retStr.substr(pos + 1);
    pos = retStr.find_last_of(".");
    if (pos != std::string::npos)
        retStr = retStr.substr(0, pos);
    return retStr;
}
inline std::string getFileExtension(const std::string filePathName)
{
    std::string retStr = filePathName;
    std::string::size_type pos = retStr.rfind(".");
    if (pos != std::string::npos)
        retStr = retStr.substr(pos + 1);
    return retStr;
}
} // namespace utilParser

#endif
