#ifndef KICAD_PCB_TREE_H
#define KICAD_PCB_TREE_H

#include <vector>
#include <string>

struct Tree
{
    std::string m_value;
    std::vector<Tree> m_branches;
};

#endif
