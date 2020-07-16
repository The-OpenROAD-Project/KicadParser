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
        : m_type(type), m_dbId(dbId), m_netId(netId), m_compId(compId), m_instId(instId), m_locked(false)
    {
        m_extraSpace.first = 0;
        m_extraSpace.second = 0;
        m_prePos.emplace_back(point_2d(0, 0));
    };
    ~Object(){};
    void setRTreeId(const std::pair<int, int> &id) { m_ids.push_back(id); }
    void setBBox(const box &b) { m_bbox = b; }
    void setShape(const points_2d &s) { m_shape = s; }
    void setRelativeShape(const points_2d &s) { m_relativeShape = s; }
    void setPoly(const polygon_t &poly) { m_poly = poly; }
    void setMultipoly(const multipoly &poly) { m_mpoly = poly; }
    void setPos(const points_2d &pos) { m_pos = pos; }
    void setX(const double &x) { m_pos[0].m_x = x; }
    void setY(const double &y) { m_pos[0].m_y = y; }
    void setPos(const point_2d &pos) { m_pos.push_back(pos); }
    void setLocked(const bool locked) { m_locked = locked; }
    void setLayer(const int layer) { m_layer = layer; }
    void setExtraSpace(const double &width, const std::string &type)
    {
        if (type == "right")
        {
            m_extraSpace.second = width;
        }
        else if (type == "left")
        {
            m_extraSpace.first = width;
        }
    }
    bool isLocked() { return m_locked; }
    box &getBBox() { return m_bbox; }
    ObjectType &getType() { return m_type; }
    int &getDBId() { return m_dbId; }
    int &getCompId() { return m_compId; }
    int &getInstId() { return m_instId; }
    points_2d &getShape() { return m_shape; }
    points_2d &getRelativeShape() { return m_relativeShape; }
    //point_2d &getPos() { return m_pos[0];}
    points_2d &getPos() { return m_pos; }
    bool isBus() { return m_bus; }
    bool setIsBus(bool isBus) { m_bus = isBus; }
    std::pair<double, double> getExtraSpace() { return m_extraSpace; }
    double getX()
    {
        point_2d pos = getCenterPos();
        return pos.m_x;
    }
    double getY()
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

    int getAngle()
    {
        int angle = 0;
        if (m_type == ObjectType::SEGMENT)
        {
            if (m_pos[0].m_x == m_pos[1].m_x)
            {
                angle = 90;
            }
            else if (m_pos[0].m_y == m_pos[1].m_y)
            {
                angle = 0;
            }
            else if ((m_pos[0].m_x > m_pos[1].m_x && m_pos[0].m_y > m_pos[1].m_y) ||
                     (m_pos[1].m_x > m_pos[0].m_x && m_pos[1].m_y > m_pos[0].m_y))
            {
                angle = 45;
            }
            else if ((m_pos[0].m_x < m_pos[1].m_x && m_pos[0].m_y > m_pos[1].m_y) ||
                     (m_pos[1].m_x < m_pos[0].m_x && m_pos[1].m_y > m_pos[0].m_y))
            {
                angle = 135;
            }
        }
        return angle;
    }
    polygon_t &getPoly() { return m_poly; }
    multipoly &getMultipoly() { return m_mpoly; }
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
    void clearEquation()
    {
        m_equs.clear();
    }
    void printObject()
    {
        std::cout << "Net: " << m_netId;
        if (m_type == ObjectType::PIN)
        {
            std::cout << " Pin " << m_dbId << " comp: " << m_compId << " inst: " << m_instId << std::endl;
        }
        else if (m_type == ObjectType::SEGMENT)
        {
            std::cout << "Segment " << m_dbId << std::endl;
        }
        else if (m_type == ObjectType::VIA)
        {
            std::cout << "Via " << m_dbId << std::endl;
        }

        printPolygon();

        std::cout << "position: ";
        for (auto &&p : m_pos)
        {
            std::cout << p << " ";
        }
        std::cout << std::endl;

        auto center = getCenterPos();
        std::cout << "center position: " << center;
        std::cout << std::endl;
    }

    void updateShape(std::string type, double &diff)
    {
        for (auto &&point : m_shape)
        {
            if (type == "x")
            {
                point.m_x = point.m_x + diff;
            }
            else if (type == "y")
            {
                point.m_y = point.m_y + diff;
            }
        }
    }

    void printPolygon()
    {
        std::cout << "Polygon(";
        for (size_t i = 0; i < m_shape.size(); ++i)
        {

            std::cout << m_shape[i];
            if (i != 7)
                std::cout << ", ";
        }
        std::cout << ")" << std::endl;
    }

    double getLength()
    {
        if (m_pos.size() != 2)
            return 0;
        double length = m_pos[0].getDistance(m_pos[0], m_pos[1]);
        return length;
    }

    void setPreviousPosition()
    {
        m_prePos = m_pos;
    }

    bool isMoved()
    {
        if (m_prePos == m_pos)
            return false;
        return true;
    }

    points_2d &getPreviousPosition()
    {
        return m_prePos;
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
    points_2d m_prePos;
    int m_layer;
    bool m_locked;
    bool m_bus;
    std::pair<double, double> m_extraSpace; //<left width, right, width>
    box m_bbox;
    std::vector<std::pair<int, int>> m_ids; //< the ith rtree, id in rtree >

    std::vector<std::vector<double>> m_equs;
    multipoly m_mpoly;
};

#endif
