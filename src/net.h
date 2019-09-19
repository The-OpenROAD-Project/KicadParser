#ifndef KICAD_PCB_NET_H
#define KICAD_PCB_NET_H

#include "mymath.h"
#include "pin.h"
#include "rule.h"
#include <map>
#include <set>
#include <cmath>
#include <string>

class net;

//netlist structures
typedef std::vector<point_3d> path;
typedef std::vector<path> paths;

struct track
{
    std::string m_id;
    double m_track_radius;
    double m_via_radius;
    double m_clearance;
    pads m_pads;
    paths m_paths;
};
typedef std::vector<track> tracks;
//typedef std::vector<net> nets;

class netclass
{
public:
    //ctor
    netclass(const int id = -1,
             const std::string name = "defaultName",
             const double clearance = 0.0,
             const double trace_width = 0.0,
             const double via_dia = 0.0,
             const double via_drill = 0.0,
             const double uvia_dia = 0.0,
             const double uvia_drill = 0.0)
        : m_id(id), m_name(name), m_clearance(clearance), m_trace_width(trace_width), m_via_dia(via_dia), m_via_drill(via_drill), m_uvia_dia(uvia_dia), m_uvia_drill(uvia_drill) {}
    //dtor
    ~netclass() {}

    int getId() { return m_id; }
    std::string &getName() { return m_name; }
    double getClearance() { return m_clearance; }
    double getTraceWidth() { return m_trace_width; }
    double getViaDia() { return m_via_dia; }
    double getViaDrill() { return m_via_drill; }
    double getMicroViaDia() { return m_uvia_dia; }
    double getMicroViaDrill() { return m_uvia_drill; }

private:
    int m_id = -1;
    std::string m_name = "defaultName";
    double m_clearance = 0.0;
    double m_trace_width = 0.0;
    double m_via_dia = 0.0;
    double m_via_drill = 0.0;
    double m_uvia_dia = 0.0;
    double m_uvia_drill = 0.0;
};

class net
{
public:
    net(const int id = -1,
        const std::string name = "defaultName",
        const int netclass_id = -1,
        const std::pair<int, int> pair = std::make_pair(-1, -1))
        : m_id(id), m_name(name), m_netclass_id(netclass_id), m_diff_pair(pair) {}

    int getId() { return m_id; }
    std::string &getName() { return m_name; }
    int getNetclassId() { return m_netclass_id; }
    void addPin(const pin &_pin) { m_pins.push_back(_pin); }
    std::vector<pin> &getPins() { return m_pins; }

    bool isDiffPair()
    {
        if (m_diff_pair.first != -1 && m_diff_pair.second != -1)
            return true;
        return false;
    }
    int getDiffPairId()
    {
        if (m_diff_pair.first == m_id)
            return m_diff_pair.second;
        else if (m_diff_pair.second == m_id)
            return m_diff_pair.first;
        return -1;
    }

    friend class kicadPcbDataBase;

private:
    int m_id;
    std::string m_name;
    int m_netclass_id;

    std::pair<int, int> m_diff_pair;
    std::vector<pin> m_pins;
};

#endif
