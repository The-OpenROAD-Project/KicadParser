#ifndef KICAD_PCB_VIA_H
#define KICAD_PCB_VIA_H

#include <string>
#include <vector>
#include "mymath.h"

class Via
{
public:
    Via(const int id = -1,
        const int netId = -1,
        const double size = 0)
        : m_id(id), m_netId(netId), m_size(size) {}

    int getId() { return m_id; }
    int getNetId() { return m_netId; }
    double getSize() { return m_size; }
    point_2d &getPos() { return m_pos; }
    std::vector<std::string> &getLayer() { return m_layers; }
    void setPosition(const point_2d &_pos) { m_pos = std::move(_pos); }
    void setLayer(const std::vector<std::string> &_layers) { m_layers = std::move(_layers); }

private:
    int m_id;
    int m_netId;
    double m_size;
    std::vector<std::string> m_layers;
    point_2d m_pos;
};

#endif
