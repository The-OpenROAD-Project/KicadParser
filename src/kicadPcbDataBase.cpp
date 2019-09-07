#include "kicadPcbDataBase.h"


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim))
  {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

auto shape_to_cords(const points_2d &shape, double a1, double a2)
{
  auto cords = points_2d{};
  auto rads = fmod((a1+a2)*-M_PI/180, 2*M_PI);
  auto s = sin(rads);
  auto c = cos(rads);
  for (auto &p : shape)
  {
    auto px = double(c*p.m_x - s*p.m_y);
    auto py = double(s*p.m_x + c*p.m_y);
    cords.push_back(point_2d{px, py});
  }
  return cords;
}

auto roundrect_to_cords( const point_2d &size, const double &ratio)
{
  auto cords = points_2d{};
  auto width = size.m_x/2;
  auto height = size.m_y/2;
  auto radius = std::min(width,height)*ratio*2;
  auto deltaWidth = width - radius;
  auto deltaHeight = height - radius; 
  auto point = point_2d{};                                
        for(int i = 0; i < 10; ++i) {        //10 points
          point.m_x = deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = -deltaHeight + radius * sin(-9*i*M_PI/180);
          //std::cout << point.m_x << " " << point.m_y << std::endl;
          cords.push_back(point);
        }
        for(int i = 10; i < 20; ++i) {       //10 points
          point.m_x = -deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = -deltaHeight + radius * sin(-9*i*M_PI/180);
          //std::cout << point.m_x << " " << point.m_y << std::endl;
          cords.push_back(point);
        }
        for(int i = 20; i < 30; ++i) {       //10 points
          point.m_x = -deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = deltaHeight + radius * sin(-9*i*M_PI/180);
          //std::cout << point.m_x << " " << point.m_y << std::endl;
          cords.push_back(point);
        }
        for(int i = 30; i < 40; ++i) {       //10 points
          point.m_x = deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = deltaHeight + radius * sin(-9*i*M_PI/180);
          //std::cout << point.m_x << " " << point.m_y << std::endl;
          cords.push_back(point);
        }
        point.m_x = deltaWidth + radius * cos(0*M_PI/180);
        point.m_y = -deltaHeight + radius * sin(0*M_PI/180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
  return cords;
}

auto shape_to_cords(const point_2d &size, point_2d &pos, padShape shape, double a1, double a2, const double &ratio)
{
  auto cords = points_2d{};
  switch (shape)
  {
    case padShape::CIRCLE:
      {
        auto radius = size.m_x/2; 
        auto point = point_2d{};
        for(int i = 0; i < 40; ++i) {       //40 points
          point.m_x = pos.m_x + radius * cos(-9*i*M_PI/180);
          point.m_y = pos.m_y + radius * sin(-9*i*M_PI/180);
          cords.push_back(point);
        }
        point.m_x = pos.m_x + radius * cos(0*M_PI/180);
        point.m_y = pos.m_y + radius * sin(0*M_PI/180);
        cords.push_back(point);
        break;
      }
    case padShape::OVAL:
      {
        auto width = size.m_x/2;
        auto height = size.m_y/2;
        auto point = point_2d{};
        for(int i = 0; i < 40; ++i) {       //40 points        
          point.m_x = pos.m_x + width * cos(-9*i*M_PI/180);
          point.m_y = pos.m_y + height * sin(-9*i*M_PI/180);
          cords.push_back(point);
        }
        point.m_x = pos.m_x + width * cos(0*M_PI/180);
        point.m_y = pos.m_y + height * sin(0*M_PI/180);
        cords.push_back(point);
        break;
      }
    case padShape::RECT:
      {
        auto width = size.m_x/2;
        auto height = size.m_y/2;
        cords.push_back(point_2d{-1*width, -1*height});
        cords.push_back(point_2d{width, height});
        break;
      }
    case padShape::ROUNDRECT:
      {
        auto width = size.m_x/2;
        auto height = size.m_y/2;
        auto radius = std::min(width,height)*ratio*2;
        auto deltaWidth = width - radius;
        auto deltaHeight = height - radius; 
        auto point = point_2d{};                                
        for(int i = 0; i < 10; ++i) {        //10 points
          point.m_x = pos.m_x + deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = pos.m_y - deltaHeight + radius * sin(-9*i*M_PI/180);
          cords.push_back(point);
        }
        for(int i = 10; i < 20; ++i) {       //10 points
          point.m_x = pos.m_x - deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = pos.m_y - deltaHeight + radius * sin(-9*i*M_PI/180);
          cords.push_back(point);
        }
        for(int i = 20; i < 30; ++i) {       //10 points
          point.m_x = pos.m_x - deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = pos.m_y + deltaHeight + radius * sin(-9*i*M_PI/180);
          cords.push_back(point);
        }
        for(int i = 30; i < 40; ++i) {       //10 points
          point.m_x = pos.m_x + deltaWidth + radius * cos(-9*i*M_PI/180);
          point.m_y = pos.m_y + deltaHeight + radius * sin(-9*i*M_PI/180);
          cords.push_back(point);
        }
        point.m_x = pos.m_x + deltaWidth + radius * cos(0*M_PI/180);
        point.m_y = pos.m_y - deltaHeight + radius * sin(0*M_PI/180);
        cords.push_back(point);
        break;
      }
    case padShape::TRAPEZOID:
      {
        break;
      } 
    default: 
      {
        break;
      }
  }

  return shape_to_cords(cords, a1, a2);
}

//read input till given byte appears
auto read_until(std::istream &in, char c)
{
  char input;
  while (in.get(input))
  {
    if (input == c) return false;
  }
  return true;
}

//read whitespace
auto read_whitespace(std::istream &in)
{
  for (;;)
  {
    auto b = in.peek();
    if (b != '\t' && b != '\n' && b != '\r' && b != ' ') break;
    char c;
    in.get(c);
  }
}

auto read_node_name(std::istream &in)
{
  std::string s;
  for (;;)
  {
    auto b = in.peek();
    if (b == '\t' || b == '\n' || b == '\r' || b == ' ' || b == ')') break;
    char c;
    in.get(c);
    s.push_back(c);
  }
  return s;
}

auto read_string(std::istream &in)
{
  std::string s;
  for (;;)
  {
    auto b = in.peek();
    if (b == '\t' || b == '\n' || b == '\r' || b == ')' || b == ' ') break;
    char c;
    in.get(c);
    s.push_back(c);
  }
  return tree{s, {}};
}

auto read_quoted_string(std::istream &in)
{
  std::string s;
  s.push_back('"');
  auto a = in.peek();
  for (;;)
  {
    auto b = in.peek();
    if (b == '"' && a != '\\') break;
    char c;
    in.get(c);
    s.push_back(c);
    a = b;
  }
  s.push_back('"');
  return tree{s, {}};
}

void ss_reset(std::stringstream &ss, const std::string &s)
{
  ss.str(s);
  ss.clear();
}

void get_value(std::stringstream &ss, std::vector<tree>::iterator t, int &x)
{
  ss_reset(ss, t->m_value);
  ss >> x;
}

void get_value(std::stringstream &ss, std::vector<tree>::iterator t, double &x)
{
  ss_reset(ss, t->m_value);
  ss >> x;
}

void get_2d(std::stringstream &ss, std::vector<tree>::iterator t, double &x, double &y)
{
  get_value(ss, t, x);
  get_value(ss, t + 1, y);
}

void get_rect(std::stringstream &ss, std::vector<tree>::iterator t, double &x1, double &y1, double &x2, double &y2)
{
  get_2d(ss, t, x1, y1);
  get_2d(ss, t + 2, x2, y2);
}
void kicadPcbDataBase::printKicadPcb(const tree &t, int indent)
{

  if (!t.m_value.empty())
  {
    for (auto i = 1; i < indent; ++i) std::cout << " "; 
    if (!t.m_branches.empty()) std::cout << "(";
    std::cout << t.m_value;
    //	if (t.m_branches.empty() && isYoungest) std::cout << ")";
  }
  //if (t.m_branches.empty()) std::cout << ")\n";
  //else 
  if (!t.m_branches.empty()) {
    for (auto &ct : t.m_branches) {
      printTree(ct, indent+1);
    }
    std::cout << ")" << std::endl;
  }
}


tree kicadPcbDataBase::readTree(std::istream &in)
{
  read_until(in, '(');
  read_whitespace(in);
  auto t = tree{read_node_name(in), {}};
  for (;;)
  {
    read_whitespace(in);
    auto b = in.peek();
    char c;
    if (b == EOF) {
      break;
    }

    if (b == ')')
    {
      in.get(c);
      break;
    }
    if (b == '(')
    {
      t.m_branches.push_back(readTree(in));
      continue;
    }
    if (b == '"')
    {
      in.get(c);
      t.m_branches.push_back(read_quoted_string(in));
      in.get(c);
      continue;
    }
    t.m_branches.push_back(read_string(in));
  }
  return t;
}

void kicadPcbDataBase::printTree(const tree &t, int indent)
{
  if (!t.m_value.empty())
  {
    for (auto i = 1; i < indent; ++i) std::cout << "  "; 
    std::cout << t.m_value << "\n";
  }
  for (auto &ct : t.m_branches) {
    printTree(ct, indent+1);
  }
}


bool kicadPcbDataBase::parseKicadPcb()
{

  std::ifstream arg_infile;
  arg_infile.open(m_fileName, std::ifstream::in);
  if (!arg_infile.is_open()) return false;
  std::istream &in = arg_infile;

  //create tree from input
  auto tree = readTree(in);
  //printTree(tree, 0);


  std::stringstream ss;
  auto default_rule = rule{0.25, 0.25};
  int noNameId = 0;
  for (auto &&sub_node : tree.m_branches) {
    //layer part
    if (sub_node.m_value == "layers") {

      for (auto &&layer_node : sub_node.m_branches)
      {
        auto layer_index = 0;
        auto layer_name = layer_node.m_branches[0].m_value;
        auto layer_type = layer_node.m_branches[1].m_value;
        if (layer_type == "signal") {
          layer_to_index_map[layer_name] = std::stoi(layer_node.m_value);
          index_to_layer_map[std::stoi(layer_node.m_value)] = layer_name;
        }
      }
    } 
    //net
    else if (sub_node.m_value == "net") {
      auto net_index = 0;
      auto net_name = sub_node.m_branches[1].m_value;
      get_value(ss, begin(sub_node.m_branches), net_index);
      index_to_net_map[net_index] = net_name;

      if (net_name[net_name.size()-1] == '+' || net_name[net_name.size()-1] == '-') {
        auto name = net_name.substr(0,net_name.size()-1);
        if (name_to_diff_pair_net_map.find(name) != name_to_diff_pair_net_map.end()) {
           auto &&pair = name_to_diff_pair_net_map[name];
           pair.second = net_index;
        } else {
          name_to_diff_pair_net_map[name] = std::make_pair(net_index, -1);
        }
      }
    }
    //net class
    else if (sub_node.m_value == "net_class") {
      for (auto &&net_class_node : sub_node.m_branches)
      {
        double m_clearance, m_trace_width, m_via_dia, m_via_drill, m_uvia_dia, m_uvia_drill;
        if (net_class_node.m_value == "clearance")
          get_value(ss, begin(net_class_node.m_branches), m_clearance);
        else if (net_class_node.m_value == "trace_width")
          get_value(ss, begin(net_class_node.m_branches), m_trace_width);
        else if (net_class_node.m_value == "via_dia")
          get_value(ss, begin(net_class_node.m_branches), m_via_dia);
        else if (net_class_node.m_value == "via_drill")
          get_value(ss, begin(net_class_node.m_branches), m_via_drill);
        else if (net_class_node.m_value == "uvia_dia")
          get_value(ss, begin(net_class_node.m_branches), m_uvia_dia);
        else if (net_class_node.m_value == "uvia_drill")
          get_value(ss, begin(net_class_node.m_branches), m_uvia_drill);
        else if (net_class_node.m_value == "add_net") {
          
          auto net_name = net_class_node.m_branches[0].m_value;
          
          auto name = net_name.substr(0,net_name.size()-1);
          auto pair = std::make_pair(-1, -1);
          if (name_to_diff_pair_net_map.find(name) != name_to_diff_pair_net_map.end()) {
            pair = name_to_diff_pair_net_map[name];
          } 
          auto the_net = net(pair, m_clearance, m_trace_width, m_via_dia, m_via_drill, m_uvia_dia, m_uvia_drill);
          name_to_net_map[net_name] = the_net;
        }
      }
    }
    else if (sub_node.m_value == "module") {
      std::string component_name;
      auto name = split(sub_node.m_branches[0].m_value, ':');
      if(name.size()==2) component_name = name[1]; 
      else component_name = name[0];
      

        auto layer = sub_node.m_branches[1].m_branches[0].m_value;
        auto the_instance = instance{};
        the_instance.m_comp = component_name;

        get_2d(ss, begin(sub_node.m_branches[4].m_branches), the_instance.m_x, the_instance.m_y);
        //if(component_name == "2X08") {
        //  std::cout << "HELLO" << the_instance.m_x << " " << the_instance.m_y << std::endl;
        //}
        if (int(sub_node.m_branches[4].m_branches.size()) == 3)
          get_value(ss,begin(sub_node.m_branches[4].m_branches)+2, the_instance.m_angle);
        else the_instance.m_angle = 0;

        comp_it = name_to_component_map.find(component_name);

        auto the_comp = component{component_name, std::map<std::string, padstack>{}};

        for (auto &&module_node : sub_node.m_branches) {

          if (module_node.m_value == "fp_text" && module_node.m_branches[0].m_value == "reference") {
            the_instance.m_name = module_node.m_branches[1].m_value;
          }
          if(comp_it == name_to_component_map.end()) { 
            if (module_node.m_value == "fp_line") {
              auto the_line = line{};
              get_2d(ss, begin(module_node.m_branches[0].m_branches), the_line.m_start.m_x, the_line.m_start.m_y);
              get_2d(ss, begin(module_node.m_branches[1].m_branches), the_line.m_end.m_x, the_line.m_end.m_y);
              get_value(ss, begin(module_node.m_branches[3].m_branches), the_line.m_width);
              the_comp.m_lines.push_back(the_line);
            }
            else if (module_node.m_value == "fp_poly") {
              auto the_poly = poly{};
              for (auto &&cor_node : module_node.m_branches[0].m_branches) {
                auto the_point = point_2d{};
                get_2d(ss, begin(cor_node.m_branches), the_point.m_x, the_point.m_y);
                the_poly.m_shape.push_back(the_point);
              }
              get_value(ss, begin(module_node.m_branches[2].m_branches), the_poly.m_width);
              the_comp.m_polys.push_back(the_poly);
            }
            else if (module_node.m_value == "fp_circle") {
              auto the_circle = circle{};
              get_2d(ss, begin(module_node.m_branches[0].m_branches), the_circle.m_center.m_x, the_circle.m_center.m_y);
              get_2d(ss, begin(module_node.m_branches[1].m_branches), the_circle.m_end.m_x, the_circle.m_end.m_y);
              get_value(ss, begin(module_node.m_branches[3].m_branches), the_circle.m_width);
              the_comp.m_circles.push_back(the_circle);
            }
            else if (module_node.m_value == "fp_arc") {
              auto the_arc = arc{};
              get_2d(ss, begin(module_node.m_branches[0].m_branches), the_arc.m_start.m_x, the_arc.m_start.m_y);
              get_2d(ss, begin(module_node.m_branches[1].m_branches), the_arc.m_end.m_x, the_arc.m_end.m_y);
              get_value(ss, begin(module_node.m_branches[2].m_branches), the_arc.m_angle);
              get_value(ss, begin(module_node.m_branches[4].m_branches), the_arc.m_width);
              the_comp.m_arcs.push_back(the_arc);
            }

            else if (module_node.m_value == "pad") {
              auto the_pin = padstack{};
              auto the_point = point_2d{};
              auto points = points_2d{};
					    the_pin.m_rule = default_rule;

              auto form = module_node.m_branches[2].m_value;
              auto type = module_node.m_branches[1].m_value;
              the_pin.m_name = module_node.m_branches[0].m_value;
              if(the_pin.m_name == "\"\"") {
                the_pin.m_name = "Unnamed" + std::to_string(noNameId);
                ++noNameId;
              }

              the_pin.setForm(form);
              the_pin.setType(type);

              get_2d(ss, begin(module_node.m_branches[3].m_branches), the_pin.m_pos.m_x, the_pin.m_pos.m_y);
              get_2d(ss, begin(module_node.m_branches[4].m_branches), the_point.m_x, the_point.m_y);
              the_pin.m_size = the_point;
              if ((int)module_node.m_branches[3].m_branches.size() == 3) {
                get_value(ss, begin(module_node.m_branches[3].m_branches)+2, the_pin.m_angle);
              }
              else the_pin.m_angle = 0;  
              the_pin.m_angle = the_pin.m_angle - the_instance.m_angle;

              if (type == "smd") {
                for (auto &&layer_node : module_node.m_branches[5].m_branches) {
                  if (layer_node.m_value == "Top" || layer_node.m_value == "Bottom") {
                    the_pin.m_layers.push_back(layer_node.m_value);
                  }
                }
              } else if (type == "thru_hole" || type == "np_thru_hole") {
                for (auto &&layer_node : module_node.m_branches[6].m_branches) {
                  if (layer_node.m_value == "*.Cu") {
                    for (auto &&layer : layer_to_index_map)
                      the_pin.m_layers.push_back(layer.first);
                  }
                }
              } else {
                for (auto &&layer_node : module_node.m_branches[5].m_branches) {
                  if (layer_to_index_map.find(layer_node.m_value)!=layer_to_index_map.end())
                    the_pin.m_layers.push_back(layer_node.m_value);
                }
              }

              if (form == "circle") {
                  the_pin.m_rule.m_radius = the_pin.m_size.m_x/2;
              } else if (form == "oval") {
                  the_pin.m_rule.m_radius = the_pin.m_size.m_x/4;
                  auto point1 = point_2d{the_pin.m_size.m_y/(-2),0};
                  auto point2 = point_2d{the_pin.m_size.m_y/2, 0};
                  the_pin.m_shape.push_back(point1);
                  the_pin.m_shape.push_back(point2);
              } else if (form == "rect") {
                  the_pin.m_rule.m_radius = 0.0;  
                  auto x1 = the_pin.m_size.m_x/(-2);
                  auto y1 = the_pin.m_size.m_y/2;
                  auto x2 = the_pin.m_size.m_x/2;
                  auto y2 = the_pin.m_size.m_y/(-2);
                  the_pin.m_shape.push_back(point_2d{x1, y1}); 
                  the_pin.m_shape.push_back(point_2d{x2, y1}); 
                  the_pin.m_shape.push_back(point_2d{x2, y2});
                  the_pin.m_shape.push_back(point_2d{x1, y2}); 
                  the_pin.m_shape.push_back(point_2d{x1, y1}); 
              } else if (form == "roundrect") {
                  the_pin.m_rule.m_radius = 0.0;
                  get_value(ss, begin(module_node.m_branches[6].m_branches), the_pin.m_roundrect_ratio);
                  the_pin.m_shape = roundrect_to_cords(the_pin.m_size, the_pin.m_roundrect_ratio);
                  /*
                  //TEST
                  for(auto &&cord : the_pin.m_shape) {
                    std::cout << "(" << cord.m_x << "," << cord.m_y << ") ";
                  }
                  std::cout << std::endl;*/
              }
              the_pin.m_rule.m_gap = 0; //TODO: double check
              the_comp.m_pin_map[the_pin.m_name] = the_pin;
            }
            name_to_component_map[component_name] = the_comp;
          }
        }

        //Find the connection of the pad
        for (auto &&pad_node : sub_node.m_branches)
        {
          if (pad_node.m_value == "pad") {
            auto pin_name = pad_node.m_branches[0].m_value;
            int net_index = 0;
            std::string net_name = "";
            for (auto &&net_node : pad_node.m_branches) {
              if (net_node.m_value == "net") {
                net_name = net_node.m_branches[1].m_value;
                get_value(ss, begin(net_node.m_branches), net_index);

                auto the_pin = pin{pin_name, component_name, the_instance.m_name};
                the_instance.m_pin_net_map[pin_name] = net_index;
                auto &&the_net = name_to_net_map[net_name];
                the_net.setId(net_index);
                the_net.pins.push_back(the_pin);

              }
            }
          }
        }
        name_to_instance_map[the_instance.m_name] = the_instance;
      
    }
    //TODO: calculate outline  
    else if (sub_node.m_value == "gr_line")
    {
    }

    else if (sub_node.m_value == "segment")
    {
      auto x = 0.0, y = 0.0, z = 0.0;
      auto width = 0.0;
      auto net = 0;
      auto segment = path{};

      z = layer_to_index_map[sub_node.m_branches[3].m_branches[0].m_value];
      get_2d(ss, begin(sub_node.m_branches[0].m_branches), x, y);
      segment.push_back(point_3d(x, y, z));
      get_2d(ss, begin(sub_node.m_branches[1].m_branches), x, y);
      segment.push_back(point_3d(x, y, z));
      get_value(ss, begin(sub_node.m_branches[2].m_branches), width);
      get_value(ss, begin(sub_node.m_branches[4].m_branches), net);
      net_to_segments_map[net].emplace_back(std::move(segment));
    }

    else if (sub_node.m_value == "via")
    {
        auto x = 0.0, y = 0.0;
        auto net = 0;
        get_2d(ss, begin(sub_node.m_branches[0].m_branches), x, y);
        get_value(ss, begin(sub_node.m_branches[4].m_branches), net);
        layer_it = layer_to_index_map.begin();
        auto z0 = layer_it->second;
        layer_it = layer_to_index_map.end();
        auto z1 = layer_it->second;
        net_to_segments_map[net].emplace_back(
            path{point_3d{x, y, double(z0)}, point_3d{x, y, double(z1)}});
    }

    else if (sub_node.m_value == "zone")
    {
      auto layer = sub_node.m_branches[2].m_branches[0].m_value;
      if (sub_node.m_branches[8].m_value == "keepout") {
        for (auto &&zone_node : sub_node.m_branches) {
          if (zone_node.m_value == "polygon") {
            auto p = path{};
            for (auto &&cord_node : zone_node.m_branches[0].m_branches) {
              if (cord_node.m_value == "xy") {
                auto point = point_3d{};
                get_2d(ss, begin(cord_node.m_branches), point.m_x, point.m_y);
                point.m_z = layer_to_index_map[layer];
                p.push_back(point);
              }
            }
            layer_to_keepout_map[layer].push_back(p);
          }
        }
      }
    }

  }


  auto minx = double(1000000.0);
	auto miny = double(1000000.0);
	auto maxx = double(-1000000.0);
	auto maxy = double(-1000000.0);

  //auto all_pads = pads{};
	for (auto &&inst : name_to_instance_map)
	{
		auto instance = inst.second;
		auto component = name_to_component_map[instance.m_comp];
		for (auto &&p : component.m_pin_map)
		{
			auto pin = p.second;
			auto m_x = pin.m_pos.m_x;
			auto m_y = pin.m_pos.m_y;
			auto s = sin(instance.m_angle*-M_PI/180);
			auto c = cos(instance.m_angle*-M_PI/180);
			auto px = double((c*m_x - s*m_y) + instance.m_x);
			auto py = double((s*m_x + c*m_y) + instance.m_y);
      auto layerId = 0.0;
      for(auto &&layer : pin.m_layers) {
        layerId = (double)layer_to_index_map[layer];
        auto tp = point_3d{px, py, layerId}; // x, y, layer
        auto cords = shape_to_cords(pin.m_shape,pin.m_angle, instance.m_angle);
        //shape_to_cords(pin.m_size, pin.m_pos, pin.m_form, pin.m_angle, instance.m_angle, pin.m_roundrect_ratio);
      /*
      //TEST
      if(instance.m_name == "IC6") {
      std::cout << "inst: " << instance.m_name << " comp: " << component.m_name << " pin: " << pin.m_name << " pin angle: " << pin.m_angle << " instance angle: " << instance.m_angle << std::endl;
      std::cout << "pin pos: (" << px << "," << py << ")" << pin.m_rule.m_radius << " " << pin.m_rule.m_gap << std::endl;
      std::cout << "\t\n";

        for (auto &&cord : cords) {
          std::cout << "(" << cord.m_x << "," << cord.m_y << ")";
        }
        std::cout << std::endl;
      }
      */
			  all_pads.push_back(pad{p.second.m_rule.m_radius, p.second.m_rule.m_gap, tp, cords, pin.m_size});
      
			  for (auto &&p1 : cords)
			  {
				  auto x = p1.m_x + px;
				  auto y = p1.m_y + py;
				  minx = std::min(x - p.second.m_rule.m_radius - p.second.m_rule.m_gap, minx);
				  maxx = std::max(x + p.second.m_rule.m_radius + p.second.m_rule.m_gap, maxx);
          miny = std::min(y - p.second.m_rule.m_radius - p.second.m_rule.m_gap, miny);
				  maxy = std::max(y + p.second.m_rule.m_radius + p.second.m_rule.m_gap, maxy);
			  }

				  minx = std::min(px - p.second.m_rule.m_radius - p.second.m_rule.m_gap, minx);
				  maxx = std::max(px + p.second.m_rule.m_radius + p.second.m_rule.m_gap, maxx);
          miny = std::min(py - p.second.m_rule.m_radius - p.second.m_rule.m_gap, miny);
				  maxy = std::max(py + p.second.m_rule.m_radius + p.second.m_rule.m_gap, maxy);

      }
		}
	}


//std::cout << "MINX: " << minx << " MAXX: " << maxx <<std::endl;
  auto track_id = 0;
  auto the_tracks = tracks{};
  for (net_it = name_to_net_map.begin(); net_it != name_to_net_map.end(); ++net_it) {
    
    auto net = net_it->second;
    //std::cout << "Net: " << net.getId() << std::endl;
    auto the_pads = pads{};
    for (size_t i = 0; i < net.pins.size(); ++i) {
      auto pin = net.pins[i];
      auto &&comp = name_to_component_map[pin.m_comp_name];
      auto &&p = comp.m_pin_map[pin.m_name];
      auto &&instance = name_to_instance_map[pin.m_instance_name];
      auto m_x = p.m_pos.m_x;
			auto m_y = p.m_pos.m_y;
								
      auto s = sin(instance.m_angle*-M_PI/180);
			auto c = cos(instance.m_angle*-M_PI/180);
			auto px = double((c*m_x - s*m_y) + instance.m_x);
			auto py = double((s*m_x + c*m_y) + instance.m_y);

      auto layerId = 0.0;
      for(auto &&layer : p.m_layers) {
        layerId =  (double)layer_to_index_map[layer];
        auto tp = point_3d{px, py, layerId}; // x, y, layer
        auto cords = shape_to_cords(p.m_shape, p.m_angle, instance.m_angle);
       // auto cords = shape_to_cords(p.m_size, p.m_pos, p.m_form, p.m_angle, instance.m_angle, p.m_roundrect_ratio);
        auto term = pad{p.m_rule.m_radius, p.m_rule.m_gap, tp, cords, p.m_size};
			  the_pads.push_back(term);

        //TODO: delete pin in all pin
        all_pads.erase(std::find(begin(all_pads), end(all_pads), term));
      }
    }
		the_tracks.push_back(track{std::to_string(track_id++), default_rule.m_radius, default_rule.m_radius, default_rule.m_gap,
												the_pads, net_to_segments_map[net.getId()]});
  }

/*for (auto &&pad : all_pads) {
  auto p = path{};
  auto point = point_3d{};
  for (auto &&cord : pad.m_shape) {
    point.m_x = pad.m_pos.m_x+cord.m_x;
    point.m_y = pad.m_pos.m_y+cord.m_y;
    point.m_z = pad.m_pos.m_z;
    p.push_back(point);
  }
auto layer_name = index_to_layer_map[point.m_z];
layer_to_keepout_map[layer_name].push_back(p);
}*/


  /*Test
  for (auto &&pad : all_pads) {
    std::cout << pad.m_pos.m_x << " " << pad.m_pos.m_y << " " << pad.m_pos.m_z << std::endl;
    for (auto &&cord : pad.m_shape)
      std::cout << "(" << cord.m_x << "," << cord.m_y << ") ";
    std::cout << std::endl;
  }*/

//print for GUI
	double border = 0.0;
  auto num_layers = (int)layer_to_index_map.size();
	std::cout << "(" << maxx - minx + border * 2
	 			<< " " << maxy - miny + border * 2
				<< " " << num_layers << ")\n";
	minx -= border;
	miny -= border;
	for (auto &&track : the_tracks)
	{
		std::cout << "(" << track.m_id << " " << track.m_track_radius << " "
						<< track.m_via_radius << " " << track.m_gap << " (";
		for (auto i = 0; i < static_cast<int>(track.m_pads.size()); ++i)
		{
			auto &&term = track.m_pads[i];
			std::cout << "(" << term.m_radius << " " << term.m_gap
				<< " (" << term.m_pos.m_x - minx
				<< " " << term.m_pos.m_y - miny
				<< " " << term.m_pos.m_z << ") (";
			for (auto j = 0; j < static_cast<int>(term.m_shape.size()); ++j)
			{
				auto cord = term.m_shape[j];
				std::cout << "(" << cord.m_x << " " << cord.m_y << ")";
			}
			std::cout << "))";
		}
		std::cout << ") (";
		for (auto i = 0; i < static_cast<int>(track.m_paths.size()); ++i)
		{
			std::cout << "(";
			auto &&p = track.m_paths.at(i);
			for (auto j = 0; j < static_cast<int>(p.size()); ++j)
			{
				std::cout << "(" << p[j].m_x - minx
					<< " " << p[j].m_y - miny
					<< " " << p[j].m_z << ")";
			}
			std::cout << ")";
		}
		std::cout << "))\n";
	}
  

  return true;
}

void kicadPcbDataBase::printSegment()
{
  for (auto &&s : net_to_segments_map) {
    std::cout << "net: " << s.first << std::endl;
    for (auto &&path : s.second) {
      for(auto &&point : path)
      std::cout << "(" << point.m_x << "," << point.m_y << "," << point.m_z << ")" << std::endl;
    }
  }
}

void kicadPcbDataBase::printComp() 
{

  std::cout << "###########COMP############" << std::endl;
  for (comp_it = name_to_component_map.begin(); comp_it != name_to_component_map.end(); ++comp_it) {
    std::cout << comp_it->first <<  " " << "====================== " << std::endl;
    auto comp = comp_it->second;
    for(size_t i = 0; i < comp.m_lines.size(); ++i) {
      std::cout << "\tstart: (" << comp.m_lines[i].m_start.m_x << "," << comp.m_lines[i].m_start.m_y << ")" << std::endl;
      std::cout << "\tend: (" << comp.m_lines[i].m_end.m_x << "," << comp.m_lines[i].m_end.m_y << ")" << std::endl;
      std::cout << "\twidth: " << comp.m_lines[i].m_width << std::endl;
    }
    for (pad_it = comp.m_pin_map.begin(); pad_it != comp.m_pin_map.end(); ++pad_it) {
      auto pad = pad_it->second;
      std::cout << "\tpad: " << pad.m_name << " (" << pad.m_pos.m_x << "," << pad.m_pos.m_y << ") " << pad.m_angle << std::endl;
      std::cout << "\t\tsize: " << (int)pad.m_form << " (" << pad.m_size.m_x << "," << pad.m_size.m_y << ")" << std::endl;
    }
  }
}

void kicadPcbDataBase::printInst()
{

  std::cout << "###########INST############" << std::endl;
  for (inst_it = name_to_instance_map.begin(); inst_it != name_to_instance_map.end(); ++inst_it) {
    auto inst = inst_it->second;
    std::cout << inst_it->first <<  " " << inst.m_comp << "====================== " << std::endl;
    for (pin_it = inst.m_pin_net_map.begin(); pin_it != inst.m_pin_net_map.end(); ++pin_it) {
      std::cout << "\tpin: " << pin_it->first << " net: " << pin_it->second << std::endl;
    }
  }
}


void kicadPcbDataBase::printNet()
{
  std::cout << "#################NET###############" << std::endl;
  for (net_it = name_to_net_map.begin(); net_it != name_to_net_map.end(); ++net_it) {
    auto net = net_it->second;
    std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
    for (size_t i = 0; i < net.pins.size(); ++i) {
      auto pin = net.pins[i];
      point_2d pos;
      getPinPosition(pin.m_instance_name, pin.m_name, &pos);
      auto &&comp = name_to_component_map[pin.m_comp_name];
      auto &&pad = comp.m_pin_map[pin.m_name];
      auto &&inst = name_to_instance_map[pin.m_instance_name];
      auto angle = inst.m_angle + pad.m_angle;
      std::cout << "\tinst name: " << pin.m_instance_name << " pin name: " << pin.m_name << " comp name: " << pin.m_comp_name << "pos: (" << pos.m_x << "," << pos.m_y << ")";
      if (angle == 0 || angle == 180)
        std::cout << "width: " << pad.m_size.m_x << "height: " << pad.m_size.m_y << std::endl;
      else 
        std::cout << "height: " << pad.m_size.m_x << "width: " << pad.m_size.m_y << std::endl;

    }
  }
}

/* print info for Dongwon*/
void kicadPcbDataBase::printFile() {

  std::cout << "#netId   instName   pinName   pinXPos   pinYPos   pinWidth   pinHeight   netType   diffPair   layerId   layerName" << std::endl;
  for (net_it = name_to_net_map.begin(); net_it != name_to_net_map.end(); ++net_it) {
    auto net = net_it->second;
    //std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
    for (auto &&pin : net.pins) {
      point_2d pos;
      getPinPosition(pin.m_instance_name, pin.m_name, &pos);
      auto &&comp = name_to_component_map[pin.m_comp_name];
      auto &&pad = comp.m_pin_map[pin.m_name];
      auto &&inst = name_to_instance_map[pin.m_instance_name];
      auto angle = inst.m_angle + pad.m_angle;
      std::cout << net.getId() << " " << pin.m_instance_name << " " << pin.m_name << " " << pos.m_x << " " << pos.m_y << " ";
      if (angle == 0 || angle == 180)
        std::cout << pad.m_size.m_x << " " << pad.m_size.m_y;
      else 
        std::cout << pad.m_size.m_x << " " << pad.m_size.m_y;
      
      if (net.isDiffPair()) std::cout << " DIFFPAIR " << net.getDiffPairId() << " ";
      else std::cout << " SIGNAL " << net.getDiffPairId() << " ";

      std::cout << "( ";
      for (auto &&layer : pad.m_layers) {
         std::cout << layer_to_index_map[layer] << " ";
      }
      std::cout << ") ";

      std::cout << "( ";
      for (auto &&layer : pad.m_layers) {
         std::cout << layer << " ";
      }
      std::cout << ")" << std::endl;
    }
  }
  std::cout << "\n#keepoutId   layerId   layerName   xPos   yPos" << std::endl;
  int i = 0;
  for (auto &&keepout : layer_to_keepout_map) {
    for(auto &&path : keepout.second) {
      for(auto &&cord : path) {
        std::cout << i << " " << layer_to_index_map[keepout.first] << " " << keepout.first;
        std::cout << " " << cord.m_x << " " << cord.m_y << std::endl;
        
      }
    ++i;
    }
  }

  std::cout << "#unusedPinId   pinXPos   pinYPos   pinWidth   pinHeight   layerId   layerName" << std::endl;

  i = 0;
  for(auto &&pin : all_pads) {
    std::cout << i << " " << pin.m_pos.m_x << " " << pin.m_pos.m_y << " " << pin.m_size.m_x << " " << pin.m_size.m_y;
    std::cout << " " << pin.m_pos.m_z << " " << index_to_layer_map[pin.m_pos.m_z] << std::endl;
    ++i;
  }

}

// Warning! Doesn't count component rotation
void kicadPcbDataBase::printPcbRouterInfo()
{
  std::cout << std::endl << "#################Routing Input###############" << std::endl;
  for (net_it = name_to_net_map.begin(); net_it != name_to_net_map.end(); ++net_it) {
    auto &&net = net_it->second;
    std::cout << net_it->first << ", clearance: " << net.getClearance() << ", size: " << net.pins.size() << ", netId: " << net.getId() << std::endl;
    for (size_t i = 0; i < net.pins.size(); ++i) {
      auto &&pin = net.pins[i];
      auto &&inst = name_to_instance_map[pin.m_instance_name];
      std::string compName = inst.m_comp;
      auto &&comp = name_to_component_map[compName];
      auto &&pad = comp.m_pin_map[pin.m_name];
      point_2d pinLocation;

      getPinPosition(pin.m_instance_name, pin.m_name, &pinLocation);

      std::cout << "\tinst name: " << pin.m_instance_name << ", " << inst.m_name << " (" << inst.m_x << " " << inst.m_y << ")" << ", Rot: " << inst.m_angle
                << " pin name: " << pin.m_name << " Relative:(" << pad.m_pos.m_x << " " << pad.m_pos.m_y << ")" << ", Rot: " << pad.m_angle << ", Absolute Pin Loc:(" << pinLocation.m_x << ", " << pinLocation.m_y << ")"
                << " comp name: " << pin.m_comp_name << ", " << comp.m_name << std::endl;

    }
  }
}

bool kicadPcbDataBase::getPcbRouterInfo(std::vector<std::set<std::pair<double, double>>> *routerInfo)
{
  int numNet = name_to_net_map.size();
  // netId = 0 is default 
  routerInfo->resize(numNet+1);
  //int netCounter = 0;

  for (net_it = name_to_net_map.begin(); net_it != name_to_net_map.end(); ++net_it) {
    auto &&net = net_it->second;
    assert(net.getId()<=numNet);

    //std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
    for (size_t i = 0; i < net.pins.size(); ++i) {
      auto &&pin = net.pins[i];
      /*
      auto &&inst = name_to_instance_map[pin.m_instance_name];
      std::string compName = inst.m_comp;
      auto &&comp = name_to_component_map[compName];
      auto &&pad = comp.m_pin_map[pin.m_name];


      //routerInfo->at(net.m_id).insert(std::pair<double, double>(inst.m_x+pad.m_x, inst.m_y+pad.m_y));
      //No rotation considered
      //routerInfo->at(netCounter).insert(std::pair<double, double>(inst.m_x+pad.m_x, inst.m_y+pad.m_y));
      */

      point_2d pinLocation;
      getPinPosition(pin.m_instance_name, pin.m_name, &pinLocation);
      //routerInfo->at(netCounter).insert(std::pair<double, double>(pinLocation.m_x, pinLocation.m_y));
      routerInfo->at(net.getId()).insert(std::pair<double, double>(pinLocation.m_x, pinLocation.m_y));

      //std::cout << "\tinst name: " << pin.m_instance_name << ", " << inst.m_name << " (" << inst.m_x << " " << inst.m_y << ")"
      //          << " pin name: " << pin.m_name << " Relative:(" << pad.m_x << " " << pad.m_y << ")"
      //          << " comp name: " << pin.m_comp_name << ", " << comp.m_name << std::endl;
    }

    //++netCounter;
  }
  return true;
}

bool kicadPcbDataBase::getNumOfInst(int* num)
{
  if(!num) return false;
  *num = (int)name_to_instance_map.size();
  return true;
}


bool kicadPcbDataBase::getInst(int &id, instance* inst)
{
  if(!inst) return false;
  inst_it = name_to_instance_map.begin();
  inst_it = next(inst_it, id);
  *inst = inst_it->second;
  return true;
}

//TODO: consider pin shape;
bool kicadPcbDataBase::getPinPosition(std::string & instName, std::string & pinName, point_2d *pos)
{
  if(!pos) return false;

  auto &&inst = name_to_instance_map[instName];
  std::string compName = inst.m_comp;
  auto &&comp = name_to_component_map[compName];
  auto &&pad = comp.m_pin_map[pinName];
  double padX = pad.m_pos.m_x, padY = pad.m_pos.m_y, padAngle = pad.m_angle;
  auto instAngle = inst.m_angle * (-M_PI / 180.0);

  auto s = sin((float)instAngle);
  auto c = cos((float)instAngle);
  pos->m_x = double((c*padX - s*padY) + inst.m_x);
  pos->m_y = double((s*padX + c*padY) + inst.m_y);

  return true;
}

bool kicadPcbDataBase::getInstBBox(std::string &instName, point_2d *bBox)
{
  if(!bBox) return false;
  auto &&inst = name_to_instance_map[instName];
  auto &&comp = name_to_component_map[inst.m_comp];
  auto angle = inst.m_angle;
  auto minx = double(1000000.0);
  auto miny = double(1000000.0);
  auto maxx = double(-1000000.0);
  auto maxy = double(-1000000.0);
  for (size_t i = 0; i < comp.m_lines.size(); ++i) {
    auto start = comp.m_lines[i].m_start;
    auto end = comp.m_lines[i].m_end;
    auto width = comp.m_lines[i].m_width/2;
    minx = std::min(start.m_x-width, minx);
    maxx = std::max(start.m_x+width, maxx);
    minx = std::min(end.m_x-width, minx);
    maxx = std::max(end.m_x+width, maxx);

    miny = std::min(start.m_y-width, miny);
    maxy = std::max(start.m_y+width, maxy);
    miny = std::min(end.m_y-width, miny);
    maxy = std::max(end.m_y+width, maxy);

  }

  for (size_t i = 0; i < comp.m_circles.size(); ++i) {
    auto center = comp.m_circles[i].m_center;
    auto end = comp.m_circles[i].m_end;
    auto width = comp.m_circles[i].m_width/2;
    minx = std::min(center.m_x-end.m_x-width, minx);
    maxx = std::max(center.m_x+width+end.m_x, maxx);

    miny = std::min(center.m_y-end.m_y-width, miny);
    maxy = std::max(center.m_y+end.m_y+width, maxy);
  }

  for (size_t i = 0; i < comp.m_polys.size(); ++i) {
    for (size_t j = 0; j < comp.m_polys[i].m_shape.size(); ++j) {
      auto point = comp.m_polys[i].m_shape[j];
      auto width = comp.m_polys[i].m_width/2;
      minx = std::min(point.m_x-width, minx);
      maxx = std::max(point.m_x+width, maxx);
      miny = std::min(point.m_y-width, miny);
      maxy = std::max(point.m_y+width, maxy);
    }
  }


  for (size_t i = 0; i < comp.m_arcs.size(); ++i) {
    auto start = comp.m_arcs[i].m_start;
    auto end = comp.m_arcs[i].m_end;
    auto width = comp.m_arcs[i].m_width/2;
    minx = std::min(start.m_x-width, minx);
    maxx = std::max(start.m_x+width, maxx);
    minx = std::min(end.m_x-width, minx);
    maxx = std::max(end.m_x+width, maxx);

    miny = std::min(start.m_y-width, miny);
    maxy = std::max(start.m_y+width, maxy);
    miny = std::min(end.m_y-width, miny);
    maxy = std::max(end.m_y+width, maxy);
  }

  for (pad_it = comp.m_pin_map.begin(); pad_it != comp.m_pin_map.end(); ++pad_it) {
    auto &&pad = pad_it->second;
    auto pad_x = pad.m_pos.m_x;
    auto pad_y = pad.m_pos.m_y;
    auto size = pad.m_size;
    auto pad_angle = pad.m_angle;

    if(pad_angle == 0 || pad_angle == 180) {
      minx = std::min(pad_x-size.m_x/2, minx);
      maxx = std::max(pad_x+size.m_x/2, maxx);

      miny = std::min(pad_y-size.m_y/2, miny);
      maxy = std::max(pad_y+size.m_y/2, maxy);
    }

    if(pad_angle == 90 || pad_angle == 270) {
      miny = std::min(pad_x-size.m_x/2, miny);
      maxy = std::max(pad_x+size.m_x/2, maxy);

      minx = std::min(pad_y-size.m_y/2, minx);
      maxx = std::max(pad_y+size.m_y/2, maxx);
    }
  }

  double width = abs(maxx-minx);
  double height = abs(maxy-miny);
  if(angle == 90 || angle == 270) {
    bBox->m_x = height;
    bBox->m_y = width;
  } else {
    bBox->m_x = width;
    bBox->m_y = height;
  }

  return true;
}

bool kicadPcbDataBase::getPad(std::string &compName, std::string &padName, padstack * pad)
{
  if(!pad) return false;
  auto &&comp = name_to_component_map[compName];
  *pad = comp.m_pin_map[padName];
  return true;
}

