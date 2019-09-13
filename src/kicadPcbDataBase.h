#ifndef KICADPCB_DATABASE_H
#define KICADPCB_DATABASE_H

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

    /*
           getPinPosition(std::string inst_name, std::string pin_name, point_2d *pos);
           getNet
           getPin
           getComponent
           getInstance
           getInstanceBBox
           */

private:
    std::string m_fileName;

    std::map<std::string, component>::iterator comp_it;
    std::map<std::string, padstack>::iterator pad_it;
    std::map<std::string, instance>::iterator inst_it;
    std::map<std::string, net>::iterator net_it;
    std::map<std::string, int>::iterator pin_it, layer_it;
    std::map<int, paths> net_to_segments_map;
    std::map<std::string, component> name_to_component_map;
    std::map<std::string, int> layer_to_index_map;
    std::map<int, std::string> index_to_layer_map;
    std::map<std::string, paths> layer_to_keepout_map;
    std::map<std::string, instance> name_to_instance_map;
    std::map<int, std::string> index_to_net_map;
    std::map<std::string, net> name_to_net_map;
    std::map<std::string, std::pair<int, int>> name_to_diff_pair_net_map;
    std::vector<pad> all_pads;
    paths all_keepouts;

    tree tree;
};

#endif
