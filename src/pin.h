#ifndef PIN_H
#define PIN_H

#include "mymath.h"
#include "shape.h"
#include "rule.h"
#include <vector>
#include <string>
#include <iostream>

enum class padType
{
    SMD,
    THRU_HOLE,
    CONNECT,
    NP_THRU_HOLE
};

struct pin
{
    std::string m_name;
    std::string m_comp_name;
    std::string m_instance_name;
};

struct pad
{
    bool operator==(const pad &t) const
    {
        return std::tie(m_radius, m_clearance, m_pos, m_shape) == std::tie(t.m_radius, t.m_clearance, t.m_pos, t.m_shape);
    }
    bool operator<(const pad &t) const { return m_pos < t.m_pos; }
    double m_radius;
    double m_clearance;
    point_3d m_pos;
    points_2d m_shape;
    point_2d m_size;
};
typedef std::vector<pad> pads;

struct padstack
{
    std::string m_name;
    padShape m_form;
    padType m_type;
    point_2d m_pos;
    double m_angle;
    std::vector<std::string> m_layers;
    point_2d m_size; //(width,height)
    points_2d m_shape;
    rule m_rule;
    double m_roundrect_ratio;

    void setForm(std::string &form)
    {
        if (form == "rect")
            m_form = padShape::RECT;
        else if (form == "roundrect")
            m_form = padShape::ROUNDRECT;
        else if (form == "circle")
            m_form = padShape::CIRCLE;
        else if (form == "oval")
            m_form = padShape::OVAL;
        else if (form == "trapezoid")
            m_form = padShape::TRAPEZOID;
        else
            std::cout << "Error: No this pin shape!" << std::endl;
    }

    void setType(std::string &type)
    {
        if (type == "smd")
            m_type = padType::SMD;
        else if (type == "thru_hole")
            m_type = padType::THRU_HOLE;
        else if (type == "connect")
            m_type = padType::CONNECT;
        else if (type == "np_thru_hole")
            m_type = padType::NP_THRU_HOLE;
        else
            std::cout << "Error: No this pin type!" << std::endl;
    }
};
#endif