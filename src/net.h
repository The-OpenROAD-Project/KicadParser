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
typedef std::vector<net> nets;

//net object
class net
{
public:
    net(const std::pair<int, int> pair = std::make_pair(-1, -1),
        const double clearance = 0.0,
        const double trace_width = 0.0,
        const double via_dia = 0.0,
        const double via_drill = 0.0,
        const double uvia_dia = 0.0,
        const double uvia_drill = 0.0)
        : m_diff_pair(pair), m_clearance(clearance), m_trace_width(trace_width), m_via_dia(via_dia), m_via_drill(via_drill), m_uvia_dia(uvia_dia), m_uvia_drill(uvia_drill) {}
    std::vector<pin> pins;
    double getClearance() { return m_clearance; }
    double getTraceWidth() { return m_trace_width; }
    double getViaDia() { return m_via_dia; }
    bool setId(int &id)
    {
        m_id = id;
        return true;
    }
    bool getId(int *id)
    {
        if (!id)
            return false;
        *id = m_id;
        return true;
    }
    int getId() { return m_id; }
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

private:
    int m_id;

    //double m_via;
    double m_clearance;
    double m_trace_width;
    double m_via_dia;
    double m_via_drill;
    double m_uvia_dia;
    double m_uvia_drill;
    std::pair<int, int> m_diff_pair;
};

#endif
