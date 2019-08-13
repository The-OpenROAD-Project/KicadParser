#ifndef KICADPCB_DATABASE_H
#define KICADPCB_DATABASE_H


#include "router.h"
#include "math.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <iterator>

struct tree
{
  std::string m_value;
  std::vector <tree> m_branches;
};

struct rule
{
  double m_radius;
  double m_gap;
};

struct padstack
{
  std::string m_name;
  std::string m_form;
  std::string m_type;
  double m_x;
  double m_y;
  double m_angle;
  int m_layer;
  point_2d m_size;
  // points_2d m_shape;
  rule m_rule;
};

struct line
{
  point_2d m_start;
  point_2d m_end;
  double m_width;
  int m_layer;
};

struct circle
{
  point_2d m_center;
  point_2d m_end;
  double m_width;
  int m_layer;
};

struct poly
{
  points_2d m_shape;
  double m_width;
  int m_layer;
};

struct arc
{
  point_2d m_start;
  point_2d m_end;
  double m_angle;
  double m_width;
  int m_layer;
};

struct component
{
  std::string m_name;
  std::map<std::string, padstack> m_pin_map;
  std::vector <line> m_lines;
  std::vector <circle> m_circles;
  std::vector <poly> m_polys;
  std::vector <arc> m_arcs;
  int m_layer;
};

struct instance
{
  std::string m_name;
  std::string m_comp;
  std::string m_side;
  double m_x;
  double m_y;
  double m_angle;
  double m_width;
  double m_height;
  std::map<std::string, int> m_pin_net_map;
};

struct circuit
{
  std::string m_via;
  rule m_rule;
};

class kicadPcbDataBase
{
  public:

    kicadPcbDataBase(std::string fileName):m_fileName(fileName){
      std::cout << "Build Kicad Pcb database..." << std::endl;
      if(!parseKicadPcb()) {
        std::cerr << "ERROR: Building Kicad Pcb database failed." << std::endl;
        assert(false);
      }
    };

    ~kicadPcbDataBase(){
      name_to_component_map.clear();
      layer_to_index_map.clear();
      name_to_instance_map.clear();
      index_to_net_map.clear();
      name_to_net_map.clear();
    };

      void printTree(const tree &, int);
      void printKicadPcb(const tree &, int);
      void printNet();
      void printInst();
      void printComp();

      tree readTree(std::istream &);

      bool parseKicadPcb();

      bool getNumOfInst(int *);
      bool getInst(int &, instance *);
      bool getPinPosition(std::string & inst_name, std::string & pin_name, point_2d *pos);
      bool getInstBBox(std::string &, point_2d *);
      

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

    std::map<std::string,component>::iterator comp_it;
    std::map<std::string, padstack>::iterator pad_it;
    std::map<std::string, instance>::iterator inst_it;
    std::map<std::string, net>::iterator net_it;
    std::map<std::string, int>::iterator pin_it;
    std::map<std::string, component> name_to_component_map;
    std::map<std::string, int> layer_to_index_map;
    std::map<std::string, instance> name_to_instance_map;
    std::map<int, std::string> index_to_net_map;
    std::map<std::string, net> name_to_net_map;
};


#endif
