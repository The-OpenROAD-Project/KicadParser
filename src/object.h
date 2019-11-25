#ifndef PCBDRC_OBJECT_H
#define PCBDRC_OBJECT_H

#include "pcbBoost.h"
#include "kicadPcbDataBase.h"
#include "module.h"
#include "net.h"
#include "point.h"
#include "segment.h"
#include "shape.h"
#include "via.h"

enum class ObjectType
{
    PIN,
    VIA,
    SEGMENT,
    NONE
};

class Object
{
public:
    Object(const ObjectType type = ObjectType::NONE,
           const int dbId = -1,
           const int netId = -1,
           const int compId = -1,
           const int instId = -1)
        : m_type(type), m_dbId(dbId), m_netId(netId), m_compId(compId), m_instId(instId){};
    ~Object(){};
    void setRTreeId(const std::pair<int, int> &id) { m_ids.push_back(id); }
    void setBBox(const box &b) { m_bbox = b; }
    void setShape(const points_2d &s) { m_shape = s; }
    void setRelativeShape(const points_2d &s) { m_relativeShape = s; }
    void setPoly(const polygon_t &poly) { m_poly = poly; }
    void setPos(const points_2d &pos) { m_pos = pos; }
    void setX(const double &x) { m_pos[0].m_x = x; }
    void setY(const double &y) { m_pos[0].m_y = y; }
    void setPos(const point_2d &pos) { m_pos.push_back(pos); }
    box &getBBox() { return m_bbox; }
    ObjectType &getType() { return m_type; }
    int &getDBId() { return m_dbId; }
    int &getCompId() { return m_compId; }
    int &getInstId() { return m_instId; }
    points_2d &getShape() { return m_shape; }
    points_2d &getRelativeShape() { return m_relativeShape; }
    //point_2d &getPos() { return m_pos[0];}
    points_2d &getPos() { return m_pos; }
    double &getX()
    {
        point_2d pos = getCenterPos();
        return pos.m_x;
    }
    double &getY()
    {
        point_2d pos = getCenterPos();
        return pos.m_y;
    }

    point_2d getCenterPos()
    {
        auto p = point_2d{};
        if (m_type == ObjectType::SEGMENT)
        {
            p.m_x = (m_pos[0].m_x + m_pos[1].m_x) / 2;
            p.m_y = (m_pos[0].m_y + m_pos[1].m_y) / 2;
        }
        else
        {
            p = m_pos[0];
        }
        return p;
    }
    polygon_t &getPoly() { return m_poly; }
    int &getNetId() { return m_netId; }
    int &getLayer() { return m_layer; }
    std::vector<std::pair<int, int>> &getRtreeId() { return m_ids; }

    int &getId() { return m_ids[0].second; }
    bool operator==(const Object &obj) const
    {
        return std::tie(m_type, m_dbId, m_netId) == std::tie(obj.m_type, obj.m_dbId, obj.m_netId);
    }

    bool operator!=(const Object &obj) const
    {
        return std::tie(m_type, m_dbId, m_netId, m_ids[0].second) != std::tie(obj.m_type, obj.m_dbId, obj.m_netId, m_ids[0].second);
    }

    void addEquation(std::vector<double> &equ)
    {
        m_equs.push_back(equ);
    }
    std::vector<std::vector<double>> &getEquations()
    {
        return m_equs;
    }

private:
    ObjectType m_type;
    int m_dbId; // id in kicad database
    int m_netId;
    int m_compId;
    int m_instId;
    points_2d m_shape;
    points_2d m_relativeShape;
    polygon_t m_poly;
    points_2d m_pos;
    int m_layer;

    box m_bbox;
    std::vector<std::pair<int, int>> m_ids; //< the ith rtree, id in rtree >

    std::vector<std::vector<double>> m_equs;
};

#endif
