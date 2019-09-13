#ifndef KICAD_PCB_MODULE_H
#define KICAD_PCB_MODULE_H

#include "shape.h"
#include "pin.h"
#include <string>
#include <map>
#include <vector>

struct component
{
    std::string m_name;
    std::map<std::string, padstack> m_pin_map;
    std::vector<line> m_lines;
    std::vector<circle> m_circles;
    std::vector<poly> m_polys;
    std::vector<arc> m_arcs;
    int m_layer;
};

struct instance
{
    std::string m_name;
    std::string m_comp;
    std::string m_side;
    double m_x;
    double m_y;
    double m_angle;
    double m_width;
    double m_height;
    std::map<std::string, int> m_pin_net_map;
};

#endif