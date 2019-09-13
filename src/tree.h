#ifndef KICAD_PCB_TREE_H
#define KICAD_PCB_TREE_H

#include <vector>
#include <string>

struct tree
{
    std::string m_value;
    std::vector<tree> m_branches;
};

#endif
