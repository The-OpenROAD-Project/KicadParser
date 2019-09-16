#ifndef KICAD_PCB_KICADPCB_DATABASE_H
#define KICAD_PCB_KICADPCB_DATABASE_H

#include "net.h"
#include "kicadParser.h"
#include "tree.h"
#include "util.h"
#include "module.h"
#include "pin.h"
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <iterator>
#include <unordered_map>

class kicadPcbDataBase
{
public:
    kicadPcbDataBase(std::string fileName) : m_fileName(fileName)
    {
        std::cerr << "Build Kicad Pcb database..." << std::endl;
        if (!buildKicadPcb())
        {
            std::cerr << "ERROR: Building Kicad Pcb database failed." << std::endl;
            assert(false);
        }
    };

    ~kicadPcbDataBase()
    {
        name_to_component_map.clear();
        layer_to_index_map.clear();
        name_to_instance_map.clear();
        index_to_net_map.clear();
        name_to_net_map.clear();
    };

    void printNet();
    void printInst();
    void printComp();
    void printPcbRouterInfo();
    void printFile();
    void printSegment();

    bool buildKicadPcb();

    bool getPcbRouterInfo(std::vector<std::set<std::pair<double, double>>> *);
    bool getNumOfInst(int *);
    bool getInst(int &, instance *);
    bool getPinPosition(std::string &inst_name, std::string &pin_name, point_2d *pos);
    bool getInstBBox(std::string &, point_2d *);
    bool getPad(std::string &, std::string &, padstack *);

    std::string getFileName() { return m_fileName; }

    //TODO
    //getPinPosition(std::string inst_name, std::string pin_name, point_2d *pos);
    //getNet
    //getPin
    //getComponent

private:
    // Input
    std::string m_fileName;

    // Iterator
    std::unordered_map<std::string, component>::iterator comp_it;
    std::map<std::string, padstack>::iterator pad_it;
    std::unordered_map<std::string, instance>::iterator inst_it;
    std::unordered_map<std::string, net>::iterator net_it;
    std::map<std::string, int>::iterator pin_it, layer_it;

    // Index map
    std::map<std::string, int> layer_to_index_map;
    std::map<int, std::string> index_to_layer_map;
    std::map<int, std::string> index_to_net_map;

    // TODO: netclass objects
    // Object Instances
    std::unordered_map<std::string, instance> name_to_instance_map;   //<instance name, instance instance>
    std::unordered_map<std::string, component> name_to_component_map; //<component name, component instance>
    std::unordered_map<std::string, net> name_to_net_map;             //<net name, net instance>

    // Keepouts
    std::map<std::string, paths> layer_to_keepout_map; // keepout zones <layer name, polygon>
    paths all_keepouts;                                // All keepout zones in polygon

    // TODO: Move to Net Instance and Consider the usage of DRC checking
    std::map<int, paths> net_to_segments_map;

    // TODO: Refactor this
    // Reorganized for router
    std::vector<pad> all_pads; // unconnected pins
    std::vector<track> the_tracks;

    // For differential pair lookup
    std::map<std::string, std::pair<int, int>> name_to_diff_pair_net_map; // <net name, <netId1, netId2>>

    // Misc.
    tree tree;
};

#endif
