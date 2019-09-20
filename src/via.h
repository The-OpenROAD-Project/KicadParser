#ifndef KICAD_PCB_SEGMENT_H
#define KICAD_PCB_SEGMENT_H

#include <string>
#include <vector>

class Via
{
    public:

        Via(const int id = -1,
                const int netId = -1,
                const double size = 0)
               : m_id(id), m_netId(netId), m_size(size) {}

        int getId() { return m_id;}
        int getNetId() { return m_netId;}
        double getSize() { return m_size;}
        point_2d &getPos() { return m_pos;}
        std::vector<std::string> &getLayer() { return m_layers;}

        
    private:
        int m_id;
        int m_netId;
        double m_size;
        std::vector<std:string> m_layers;
        point_2d m_pos;
};



#endif

