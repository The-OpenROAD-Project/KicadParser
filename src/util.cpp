#include "util.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void ss_reset(std::stringstream &ss, const std::string &s)
{
    ss.str(s);
    ss.clear();
}

void get_value(std::stringstream &ss, std::vector<tree>::iterator t, int &x)
{
    ss_reset(ss, t->m_value);
    ss >> x;
}

void get_value(std::stringstream &ss, std::vector<tree>::iterator t, double &x)
{
    ss_reset(ss, t->m_value);
    ss >> x;
}

void get_2d(std::stringstream &ss, std::vector<tree>::iterator t, double &x, double &y)
{
    get_value(ss, t, x);
    get_value(ss, t + 1, y);
}

void get_rect(std::stringstream &ss, std::vector<tree>::iterator t, double &x1, double &y1, double &x2, double &y2)
{
    get_2d(ss, t, x1, y1);
    get_2d(ss, t + 2, x2, y2);
}
