#ifndef KICAD_PCB_PIN_H
#define KICAD_PCB_PIN_H

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

//TOOD: Change to int id based recording
struct pin
{
    int m_padstack_id;
    int m_comp_id;
    int m_inst_id;
};

struct pad
{
    bool operator==(const pad &t) const
    {
        return std::tie(m_radius, m_clearance, m_pos, m_shape) == std::tie(t.m_radius, t.m_clearance, t.m_pos, t.m_shape);
    }
    bool operator<(const pad &t) const { return m_pos < t.m_pos; }
    double m_radius;    // from rule
    double m_clearance; // from rule
    point_3d m_pos;
    points_2d m_shape;
    point_2d m_size;
};
typedef std::vector<pad> pads;

class padstack
{
public:
    //ctor
    padstack() {}
    //dtor
    ~padstack() {}

    int getId() { return m_id; }
    std::string &getName() { return m_name; }

    friend class kicadPcbDataBase;

private:
    int m_id;
    std::string m_name;
    padShape m_shape;
    padType m_type;
    point_2d m_pos;           //Relative position to Instance's position
    double m_angle;           //Relative angle to Instance's angles
    point_2d m_size;          //All shapes has (width, height) from KiCad
    rule m_rule;              //{m_radius (CIRCLE only), m_clearance}
    double m_roundrect_ratio; //ROUNDRECT only
    std::vector<std::string> m_layers;

    // Derived
    points_2d m_shape_coords; //Transformed

    void setShape(std::string &shape)
    {
        if (shape == "rect")
            m_shape = padShape::RECT;
        else if (shape == "roundrect")
            m_shape = padShape::ROUNDRECT;
        else if (shape == "circle")
            m_shape = padShape::CIRCLE;
        else if (shape == "oval")
            m_shape = padShape::OVAL;
        else if (shape == "trapezoid")
            m_shape = padShape::TRAPEZOID;
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