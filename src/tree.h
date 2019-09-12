#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>

struct tree
{
    std::string m_value;
    std::vector<tree> m_branches;
};

#endif
