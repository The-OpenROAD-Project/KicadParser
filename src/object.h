#ifndef PCBDRC_OBJECT_H
#define PCBDRC_OBJECT_H


#include "kicadPcbDataBase.h"
#include "net.h"
#include "module.h"
#include "segment.h"
#include "via.h"
#include "point.h"
#include "shape.h"
#include <boost/geometry.hpp>
#include <boost/mpl/string.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp> 


#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/indexable.hpp>
#include <boost/foreach.hpp>


namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<double, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<box, int> value;
typedef bg::model::polygon<point> polygon_t;

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
        Object( const ObjectType type = ObjectType::NONE,
                const int dbId = -1,
                const int netId = -1,  
                const int compId = -1,
                const int instId = -1)
                : m_type(type), m_dbId(dbId), m_netId(netId), m_compId(compId), m_instId(instId){};
        ~Object(){};
        void setRTreeId(const std::pair<int,int> &id) {m_ids.push_back(id); }
        void setBBox(const box &b) { m_bbox = b;}
        void setShape(const points_2d &s) { m_shape = s;}
        void setPoly(const polygon_t &poly) { m_poly = poly;}
        void setPos(const points_2d &pos) { m_pos = pos;}
        void setPos(const point_2d &pos) { m_pos.push_back(pos);}
        box &getBBox() { return m_bbox;}
        ObjectType &getType() { return m_type;}
        int &getDBId() { return m_dbId;}
        int &getCompId() {return m_compId;}
        int &getInstId() {return m_instId;}
        points_2d &getShape() { return m_shape;}
        //point_2d &getPos() { return m_pos[0];}
        points_2d &getPos() { return m_pos;}
        polygon_t &getPoly() { return m_poly;}
        int &getNetId() { return m_netId;}
        int &getLayer() {return m_layer;}
        std::vector< std::pair<int,int> > &getRtreeId(){ return m_ids;}

        int &getId() { return m_ids[0].second;}
        bool operator==(const Object &obj) const {
            return std::tie(m_type, m_dbId, m_netId) == std::tie(obj.m_type, obj.m_dbId, obj.m_netId);
        }

    private:
        ObjectType m_type;  
        int m_dbId;         // id in kicad database
        int m_netId;
        int m_compId;
        int m_instId;
        points_2d m_shape;
        polygon_t m_poly;
        points_2d m_pos;
        int m_layer;

        box m_bbox;
        std::vector< std::pair<int, int> > m_ids;  //< the ith rtree, id in rtree >
};




#endif