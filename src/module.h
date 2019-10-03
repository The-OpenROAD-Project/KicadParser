#ifndef KICAD_PCB_MODULE_H
#define KICAD_PCB_MODULE_H

#include "shape.h"
#include "pin.h"
#include <string>
#include <map>
#include <vector>
#include <optional>

class component
{
public:
    //ctor
    component(const int id = -1,
              const std::string name = "defaultName")
        : m_id(id), m_name(name) {}
    //dtor
    ~component() {}

    int getId() { return m_id; }
    std::string &getName() { return m_name; }
    std::vector<padstack> &getPadstacks() { return m_pads; }
    bool isPadstackId(const int id) { return id < m_pads.size() ? true : false; }

    padstack &getPadstack(const int id) { return m_pads.at(id); }
    bool getPadstackId(const std::string &name, int *id)
    {
        const auto padIte = m_pad_name_to_id.find(name);
        if (padIte != m_pad_name_to_id.end())
        {
            *id = padIte->second;
            return true;
        }
        else
        {
            *id = -1;
            return false;
        }
    }

    bool getPadstack(const std::string &name, padstack *&pad)
    {
        auto ite = m_pad_name_to_id.find(name);
        if (ite != m_pad_name_to_id.end())
        {
            pad = &(m_pads.at(ite->second));
            return true;
        }
        else
        {
            pad = nullptr;
            return false;
        }
    }
    bool getPadstack(const int id, padstack *&pad)
    {
        if (id < m_pads.size())
        {
            pad = &(m_pads.at(id));
            return true;
        }
        else
        {
            pad = nullptr;
            return false;
        }
    }

    friend class kicadPcbDataBase;

private:
    bool getPadstack(const std::string &name, padstack &pad)
    {
        auto ite = m_pad_name_to_id.find(name);
        if (ite != m_pad_name_to_id.end())
        {
            pad = m_pads.at(ite->second);
            return true;
        }
        else
        {
            return false;
        }
    }
    padstack &getPadstack(const std::string &name)
    {
        auto ite = m_pad_name_to_id.find(name);
        return m_pads.at(ite->second);
    }

private:
    int m_id;
    std::string m_name;
    std::map<std::string, int> m_pad_name_to_id; //<pad name, pad id>
    std::vector<padstack> m_pads;
    std::vector<line> m_lines;
    std::vector<circle> m_circles;
    std::vector<poly> m_polys;
    std::vector<arc> m_arcs;
};

class instance
{
public:
    //ctor
    instance() {}
    //dtor
    ~instance() {}

    const std::string &getName() const { return m_name; }
    int getComponentId() const { return m_comp_id; }
    int getId() const { return m_id; }
    double getAngle() const { return m_angle; }
    double getX() const { return m_x; }
    double getY() const { return m_y; }
    int getLayer() const { return m_layer; }

    friend class kicadPcbDataBase;

private:
    int m_id;
    std::string m_name;
    int m_comp_id;
    std::string m_side;
    double m_x;
    double m_y;
    double m_angle;
    double m_width;
    double m_height;
    int m_layer = -1;                         //Layer ID
    std::map<std::string, int> m_pin_net_map; //<pin name, netId>
};

#endif
