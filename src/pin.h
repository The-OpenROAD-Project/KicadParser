#ifndef KICAD_PCB_PIN_H
#define KICAD_PCB_PIN_H

#include <iostream>
#include <string>
#include <vector>
#include "point.h"
#include "rule.h"
#include "shape.h"

enum class padType {
    SMD,
    THRU_HOLE,
    CONNECT,      // ???? Does not see in any of current testcases
    NP_THRU_HOLE  // Similar to THRU_HOLE
};

class Pin {
   public:
    //ctor
    Pin() {}
    Pin(const int padId, const int compId, const int instId)
        : m_padstack_id(padId), m_comp_id(compId), m_inst_id(instId) {}
    //dtor
    ~Pin() {}
    bool operator==(const Pin &p) const {
        return std::tie(m_padstack_id, m_comp_id, m_inst_id) == std::tie(p.m_padstack_id, p.m_comp_id, p.m_inst_id);
    }

    int getPadstackId() const { return m_padstack_id; }
    int getCompId() const { return m_comp_id; }
    int getInstId() const { return m_inst_id; }
    // Should use Instance's layer and Padstack's padtype to decide
    [[deprecated]] const std::vector<int> &getLayers() const { return m_layers; }

    friend class kicadPcbDataBase;

   private:
    int m_padstack_id = -1;
    int m_comp_id = -1;
    int m_inst_id = -1;
    [[deprecated]] std::vector<int> m_layers;  // THRU_HOLE: All CopperLayers, SMD: TOP or BOTTOM
};

struct pad {
    bool operator==(const pad &t) const {
        return std::tie(m_radius, m_clearance, m_pos, m_shape) == std::tie(t.m_radius, t.m_clearance, t.m_pos, t.m_shape);
    }
    bool operator<(const pad &t) const { return m_pos < t.m_pos; }
    double m_radius;     // from rule
    double m_clearance;  // from rule
    point_3d m_pos;
    points_2d m_shape;
    point_2d m_size;
};
typedef std::vector<pad> pads;

//TODO: Move layer to pin
class padstack {
   public:
    //ctor
    padstack() {}
    //dtor
    ~padstack() {}

    int getId() { return m_id; }
    const std::string &getName() const { return m_name; }
    padType getType() const { return m_type; }
    padShape getPadShape() const { return m_shape; }
    point_2d getSize() const { return m_size; }
    point_2d getPos() const { return m_pos; }
    double getAngle() const { return m_angle; }
    double getRoundRectRatio() const { return m_roundrect_ratio; }

    points_2d getShapeCoords() const { return m_shape_coords; }
    // Center at (0, 0)
    const points_2d &getShapePolygon() const { return m_shape_polygon; }

    // Do not consider different instances
    [[deprecated]] const std::vector<std::string> &getLayers() const { return m_layers; }

    friend class kicadPcbDataBase;

   private:
    // Do not consider rotation
    [[deprecated]] double getWidth() const { return m_size.m_x; }
        [[deprecated]] double getHeight() const { return m_size.m_y; }
    [[deprecated]] double getHalfWidth() const { return m_size.m_x / 2.0; }
        [[deprecated]] double getHalfHeight() const { return m_size.m_y / 2.0; }

   private:
    int m_id;  // Local Id to a Component
    std::string m_name;
    padShape m_shape;
    padType m_type;
    point_2d m_pos;                                    // Relative position to Instance's position
    double m_angle;                                    // Relative angle to Instance's angles
    point_2d m_size;                                   // All shapes has (width, height) from KiCad
    rule m_rule;                                       // {m_radius (CIRCLE only), m_clearance}
    double m_roundrect_ratio;                          // ROUNDRECT only
    [[deprecated]] std::vector<std::string> m_layers;  // THRU_HOLE: All CopperLayers, SMD: TOP or BOTTOM

    // Derived
    points_2d m_shape_coords;   //Transformed // For C-PCB visualiztion purpose
    points_2d m_shape_polygon;  //Transformed by function shape_to_coords()

    void setShape(std::string &shape) {
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

    void setType(std::string &type) {
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
