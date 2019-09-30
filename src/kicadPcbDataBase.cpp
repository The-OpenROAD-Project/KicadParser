#include "kicadPcbDataBase.h"

bool kicadPcbDataBase::buildKicadPcb()
{
    //Basic file extension checking
    std::string fileExt = utilParser::getFileExtension(m_fileName);
    if (fileExt != "kicad_pcb")
    {
        std::cerr << "Support .kicad_pcb file only. Current input file: " << m_fileName << std::endl;
        return false;
    }

    kicadParser parser(m_fileName);
    if (!parser.parseKicadPcb(&tree)) return false;

    std::stringstream ss;
    auto default_rule = rule{0.25, 0.25};
    for (auto &&sub_node : tree.m_branches)
    {
        //layer part
        if (sub_node.m_value == "layers")
        {

            for (auto &&layer_node : sub_node.m_branches)
            {
                //auto layer_index = 0;
                auto layer_name = layer_node.m_branches[0].m_value;
                auto layer_type = layer_node.m_branches[1].m_value;
                //Use "signal" to decide
                if (layer_type == "signal")
                {
                    layer_to_index_map[layer_name] = std::stoi(layer_node.m_value);
                    index_to_layer_map[std::stoi(layer_node.m_value)] = layer_name;
                }
            }
        }
        //net
        else if (sub_node.m_value == "net")
        {
            auto net_index = 0;
            auto net_name = sub_node.m_branches[1].m_value;
            get_value(ss, begin(sub_node.m_branches), net_index);
            net_id_to_name[net_index] = net_name;
            net_name_to_id[net_name] = net_index;

            // Identify differential pairs
            if (net_name.back() == '+' || net_name.back() == '-')
            {
                auto name = net_name.substr(0, net_name.size() - 1);
                if (name_to_diff_pair_net_map.find(name) != name_to_diff_pair_net_map.end())
                {
                    auto &&pair = name_to_diff_pair_net_map[name];
                    pair.second = net_index;
                }
                else
                {
                    name_to_diff_pair_net_map[name] = std::make_pair(net_index, -1);
                }
            }
        }
        //net class (Create Net Instances)
        else if (sub_node.m_value == "net_class")
        {
            nets.resize(net_id_to_name.size());
            int m_netclass_id = (int)netclasses.size();
            auto m_netclass_name = sub_node.m_branches.front().m_value;
            double m_clearance = 0.0, m_trace_width = 0.0, m_via_dia = 0.0, m_via_drill = 0.0, m_uvia_dia = 0.0, m_uvia_drill = 0.0;

            for (auto &&net_class_node : sub_node.m_branches)
            {
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
                else if (net_class_node.m_value == "add_net")
                {
                    auto net_name = net_class_node.m_branches[0].m_value;
                    auto net_id = -1;
                    auto name = net_name.substr(0, net_name.size() - 1);
                    auto pair = std::make_pair(-1, -1);
                    auto netIte = net_name_to_id.find(net_name);
                    if (netIte != net_name_to_id.end())
                    {
                        net_id = netIte->second;
                        assert(net_id < (int)nets.size());
                    }
                    else
                    {
                        std::cerr << "Undefined net name \"" << net_name << "\" added in the netclass." << std::endl;
                    }
                    if (name_to_diff_pair_net_map.find(name) != name_to_diff_pair_net_map.end())
                    {
                        pair = name_to_diff_pair_net_map[name];
                    }
                    // Create the Net instance
                    nets.at(net_id) = net(net_id, net_name, m_netclass_id, pair);
                }
            }
            netclass the_netclass{m_netclass_id, m_netclass_name, m_clearance, m_trace_width, m_via_dia, m_via_drill, m_uvia_dia, m_uvia_drill};
            netclasses.push_back(the_netclass);
        }
        // Create Module Instances
        else if (sub_node.m_value == "module")
        {
            // Get Component Name
            std::string component_name;
            auto name = split(sub_node.m_branches[0].m_value, ':');
            if (name.size() == 2)
                component_name = name[1];
            else
                component_name = name[0];

            // Check if inst locked
            //TODO: refactor
            instance the_instance{};
            std::string layer = "";
            if(sub_node.m_branches[1].m_value == "locked") {
                layer = sub_node.m_branches[2].m_branches[0].m_value;
                
                the_instance.m_id = (int)instances.size();
                get_2d(ss, begin(sub_node.m_branches[5].m_branches), the_instance.m_x, the_instance.m_y);
                if (int(sub_node.m_branches[5].m_branches.size()) == 3)
                    get_value(ss, begin(sub_node.m_branches[5].m_branches) + 2, the_instance.m_angle);
                else
                    the_instance.m_angle = 0;
            }
            else {
                // Get Instance X, Y, Rot
                layer = sub_node.m_branches[1].m_branches[0].m_value;
                the_instance.m_id = (int)instances.size();
                get_2d(ss, begin(sub_node.m_branches[4].m_branches), the_instance.m_x, the_instance.m_y);
                if (int(sub_node.m_branches[4].m_branches.size()) == 3)
                    get_value(ss, begin(sub_node.m_branches[4].m_branches) + 2, the_instance.m_angle);
                else
                    the_instance.m_angle = 0;
            }
            // See if the component is created
            auto comp_it = component_name_to_id.find(component_name);
            int comp_id = -1;
            if (comp_it == component_name_to_id.end())
            {
                comp_id = (int)components.size();
            }
            else
            {
                comp_id = comp_it->second;
            }
            component the_comp{comp_id, component_name};
            int noNameId = 0;
            for (auto &&module_node : sub_node.m_branches)
            {
                //TODO:: Handle (fp_text value ...)
                if (module_node.m_value == "fp_text" && module_node.m_branches[0].m_value == "reference")
                {
                    the_instance.m_name = module_node.m_branches[1].m_value;
                }
                // Create Component if needed
                if (comp_it == component_name_to_id.end())
                {
                    if (module_node.m_value == "fp_line")
                    {
                        auto the_line = line{};
                        get_2d(ss, begin(module_node.m_branches[0].m_branches), the_line.m_start.m_x, the_line.m_start.m_y);
                        get_2d(ss, begin(module_node.m_branches[1].m_branches), the_line.m_end.m_x, the_line.m_end.m_y);
                        get_value(ss, begin(module_node.m_branches[3].m_branches), the_line.m_width);
                        the_comp.m_lines.push_back(the_line);
                    }
                    else if (module_node.m_value == "fp_poly")
                    {
                        auto the_poly = poly{};
                        for (auto &&cor_node : module_node.m_branches[0].m_branches)
                        {
                            auto the_point = point_2d{};
                            get_2d(ss, begin(cor_node.m_branches), the_point.m_x, the_point.m_y);
                            the_poly.m_shape.push_back(the_point);
                        }
                        get_value(ss, begin(module_node.m_branches[2].m_branches), the_poly.m_width);
                        the_comp.m_polys.push_back(the_poly);
                    }
                    else if (module_node.m_value == "fp_circle")
                    {
                        auto the_circle = circle{};
                        get_2d(ss, begin(module_node.m_branches[0].m_branches), the_circle.m_center.m_x, the_circle.m_center.m_y);
                        get_2d(ss, begin(module_node.m_branches[1].m_branches), the_circle.m_end.m_x, the_circle.m_end.m_y);
                        get_value(ss, begin(module_node.m_branches[3].m_branches), the_circle.m_width);
                        the_comp.m_circles.push_back(the_circle);
                    }
                    else if (module_node.m_value == "fp_arc")
                    {
                        auto the_arc = arc{};
                        get_2d(ss, begin(module_node.m_branches[0].m_branches), the_arc.m_start.m_x, the_arc.m_start.m_y);
                        get_2d(ss, begin(module_node.m_branches[1].m_branches), the_arc.m_end.m_x, the_arc.m_end.m_y);
                        get_value(ss, begin(module_node.m_branches[2].m_branches), the_arc.m_angle);
                        get_value(ss, begin(module_node.m_branches[4].m_branches), the_arc.m_width);
                        the_comp.m_arcs.push_back(the_arc);
                    }
                    else if (module_node.m_value == "pad")
                    {
                        auto the_pin = padstack{};
                        auto the_point = point_2d{};
                        auto points = points_2d{};
                        the_pin.m_id = the_comp.m_pads.size();
                        the_pin.m_rule = default_rule;

                        auto form = module_node.m_branches[2].m_value;
                        auto type = module_node.m_branches[1].m_value;
                        the_pin.m_name = module_node.m_branches[0].m_value;
                        if (the_pin.m_name == "\"\"")
                        {
                            the_pin.m_name = "Unnamed" + std::to_string(noNameId);
                            ++noNameId;
                        }

                        the_pin.setShape(form);
                        the_pin.setType(type);

                        get_2d(ss, begin(module_node.m_branches[3].m_branches), the_pin.m_pos.m_x, the_pin.m_pos.m_y);
                        get_2d(ss, begin(module_node.m_branches[4].m_branches), the_point.m_x, the_point.m_y);
                        the_pin.m_size = the_point;
                        if ((int)module_node.m_branches[3].m_branches.size() == 3)
                        {
                            get_value(ss, begin(module_node.m_branches[3].m_branches) + 2, the_pin.m_angle);
                        }
                        else
                        {
                            the_pin.m_angle = 0;
                        }
                        the_pin.m_angle = the_pin.m_angle - the_instance.m_angle;

                        if (type == "smd")
                        {
                            for (auto &&layer_node : module_node.m_branches[5].m_branches)
                            {
                                if (layer_node.m_value == "Top" || layer_node.m_value == "Bottom")
                                {
                                    the_pin.m_layers.push_back(layer_node.m_value);
                                }
                            }
                        }
                        else if (type == "thru_hole" || type == "np_thru_hole")
                        {
                            for (auto &&layer_node : module_node.m_branches[6].m_branches)
                            {
                                if (layer_node.m_value == "*.Cu")
                                {
                                    for (auto &layer : layer_to_index_map)
                                        the_pin.m_layers.push_back(layer.first);
                                }
                            }
                        }
                        else
                        {
                            for (auto &&layer_node : module_node.m_branches[5].m_branches)
                            {
                                if (layer_to_index_map.find(layer_node.m_value) != layer_to_index_map.end())
                                    the_pin.m_layers.push_back(layer_node.m_value);
                            }
                        }

                        if (form == "circle")
                        {
                            the_pin.m_rule.m_radius = the_pin.m_size.m_x / 2;
                        }
                        else if (form == "oval")
                        {
                            the_pin.m_rule.m_radius = the_pin.m_size.m_x / 4;
                            auto point1 = point_2d{the_pin.m_size.m_y / (-2), 0};
                            auto point2 = point_2d{the_pin.m_size.m_y / 2, 0};
                            the_pin.m_shape_coords.push_back(point1);
                            the_pin.m_shape_coords.push_back(point2);
                        }
                        else if (form == "rect")
                        {
                            the_pin.m_rule.m_radius = 0.0;
                            auto x1 = the_pin.m_size.m_x / (-2);
                            auto y1 = the_pin.m_size.m_y / 2;
                            auto x2 = the_pin.m_size.m_x / 2;
                            auto y2 = the_pin.m_size.m_y / (-2);
                            the_pin.m_shape_coords.push_back(point_2d{x1, y1});
                            the_pin.m_shape_coords.push_back(point_2d{x2, y1});
                            the_pin.m_shape_coords.push_back(point_2d{x2, y2});
                            the_pin.m_shape_coords.push_back(point_2d{x1, y2});
                            the_pin.m_shape_coords.push_back(point_2d{x1, y1});
                        }
                        else if (form == "roundrect")
                        {
                            the_pin.m_rule.m_radius = 0.0;
                            get_value(ss, begin(module_node.m_branches[6].m_branches), the_pin.m_roundrect_ratio);
                            the_pin.m_shape_coords = roundrect_to_shape_coords(the_pin.m_size, the_pin.m_roundrect_ratio);
                            /*
                            //TEST
                            for(auto &&cord : the_pin.m_shape) {
                            std::cout << "(" << cord.m_x << "," << cord.m_y << ") ";
                            }
                            std::cout << std::endl;*/
                        }
                        the_pin.m_rule.m_clearance = 0; //TODO: double check
                        the_comp.m_pads.push_back(the_pin);
                        the_comp.m_pad_name_to_id[the_pin.m_name] = the_pin.m_id;
                    }
                }
            }

            if (comp_it == component_name_to_id.end())
            {
                components.push_back(the_comp);
                component_name_to_id[component_name] = comp_id;
            }

            //Find the connection of the pad
            noNameId = 0;
            for (auto &&pad_node : sub_node.m_branches)
            {
                if (pad_node.m_value == "pad")
                {
                    auto pin_name = pad_node.m_branches[0].m_value;
                    if (pin_name == "\"\"")
                    {
                        pin_name = "Unnamed" + std::to_string(noNameId);
                        ++noNameId;
                    }
                    int net_index = 0;
                    std::string net_name = "";
                    bool connected = false;
                    for (auto &&net_node : pad_node.m_branches)
                    {
                        if (net_node.m_value == "net")
                        {
                            connected = true;
                            net_name = net_node.m_branches[1].m_value;
                            get_value(ss, begin(net_node.m_branches), net_index);
                            break;
                        }
                    }

                    if (!isComponentId(comp_id))
                    {
                        std::cerr << __FUNCTION__ << "() ilegal component id: " << comp_id << std::endl;
                        continue;
                    }

                    auto &comp = getComponent(comp_id);
                    int padstack_id = -1;
                    if (!comp.getPadstackId(pin_name, &padstack_id))
                    {
                        std::cerr << __FUNCTION__ << "() ilegal pin name: " << pin_name << std::endl;
                        continue;
                    }

                    auto the_pin = pin{padstack_id, comp_id, the_instance.m_id};

                    if (connected == true)
                    {
                        the_instance.m_pin_net_map[pin_name] = net_index;
                        auto &the_net = getNet(net_name);
                        the_net.addPin(the_pin);
                    }
                    else
                    {
                        the_instance.m_pin_net_map[pin_name] = -1;
                        unconnectedPins.push_back(the_pin);
                    }
                }
            }
            the_instance.m_comp_id = comp_id;
            instance_name_to_id[the_instance.m_name] = the_instance.m_id;
            instances.push_back(the_instance);
        }
        //TODO: calculate outline
        else if (sub_node.m_value == "gr_line")
        {
        }
        // TODO: belongs to Net Instance
        else if (sub_node.m_value == "segment")
        {
            /*
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
            */

            //put segment into net

            int netId;
            double width;
            auto p = point_2d{};
            auto points = points_2d{};
            auto layer = sub_node.m_branches[3].m_branches[0].m_value;
            get_value(ss, begin(sub_node.m_branches[2].m_branches), width);
            get_2d(ss, begin(sub_node.m_branches[0].m_branches), p.m_x, p.m_y);
            points.push_back(p);
            get_2d(ss, begin(sub_node.m_branches[1].m_branches), p.m_x, p.m_y);
            points.push_back(p);
            get_value(ss, begin(sub_node.m_branches[4].m_branches), netId);
            auto &net = nets[netId];
            auto id = net.getSegmentCount();
            Segment s(id, netId, width, layer);
            s.setPosition(points);
            net.addSegment(s);
        }

        // TODO: belongs to Net Instance
        else if (sub_node.m_value == "via")
        {
            /*
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
                */

            int netId;
            double size;
            auto p = point_2d{};
            std::vector<std::string> layers;
            layers.resize(2);
            for (auto &&via_node : sub_node.m_branches) {
                if(via_node.m_value == "at") {
                    get_2d(ss, begin(via_node.m_branches), p.m_x, p.m_y);
                }
                else if(via_node.m_value == "size") {
                    get_value(ss, begin(via_node.m_branches), size);
                }
                else if(via_node.m_value == "net") {
                    get_value(ss, begin(via_node.m_branches), netId);
                }
                else if(via_node.m_value == "layer") {
                    layers[0] = via_node.m_branches[0].m_value;
                    layers[1] = via_node.m_branches[1].m_value;
                }
            }

            auto &net = nets[netId];
            auto id = net.getViaCount();
            Via via(id, netId, size);
            via.setPosition(p);
            via.setLayer(layers);
            net.addVia(via);
        }

        //TODO: Copper Pours
        else if (sub_node.m_value == "zone")
        {
            auto layer = sub_node.m_branches[2].m_branches[0].m_value;
            if (sub_node.m_branches[8].m_value == "keepout")
            {
                for (auto &&zone_node : sub_node.m_branches)
                {
                    if (zone_node.m_value == "polygon")
                    {
                        auto p = path{};
                        for (auto &&cord_node : zone_node.m_branches[0].m_branches)
                        {
                            if (cord_node.m_value == "xy")
                            {
                                auto point = point_3d{};
                                get_2d(ss, begin(cord_node.m_branches), point.m_x, point.m_y);
                                point.m_z = layer_to_index_map[layer];
                                p.push_back(point);
                            }
                        }
                        all_keepouts.push_back(p);
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

    for (auto &inst : instances)
    {
        auto comp = components.at(inst.getComponentId());
        for (auto &padstack : comp.getPadstacks())
        {
            auto m_x = padstack.m_pos.m_x;
            auto m_y = padstack.m_pos.m_y;
            auto s = sin(inst.m_angle * -M_PI / 180);
            auto c = cos(inst.m_angle * -M_PI / 180);
            auto px = double((c * m_x - s * m_y) + inst.m_x);
            auto py = double((s * m_x + c * m_y) + inst.m_y);
            auto layerId = 0.0;
            for (auto &&layer : padstack.m_layers)
            {
                layerId = (double)layer_to_index_map[layer];
                auto tp = point_3d{px, py, layerId}; // x, y, layer

                // TODO: doesn't count CIRCLE
                auto cords = rotateShapeCoordsByAngles(padstack.m_shape_coords, padstack.m_angle, inst.m_angle);
                //shape_to_cords(pin.m_size, pin.m_pos, pin.m_form, pin.m_angle, instance.m_angle, pin.m_roundrect_ratio);

                // //TEST
                // if(instance.m_name == "IC6") {
                // std::cout << "inst: " << instance.m_name << " comp: " << component.m_name << " pin: " << pin.m_name << " pin angle: " << pin.m_angle << " instance angle: " << instance.m_angle << std::endl;
                // std::cout << "pin pos: (" << px << "," << py << ")" << pin.m_rule.m_radius << " " << pin.m_rule.m_clearance << std::endl;
                // std::cout << "\t\n";

                // for (auto &&cord : cords) {
                // std::cout << "(" << cord.m_x << "," << cord.m_y << ")";
                // }
                // std::cout << std::endl;
                // }

                all_pads.push_back(pad{padstack.m_rule.m_radius, padstack.m_rule.m_clearance, tp, cords, padstack.m_size});

                for (auto &&p1 : cords)
                {
                    auto x = p1.m_x + px;
                    auto y = p1.m_y + py;
                    minx = std::min(x - padstack.m_rule.m_radius - padstack.m_rule.m_clearance, minx);
                    maxx = std::max(x + padstack.m_rule.m_radius + padstack.m_rule.m_clearance, maxx);
                    miny = std::min(y - padstack.m_rule.m_radius - padstack.m_rule.m_clearance, miny);
                    maxy = std::max(y + padstack.m_rule.m_radius + padstack.m_rule.m_clearance, maxy);
                }

                minx = std::min(px - padstack.m_rule.m_radius - padstack.m_rule.m_clearance, minx);
                maxx = std::max(px + padstack.m_rule.m_radius + padstack.m_rule.m_clearance, maxx);
                miny = std::min(py - padstack.m_rule.m_radius - padstack.m_rule.m_clearance, miny);
                maxy = std::max(py + padstack.m_rule.m_radius + padstack.m_rule.m_clearance, maxy);
            }
        }
    }

    //std::cout << "MINX: " << minx << " MAXX: " << maxx << std::endl;
    //std::cout << "MINY: " << miny << " MAXY: " << maxy << std::endl;
    m_boundary.push_back(point_2d{minx,miny});
    m_boundary.push_back(point_2d{maxx,maxy});

    /*
    auto track_id = 0;
    //auto the_tracks = tracks{};
    //for (net_it = name_to_net_map.begin(); net_it != name_to_net_map.end(); ++net_it)
    for (auto &&net : nets)
    {
        if(net.getId() == -1) continue;
       // auto net = net_it->second;
        //std::cout << "Net: " << net.getId() << std::endl;
        auto the_pads = pads{};
        std::vector<pin> pins = net.getPins();
        auto netClassId = net.getNetclassId();
        auto netClass = getNetclass(netClassId);
        for (size_t i = 0; i < pins.size(); ++i)
        {
            auto pin = pins[i];
            auto &&comp = getComponent(pin.m_comp_id);
            auto &&p = comp.getPadstack(pin.m_padstack_id);
            auto &&instance = getInstance(pin.m_inst_id);
            auto m_x = p.m_pos.m_x;
            auto m_y = p.m_pos.m_y;

            auto s = sin(instance.m_angle * -M_PI / 180);
            auto c = cos(instance.m_angle * -M_PI / 180);
            auto px = double((c * m_x - s * m_y) + instance.m_x);
            auto py = double((s * m_x + c * m_y) + instance.m_y);

            auto layerId = 0.0;
            for (auto &&layer : p.getLayers())
            {
                layerId = (double)getLayerId(layer);
                auto tp = point_3d{px, py, layerId}; // x, y, layer
                auto cords = rotateShapeCoordsByAngles(p.getShapeCoords(), p.getAngle(), instance.getAngle());
                // auto cords = shape_to_cords(p.m_size, p.m_pos, p.m_form, p.m_angle, instance.m_angle, p.m_roundrect_ratio);
                auto term = pad{p.m_rule.m_radius, p.m_rule.m_clearance, tp, cords, p.m_size};
                the_pads.push_back(term);

                //TODO: delete pin in all pin
                //TODO: uncomment, failed at bm2.artificial
                //False alaram, failed when conflict on the reference name
                all_pads.erase(std::find(begin(all_pads), end(all_pads), term));
            }
        }
        auto seg = paths{};
        the_tracks.push_back(track{std::to_string(track_id++), netClass.getTraceWidth() / 2, netClass.getViaDia() / 2, netClass.getClearance() / 2,
                                   the_pads, seg});
    }
    

    for (auto &&pad : all_pads) {
      auto p = path{};
      auto point = point_3d{};
      for (auto &&cord : pad.m_shape) {
        
        point.m_x = pad.m_pos.m_x+cord.m_x;
        point.m_y = pad.m_pos.m_y+cord.m_y;
        point.m_z = pad.m_pos.m_z;
        p.push_back(point);
      }
      //auto layer_name = index_to_layer_map[point.m_z];
      //layer_to_keepout_map[layer_name].push_back(p);
    }

    the_tracks.push_back(track{std::to_string(track_id++), 0.0, 0.0, 0.0, all_pads, all_keepouts});

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
                  << track.m_via_radius << " " << track.m_clearance << " (";
        for (auto i = 0; i < static_cast<int>(track.m_pads.size()); ++i)
        {
            auto &&term = track.m_pads[i];
            std::cout << "(" << term.m_radius << " " << term.m_clearance
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

void kicadPcbDataBase::getBoardBoundaryByPinLocation(double &minX, double &maxX, double &minY, double &maxY)
{
    minX = std::numeric_limits<double>::max();
    maxX = std::numeric_limits<double>::min();
    minY = std::numeric_limits<double>::max();
    maxY = std::numeric_limits<double>::min();

    for (auto &inst : instances)
    {
        auto &comp = getComponent(inst.getComponentId());
        auto &pads = comp.getPadstacks();
        for (auto &pad : pads)
        {
            point_2d pinLocation;
            getPinPosition(pad, inst, &pinLocation);
            minX = std::min(pinLocation.m_x, minX);
            maxX = std::max(pinLocation.m_x, maxX);
            minY = std::min(pinLocation.m_y, minY);
            maxY = std::max(pinLocation.m_y, maxY);
        }
    }
}

void kicadPcbDataBase::printSegment()
{
    for (auto &&s : net_to_segments_map)
    {
        std::cout << "net: " << s.first << std::endl;
        for (auto &&path : s.second)
        {
            for (auto &&point : path)
                std::cout << "(" << point.m_x << "," << point.m_y << "," << point.m_z << ")" << std::endl;
        }
    }
}

void kicadPcbDataBase::printUnconnectedPins()
{
    for (auto &&pin : unconnectedPins)
    {
        auto &&inst = instances[pin.m_inst_id];
        auto &&comp = components[pin.m_comp_id];
        padstack pad = comp.getPadstack(pin.m_padstack_id);
        std::cout << "comp: " << comp.getName() << " inst: " << inst.getName() << " pad: " << pad.getName() << std::endl;
    }
}

bool kicadPcbDataBase::isCopperLayer(const int id)
{
    if (id <= MAX_COPPER_LAYER_ID && id >= MIN_COPPER_LAYER_ID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool kicadPcbDataBase::isCopperLayer(const std::string &name)
{
    const auto &layerIte = layer_to_index_map.find(name);
    if (layerIte == layer_to_index_map.end())
    {
        std::cerr << __FUNCTION__ << "() No layer named: " << name << std::endl;
        return false;
    }

    if (layerIte->second <= MAX_COPPER_LAYER_ID && layerIte->second >= MIN_COPPER_LAYER_ID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void kicadPcbDataBase::printComp()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###             COMP              ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &comp : components)
    {
        std::cout << comp.getName() << " " << comp.getId() << " "
                  << "====================== " << std::endl;

        for (size_t i = 0; i < comp.m_lines.size(); ++i)
        {
            std::cout << "\tstart: (" << comp.m_lines[i].m_start.m_x << "," << comp.m_lines[i].m_start.m_y << ")" << std::endl;
            std::cout << "\tend: (" << comp.m_lines[i].m_end.m_x << "," << comp.m_lines[i].m_end.m_y << ")" << std::endl;
            std::cout << "\twidth: " << comp.m_lines[i].m_width << std::endl;
        }
        for (auto &pad : comp.m_pads)
        {
            std::cout << "\tpad: " << pad.m_name << ", padId: " << pad.m_id << " (" << pad.m_pos.m_x << "," << pad.m_pos.m_y << ") " << pad.m_angle << std::endl;
            std::cout << "\t\tsize: " << (int)pad.m_shape << " (" << pad.m_size.m_x << "," << pad.m_size.m_y << ")" << std::endl;
        }
    }
}

void kicadPcbDataBase::printLayer()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###             LAYER             ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (const auto &layerIte : index_to_layer_map)
    {
        std::cout << layerIte.first << " " << layerIte.second;
        std::cout << " " << isCopperLayer(layerIte.first);
        std::cout << " " << isCopperLayer(layerIte.second);
        std::cout << std::endl;
    }
}

void kicadPcbDataBase::printInst()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###             INST              ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &inst : instances)
    {
        std::cout << inst.getName() << ", instId: " << inst.getId() << ", compId: "
                  << inst.getComponentId() << "====================== " << std::endl;
        //TODO: API for below loop access
        for (auto &pin_it : inst.m_pin_net_map)
        {
            std::cout << "\tpinName: " << pin_it.first << " netId: " << pin_it.second << std::endl;
        }
    }
}

void kicadPcbDataBase::printNetclass()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###           NET CLASS           ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &netclass : netclasses)
    {
        std::cout << netclass.getName() << ", netclassId: " << netclass.getId()
                  << "====================== " << std::endl;
        std::cout << "\tClearance:   " << netclass.getClearance() << std::endl;
        std::cout << "\tTraceWidth:  " << netclass.getTraceWidth() << std::endl;
        std::cout << "\tViaDiameter: " << netclass.getViaDia() << std::endl;
        std::cout << "\tViaDrill:    " << netclass.getViaDrill() << std::endl;
        std::cout << "\tuViaDiameter:" << netclass.getMicroViaDia() << std::endl;
        std::cout << "\tuViaDrill:   " << netclass.getMicroViaDrill() << std::endl;
    }
}

void kicadPcbDataBase::printNet()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###              NET              ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &net : nets)
    {
        std::cout << net.getName() << ", netId: " << net.getId() << ", netDegree: "
                  << net.m_pins.size() << ", netclassId: " << net.getNetclassId() << std::endl;
        for (auto &pin : net.m_pins)
        {
            auto &inst = getInstance(pin.m_inst_id);
            auto &comp = getComponent(pin.m_comp_id);
            point_2d pos;
            //getPinPosition(inst.m_name, pin.m_name, &pos);
            getPinPosition(pin, &pos);

            /*
            padstack *pPad;
            if (!comp.getPadstack(pin.m_name, pPad))
            {
                std::cerr << __FUNCTION__ << ": component " << comp.m_name << " has no padstack named: " << pin.m_name << std::endl;
            }
            */

            auto &pad = comp.getPadstack(pin.m_padstack_id);
            auto angle = inst.m_angle + pad.m_angle;
            std::cout << "\tinst name: " << inst.m_name << " pin name: " << pad.m_name << " comp name: " << comp.m_name << " pos: (" << pos.m_x << "," << pos.m_y << ")";
            if (angle == 0 || angle == 180)
                std::cout << " width: " << pad.m_size.m_x << " height: " << pad.m_size.m_y << std::endl;
            else
                std::cout << " height: " << pad.m_size.m_x << " width: " << pad.m_size.m_y << std::endl;
        }

        for (auto &segment : net.m_segments)
        {
            points_2d p = segment.getPos();
            std::cout << "\tsegment id: " << segment.getId() << " pos start: (" << p[0].m_x << "," << p[0].m_y << ") end: (" << p[1].m_x << "," << p[1].m_y << ")";
            std::cout << " width: " << segment.getWidth() << " layer: " << segment.getLayer() << std::endl;
        }

        for (auto &via : net.m_vias)
        {
            point_2d p = via.getPos();
            std::vector<std::string> layers = via.getLayers();
            std::cout << "\tvia id: " << via.getId() << " pos : (" << p.m_x << "," << p.m_y << ") ";
            std::cout << " size: " << via.getSize() << " layer: " << layers[0] << " " << layers[1] << std::endl;
        }
    }
}

/* print info for Dongwon*/
void kicadPcbDataBase::printFile()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###              FILE             ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "#netId   instName   pinName   pinXPos   pinYPos   pinWidth   pinHeight   netType   diffPair   layerId   layerName" << std::endl;
    for (auto &net : nets)
    {
        //std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
        auto &pins = net.getPins();
        for (auto &&pin : pins)
        {
            auto &comp = getComponent(pin.m_comp_id);
            auto &pad = comp.getPadstack(pin.m_padstack_id);
            auto &inst = getInstance(pin.m_inst_id);
            point_2d pos;

            getPinPosition(pin, &pos);
            auto angle = inst.m_angle + pad.m_angle;
            std::cout << net.getId() << " " << inst.m_name << " " << pad.m_name << " " << pos.m_x << " " << pos.m_y << " ";
            if (angle == 0 || angle == 180)
                std::cout << pad.m_size.m_x << " " << pad.m_size.m_y;
            else
                std::cout << pad.m_size.m_x << " " << pad.m_size.m_y;

            if (net.isDiffPair())
                std::cout << " DIFFPAIR " << net.getDiffPairId() << " ";
            else
                std::cout << " SIGNAL " << net.getDiffPairId() << " ";

            std::cout << "( ";
            for (auto &&layer : pad.m_layers)
            {
                std::cout << layer_to_index_map[layer] << " ";
            }
            std::cout << ") ";

            std::cout << "( ";
            for (auto &&layer : pad.m_layers)
            {
                std::cout << layer << " ";
            }
            std::cout << ")" << std::endl;
        }
    }
    std::cout << "\n#keepoutId   layerId   layerName   xPos   yPos" << std::endl;
    int i = 0;
    for (auto &&keepout : layer_to_keepout_map)
    {
        for (auto &&path : keepout.second)
        {
            for (auto &&cord : path)
            {
                std::cout << i << " " << layer_to_index_map[keepout.first] << " " << keepout.first;
                std::cout << " " << cord.m_x << " " << cord.m_y << std::endl;
            }
            ++i;
        }
    }

    std::cout << "#unusedPinId   pinXPos   pinYPos   pinWidth   pinHeight   layerId   layerName" << std::endl;

    i = 0;
    for (auto &&pin : all_pads)
    {
        std::cout << i << " " << pin.m_pos.m_x << " " << pin.m_pos.m_y << " " << pin.m_size.m_x << " " << pin.m_size.m_y;
        std::cout << " " << pin.m_pos.m_z << " " << index_to_layer_map[pin.m_pos.m_z] << std::endl;
        ++i;
    }
}

// Warning! Doesn't count component rotation
void kicadPcbDataBase::printPcbRouterInfo()
{
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###         ROUTER INFO           ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    // Nets
    for (auto net : nets)
    {
        std::cout << net.getName() << ", netDegree(#pins): " << net.getPins().size() << ", netId: " << net.getId() << std::endl;
        auto &pins = net.getPins();
        for (auto &pin : pins)
        {
            auto &comp = getComponent(pin.m_comp_id);
            auto &pad = comp.getPadstack(pin.m_padstack_id);
            auto &inst = getInstance(pin.m_inst_id);
            point_2d pinLocation;

            getPinPosition(pin, &pinLocation);

            std::cout << "   inst name: " << inst.getName() << " (" << inst.m_x << " " << inst.m_y << ")"
                      << ", Rot: " << inst.m_angle
                      << " pin name: " << pad.m_name << " Rel. Loc:(" << pad.m_pos.m_x << " " << pad.m_pos.m_y << ")"
                      << ", Rot: " << pad.m_angle << ", Abs. Pin Loc:(" << pinLocation.m_x << ", " << pinLocation.m_y << ")"
                      << " comp name: " << comp.getName() << std::endl;
        }
    }

    // Routing keepouts
    std::cout << "\n#keepoutId   layerId   layerName   xPos   yPos" << std::endl;
    int i = 0;
    for (auto keepout : layer_to_keepout_map)
    {
        for (auto &path : keepout.second)
        {
            for (auto &cord : path)
            {
                std::cout << i << " " << layer_to_index_map[keepout.first] << " " << keepout.first;
                std::cout << " " << cord.m_x << " " << cord.m_y << std::endl;
            }
            ++i;
        }
    }

    // Unused Pins
    std::cout << std::endl;
    std::cout << "#unusedPinId   pinXPos   pinYPos   pinWidth   pinHeight   layerId   layerName" << std::endl;
    for (unsigned int i = 0; i < all_pads.size(); ++i)
    {
        auto &pin = all_pads.at(i);
        std::cout << i << " " << pin.m_pos << " " << pin.m_size.m_x << " " << pin.m_size.m_y;
        std::cout << " " << pin.m_pos.m_z << " " << index_to_layer_map[pin.m_pos.m_z] << std::endl;
    }
}

bool kicadPcbDataBase::getPcbRouterInfo(std::vector<std::set<std::pair<double, double>>> *routerInfo)
{
    int numNet = nets.size();
    // netId = 0 is default
    routerInfo->resize(numNet + 1);
    //int netCounter = 0;

    for (auto &net : nets)
    {
        assert(net.getId() <= numNet);

        //std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
        auto &pins = net.getPins();
        for (auto &pin : pins)
        {
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
            //auto &inst = getInstance(pin.m_inst_id);
            getPinPosition(pin, &pinLocation);
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

bool kicadPcbDataBase::getInstance(const std::string &name, instance *&inst)
{
    auto ite = instance_name_to_id.find(name);
    if (ite != instance_name_to_id.end())
    {
        inst = &(instances.at(ite->second));
        return true;
    }
    else
    {
        inst = nullptr;
        return false;
    }
}

bool kicadPcbDataBase::getComponent(const std::string &name, component *&comp)
{
    auto ite = component_name_to_id.find(name);
    if (ite != component_name_to_id.end())
    {
        comp = &(components.at(ite->second));
        return true;
    }
    else
    {
        comp = nullptr;
        return false;
    }
}

bool kicadPcbDataBase::getNet(const std::string &name, net *&net)
{
    auto ite = net_name_to_id.find(name);
    if (ite != net_name_to_id.end())
    {
        net = &(nets.at(ite->second));
        return true;
    }
    else
    {
        net = nullptr;
        return false;
    }
}

net &kicadPcbDataBase::getNet(const std::string &name)
{
    auto ite = net_name_to_id.find(name);
    return nets.at(ite->second);
}

//TODO: consider pin shape;
bool kicadPcbDataBase::getPinPosition(const std::string &instName, const std::string &pinName, point_2d *pos)
{
    if (!pos)
        return false;

    instance *inst;
    if (!getInstance(instName, inst))
    {
        std::cerr << __FUNCTION__ << "() No Instance named: " << instName << std::endl;
        return false;
    }

    if (!isComponentId(inst->getComponentId()))
    {
        std::cerr << __FUNCTION__ << "() Ilegal Component Id: " << inst->getComponentId() << ", from Instance: " << inst->getName() << std::endl;
        return false;
    }

    auto &comp = getComponent(inst->getComponentId());

    padstack *pad;
    if (!comp.getPadstack(pinName, pad))
    {
        std::cerr << __FUNCTION__ << "() No Padstack named: " << pinName << std::endl;
        return false;
    }
    getPinPosition(*pad, *inst, pos);
    return true;
}

bool kicadPcbDataBase::getPinPosition(const int inst_id, const int &pin_id, point_2d *pos)
{
    if (!isInstanceId(inst_id))
    {
        std::cerr << __FUNCTION__ << "() Ilegal Instance Id: " << inst_id << std::endl;
        return false;
    }

    auto &inst = getInstance(inst_id);
    if (!isComponentId(inst.getComponentId()))
    {
        std::cerr << __FUNCTION__ << "() Ilegal Component Id: " << inst.getComponentId() << ", from Instance: " << inst.getName() << std::endl;
        return false;
    }

    auto &comp = getComponent(inst.getComponentId());
    if (!comp.isPadstackId(pin_id))
    {
        std::cerr << __FUNCTION__ << "() Ilegal Pin(Padstack) Id: " << pin_id << std::endl;
        return false;
    }

    auto &pad = comp.getPadstack(pin_id);
    getPinPosition(pad, inst, pos);
    return true;
}

void kicadPcbDataBase::getPinPosition(const padstack &pad, const instance &inst, point_2d *pos)
{
    double padX = pad.m_pos.m_x, padY = pad.m_pos.m_y;
    auto instAngle = inst.m_angle * (-M_PI / 180.0);

    auto s = sin((float)instAngle);
    auto c = cos((float)instAngle);
    pos->m_x = double((c * padX - s * padY) + inst.m_x);
    pos->m_y = double((s * padX + c * padY) + inst.m_y);
}

bool kicadPcbDataBase::getPinPosition(const pin &p, point_2d *pos)
{
    //TODO: range checking
    auto &inst = getInstance(p.m_inst_id);
    auto &comp = getComponent(p.m_comp_id);
    auto &pad = comp.getPadstack(p.m_padstack_id);
    getPinPosition(pad, inst, pos);
    return true;
}

bool kicadPcbDataBase::getInstBBox(const int inst_id, point_2d *bBox)
{
    if (!bBox)
        return false;
    if (!isInstanceId(inst_id))
        return false;

    auto &inst = getInstance(inst_id);

    if (!isComponentId(inst.getComponentId()))
        return false;

    auto &comp = getComponent(inst.getComponentId());
    auto angle = inst.m_angle;
    auto minx = double(1000000.0);
    auto miny = double(1000000.0);
    auto maxx = double(-1000000.0);
    auto maxy = double(-1000000.0);
    for (size_t i = 0; i < comp.m_lines.size(); ++i)
    {
        auto start = comp.m_lines[i].m_start;
        auto end = comp.m_lines[i].m_end;
        auto width = comp.m_lines[i].m_width / 2;
        minx = std::min(start.m_x - width, minx);
        maxx = std::max(start.m_x + width, maxx);
        minx = std::min(end.m_x - width, minx);
        maxx = std::max(end.m_x + width, maxx);

        miny = std::min(start.m_y - width, miny);
        maxy = std::max(start.m_y + width, maxy);
        miny = std::min(end.m_y - width, miny);
        maxy = std::max(end.m_y + width, maxy);
    }

    for (size_t i = 0; i < comp.m_circles.size(); ++i)
    {
        auto center = comp.m_circles[i].m_center;
        auto end = comp.m_circles[i].m_end;
        auto width = comp.m_circles[i].m_width / 2;
        minx = std::min(center.m_x - end.m_x - width, minx);
        maxx = std::max(center.m_x + width + end.m_x, maxx);

        miny = std::min(center.m_y - end.m_y - width, miny);
        maxy = std::max(center.m_y + end.m_y + width, maxy);
    }

    for (size_t i = 0; i < comp.m_polys.size(); ++i)
    {
        for (size_t j = 0; j < comp.m_polys[i].m_shape.size(); ++j)
        {
            auto point = comp.m_polys[i].m_shape[j];
            auto width = comp.m_polys[i].m_width / 2;
            minx = std::min(point.m_x - width, minx);
            maxx = std::max(point.m_x + width, maxx);
            miny = std::min(point.m_y - width, miny);
            maxy = std::max(point.m_y + width, maxy);
        }
    }

    for (size_t i = 0; i < comp.m_arcs.size(); ++i)
    {
        auto start = comp.m_arcs[i].m_start;
        auto end = comp.m_arcs[i].m_end;
        auto width = comp.m_arcs[i].m_width / 2;
        minx = std::min(start.m_x - width, minx);
        maxx = std::max(start.m_x + width, maxx);
        minx = std::min(end.m_x - width, minx);
        maxx = std::max(end.m_x + width, maxx);

        miny = std::min(start.m_y - width, miny);
        maxy = std::max(start.m_y + width, maxy);
        miny = std::min(end.m_y - width, miny);
        maxy = std::max(end.m_y + width, maxy);
    }

    auto pads = comp.getPadstacks();
    for (auto &pad : pads)
    {
        auto pad_x = pad.m_pos.m_x;
        auto pad_y = pad.m_pos.m_y;
        auto size = pad.m_size;
        auto pad_angle = pad.m_angle;

        if (pad_angle == 0 || pad_angle == 180)
        {
            minx = std::min(pad_x - size.m_x / 2, minx);
            maxx = std::max(pad_x + size.m_x / 2, maxx);

            miny = std::min(pad_y - size.m_y / 2, miny);
            maxy = std::max(pad_y + size.m_y / 2, maxy);
        }

        if (pad_angle == 90 || pad_angle == 270)
        {
            miny = std::min(pad_x - size.m_x / 2, miny);
            maxy = std::max(pad_x + size.m_x / 2, maxy);

            minx = std::min(pad_y - size.m_y / 2, minx);
            maxx = std::max(pad_y + size.m_y / 2, maxx);
        }
    }

    double width = abs(maxx - minx);
    double height = abs(maxy - miny);
    if (angle == 90 || angle == 270)
    {
        bBox->m_x = height;
        bBox->m_y = width;
    }
    else
    {
        bBox->m_x = width;
        bBox->m_y = height;
    }

    return true;
}

void kicadPcbDataBase::getPadstackRotatedWidthAndHeight(const instance &inst, const padstack &pad, double &width, double &height)
{
    double overallRot = inst.getAngle() + pad.getAngle();
    if ((int)(overallRot / 90.0) % 2 == 0)
    {
        width = pad.getSize().m_x;
        height = pad.getSize().m_y;
    }
    else
    {
        width = pad.getSize().m_y;
        height = pad.getSize().m_x;
    }
}

int kicadPcbDataBase::getLayerId(const std::string &layerName)
{
    auto layerIte = layer_to_index_map.find(layerName);
    if (layerIte != layer_to_index_map.end())
    {
        return layerIte->second;
    }
    return -1;
}

/*
void kicadPcbDataBase::printKiCad()
{
    std::string instName;
    for(auto &&sub_node : tree.m_branches)
    {
        
        if(sub_node.m_value == "module") {
            for(auto &&module_node : sub_node) {
                if (module_node.m_value == "fp_text" && module_node.m_branches[0].m_value == "reference")
                {
                    instName = module_node.m_branches[1].m_value;
                }
            }
            
            instance *inst;
            getInstance(instName, inst);

            if(sub_node.m_branches[1].m_value == "locked") {
                sub_node.m_branches[2].m_branches[0].m_value = inst->//layer
                
                the_instance.m_id = (int)instances.size();
                get_2d(ss, begin(sub_node.m_branches[5].m_branches), the_instance.m_x, the_instance.m_y);
                if (int(sub_node.m_branches[5].m_branches.size()) == 3)
                    get_value(ss, begin(sub_node.m_branches[5].m_branches) + 2, the_instance.m_angle);
                else
                    the_instance.m_angle = 0;
            }
            else {
                // Get Instance X, Y, Rot
                layer = sub_node.m_branches[1].m_branches[0].m_value;
                the_instance.m_id = (int)instances.size();
                get_2d(ss, begin(sub_node.m_branches[4].m_branches), the_instance.m_x, the_instance.m_y);
                if (int(sub_node.m_branches[4].m_branches.size()) == 3)
                    get_value(ss, begin(sub_node.m_branches[4].m_branches) + 2, the_instance.m_angle);
                else
                    the_instance.m_angle = 0;
            }


        }

    } 
}
*/

