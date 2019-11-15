#ifndef KICAD_PCB_KICADPCB_BOOST_H
#define KICAD_PCB_KICADPCB_BOOST_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/mpl/string.hpp>
#include <boost/foreach.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/indexable.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
using point = bg::model::point<double, 2, bg::cs::cartesian>;
using box = bg::model::box<point>;
using value = std::pair<box, int>;
using polygon_t = bg::model::polygon<point>;

#endif