#ifndef KICAD_PCB_SEGMENT_H
#define KICAD_PCB_SEGMENT_H

#include <string>
#include <vector>


class Segment
{
    public:

        Segment(const int id = -1,
                const int netId = -1,
                const std::string layer = "",
                const double width = 0)
               : m_id(id), m_netId(netId), m_layer(layer), m_width(width) {}

        int getId() { return m_id;}
        int getNetId() { return m_netId;}
        double getWidth() { return m_width;}
        points_2d &getPos() { return m_pos;}
        std::string &getLayer() { return m_layer;}

    private:
        int m_id;
        int m_netId;
        double m_width;
        points_2d m_pos;
        std::string m_layer;
};

#endif
