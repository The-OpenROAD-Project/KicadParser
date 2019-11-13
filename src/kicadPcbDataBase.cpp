#include "kicadPcbDataBase.h"

bool kicadPcbDataBase::buildKicadPcb() {
    //Basic file extension checking
    std::string fileExt = utilParser::getFileExtension(m_fileName);
    if (fileExt != "kicad_pcb") {
        std::cerr << "Support .kicad_pcb file only. Current input file: " << m_fileName << std::endl;
        return false;
    }

    kicadParser parser(m_fileName);
    if (!parser.parseKicadPcb(&tree))
        return false;

    std::stringstream ss;
    auto default_rule = rule{0.25, 0.25};
    for (auto &&sub_node : tree.m_branches) {
        //layer part
        if (sub_node.m_value == "layers") {
            for (auto &&layer_node : sub_node.m_branches) {
                //auto layer_index = 0;
                auto layer_name = layer_node.m_branches[0].m_value;
                auto layer_type = layer_node.m_branches[1].m_value;
                //Use "signal" to decide
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
            net_id_to_name[net_index] = net_name;
            net_name_to_id[net_name] = net_index;

            // Identify differential pairs
            if (net_name.back() == '+' || net_name.back() == '-') {
                auto name = net_name.substr(0, net_name.size() - 1);
                if (name_to_diff_pair_net_map.find(name) != name_to_diff_pair_net_map.end()) {
                    auto &&pair = name_to_diff_pair_net_map[name];
                    pair.second = net_index;
                } else {
                    name_to_diff_pair_net_map[name] = std::make_pair(net_index, -1);
                }
            }
        }
        //net class (Create Net Instances)
        else if (sub_node.m_value == "net_class") {
            nets.resize(net_id_to_name.size());
            int m_netclass_id = (int)netclasses.size();
            auto m_netclass_name = sub_node.m_branches.front().m_value;
            double m_clearance = 0.0, m_trace_width = 0.0, m_via_dia = 0.0, m_via_drill = 0.0, m_uvia_dia = 0.0, m_uvia_drill = 0.0;

            for (auto &&net_class_node : sub_node.m_branches) {
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
                    auto net_id = -1;
                    auto name = net_name.substr(0, net_name.size() - 1);
                    auto pair = std::make_pair(-1, -1);
                    auto netIte = net_name_to_id.find(net_name);
                    if (netIte != net_name_to_id.end()) {
                        net_id = netIte->second;
                        assert(net_id < (int)nets.size());
                    } else {
                        std::cerr << "Undefined net name \"" << net_name << "\" added in the netclass." << std::endl;
                    }
                    if (name_to_diff_pair_net_map.find(name) != name_to_diff_pair_net_map.end()) {
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
        else if (sub_node.m_value == "module") {
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
            //TODO: Refactor to use for-loop and Key...........
            // Get Instance X, Y, Rot
            if (sub_node.m_branches[1].m_value == "locked") {
                layer = sub_node.m_branches[2].m_branches[0].m_value;
                the_instance.m_layer = this->getLayerId(layer);
                the_instance.m_id = (int)instances.size();
                get_2d(ss, begin(sub_node.m_branches[5].m_branches), the_instance.m_x, the_instance.m_y);
                if (int(sub_node.m_branches[5].m_branches.size()) == 3)
                    get_value(ss, begin(sub_node.m_branches[5].m_branches) + 2, the_instance.m_angle);
                else
                    the_instance.m_angle = 0;
            } else {
                layer = sub_node.m_branches[1].m_branches[0].m_value;
                the_instance.m_layer = this->getLayerId(layer);
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
            if (comp_it == component_name_to_id.end()) {
                comp_id = (int)components.size();
            } else {
                comp_id = comp_it->second;
            }
            component the_comp{comp_id, component_name};
            int noNameId = 0;
            for (auto &&module_node : sub_node.m_branches) {
                //TODO:: Handle (fp_text value ...)
                if (module_node.m_value == "fp_text" && module_node.m_branches[0].m_value == "reference") {
                    the_instance.m_name = module_node.m_branches[1].m_value;
                }
                // Create Component if needed
                if (comp_it == component_name_to_id.end()) {
                    if (module_node.m_value == "fp_line") {
                        auto the_line = line{};
                        if (the_instance.m_angle == 0 || the_instance.m_angle == 180) {
                            get_2d(ss, begin(module_node.m_branches[0].m_branches), the_line.m_start.m_x, the_line.m_start.m_y);
                            get_2d(ss, begin(module_node.m_branches[1].m_branches), the_line.m_end.m_x, the_line.m_end.m_y);
                        } else {
                            get_2d(ss, begin(module_node.m_branches[0].m_branches), the_line.m_start.m_y, the_line.m_start.m_x);
                            get_2d(ss, begin(module_node.m_branches[1].m_branches), the_line.m_end.m_y, the_line.m_end.m_x);
                        }
                        get_value(ss, begin(module_node.m_branches[3].m_branches), the_line.m_width);
                        the_comp.m_lines.push_back(the_line);
                    } else if (module_node.m_value == "fp_poly") {
                        auto the_poly = poly{};
                        for (auto &&cor_node : module_node.m_branches[0].m_branches) {
                            auto the_point = point_2d{};
                            get_2d(ss, begin(cor_node.m_branches), the_point.m_x, the_point.m_y);
                            the_poly.m_shape.push_back(the_point);
                        }
                        get_value(ss, begin(module_node.m_branches[2].m_branches), the_poly.m_width);
                        the_comp.m_polys.push_back(the_poly);
                    } else if (module_node.m_value == "fp_circle") {
                        auto the_circle = circle{};
                        get_2d(ss, begin(module_node.m_branches[0].m_branches), the_circle.m_center.m_x, the_circle.m_center.m_y);
                        get_2d(ss, begin(module_node.m_branches[1].m_branches), the_circle.m_end.m_x, the_circle.m_end.m_y);
                        get_value(ss, begin(module_node.m_branches[3].m_branches), the_circle.m_width);
                        the_comp.m_circles.push_back(the_circle);
                    } else if (module_node.m_value == "fp_arc") {
                        auto the_arc = arc{};
                        get_2d(ss, begin(module_node.m_branches[0].m_branches), the_arc.m_start.m_x, the_arc.m_start.m_y);
                        get_2d(ss, begin(module_node.m_branches[1].m_branches), the_arc.m_end.m_x, the_arc.m_end.m_y);
                        get_value(ss, begin(module_node.m_branches[2].m_branches), the_arc.m_angle);
                        get_value(ss, begin(module_node.m_branches[4].m_branches), the_arc.m_width);
                        the_comp.m_arcs.push_back(the_arc);
                    } else if (module_node.m_value == "pad") {
                        auto the_padstack = padstack{};
                        auto the_point = point_2d{};
                        auto points = points_2d{};
                        the_padstack.m_id = the_comp.m_pads.size();
                        the_padstack.m_rule = default_rule;

                        auto form = module_node.m_branches[2].m_value;
                        auto type = module_node.m_branches[1].m_value;
                        the_padstack.m_name = module_node.m_branches[0].m_value;
                        if (the_padstack.m_name == "\"\"") {
                            the_padstack.m_name = "Unnamed" + std::to_string(noNameId);
                            ++noNameId;
                        }

                        the_padstack.setShape(form);
                        the_padstack.setType(type);

                        get_2d(ss, begin(module_node.m_branches[3].m_branches), the_padstack.m_pos.m_x, the_padstack.m_pos.m_y);
                        get_2d(ss, begin(module_node.m_branches[4].m_branches), the_point.m_x, the_point.m_y);
                        the_padstack.m_size = the_point;
                        if ((int)module_node.m_branches[3].m_branches.size() == 3) {
                            get_value(ss, begin(module_node.m_branches[3].m_branches) + 2, the_padstack.m_angle);
                        } else {
                            the_padstack.m_angle = 0;
                        }
                        the_padstack.m_angle = the_padstack.m_angle - the_instance.m_angle;

                        if (type == "smd") {
                            for (auto &&layer_node : module_node.m_branches[5].m_branches) {
                                if (layer_node.m_value == "Top" || layer_node.m_value == "Bottom") {
                                    the_padstack.m_layers.push_back(layer_node.m_value);
                                }
                            }
                        } else if (type == "thru_hole" || type == "np_thru_hole") {
                            for (auto &&layer_node : module_node.m_branches[6].m_branches) {
                                if (layer_node.m_value == "*.Cu") {
                                    for (auto &layer : layer_to_index_map)
                                        the_padstack.m_layers.push_back(layer.first);
                                }
                            }
                        } else {
                            for (auto &&layer_node : module_node.m_branches[5].m_branches) {
                                if (layer_to_index_map.find(layer_node.m_value) != layer_to_index_map.end())
                                    the_padstack.m_layers.push_back(layer_node.m_value);
                            }
                        }

                        if (form == "circle") {
                            the_padstack.m_rule.m_radius = the_padstack.m_size.m_x / 2;
                        } else if (form == "oval") {
                            the_padstack.m_rule.m_radius = the_padstack.m_size.m_x / 4;
                            auto point1 = point_2d{the_padstack.m_size.m_y / (-2), 0};
                            auto point2 = point_2d{the_padstack.m_size.m_y / 2, 0};
                            the_padstack.m_shape_coords.push_back(point1);
                            the_padstack.m_shape_coords.push_back(point2);
                        } else if (form == "rect") {
                            the_padstack.m_rule.m_radius = 0.0;
                            auto x1 = the_padstack.m_size.m_x / (-2);
                            auto y1 = the_padstack.m_size.m_y / 2;
                            auto x2 = the_padstack.m_size.m_x / 2;
                            auto y2 = the_padstack.m_size.m_y / (-2);
                            the_padstack.m_shape_coords.push_back(point_2d{x1, y1});
                            the_padstack.m_shape_coords.push_back(point_2d{x2, y1});
                            the_padstack.m_shape_coords.push_back(point_2d{x2, y2});
                            the_padstack.m_shape_coords.push_back(point_2d{x1, y2});
                            the_padstack.m_shape_coords.push_back(point_2d{x1, y1});
                        } else if (form == "roundrect") {
                            the_padstack.m_rule.m_radius = 0.0;
                            get_value(ss, begin(module_node.m_branches[6].m_branches), the_padstack.m_roundrect_ratio);
                            the_padstack.m_shape_coords = roundrect_to_shape_coords(the_padstack.m_size, the_padstack.m_roundrect_ratio);
                            /*
                            //TEST
                            for(auto &&cord : the_padstack.m_shape) {
                            std::cout << "(" << cord.m_x << "," << cord.m_y << ") ";
                            }
                            std::cout << std::endl;*/
                        }

                        the_padstack.m_shape_polygon = shape_to_coords(the_padstack.getSize(), point_2d{0, 0}, the_padstack.getPadShape(), the_instance.getAngle(), the_padstack.getAngle(), the_padstack.getRoundRectRatio(), 32);
                        the_padstack.m_rule.m_clearance = 0;  //TODO: double check
                        the_comp.m_pads.push_back(the_padstack);
                        the_comp.m_pad_name_to_id[the_padstack.m_name] = the_padstack.m_id;
                    }
                }
            }

            if (comp_it == component_name_to_id.end()) {
                components.push_back(the_comp);
                component_name_to_id[component_name] = comp_id;
            }

            //Find the connection of the pad
            noNameId = 0;
            for (auto &&pad_node : sub_node.m_branches) {
                if (pad_node.m_value == "pad") {
                    auto pin_name = pad_node.m_branches[0].m_value;
                    if (pin_name == "\"\"") {
                        pin_name = "Unnamed" + std::to_string(noNameId);
                        ++noNameId;
                    }
                    int net_index = 0;
                    std::string net_name = "";
                    bool connected = false;
                    for (auto &&net_node : pad_node.m_branches) {
                        if (net_node.m_value == "net") {
                            connected = true;
                            net_name = net_node.m_branches[1].m_value;
                            get_value(ss, begin(net_node.m_branches), net_index);
                            break;
                        }
                    }

                    if (!isComponentId(comp_id)) {
                        std::cerr << __FUNCTION__ << "() ilegal component id: " << comp_id << std::endl;
                        continue;
                    }

                    auto &comp = getComponent(comp_id);
                    int padstack_id = -1;
                    if (!comp.getPadstackId(pin_name, &padstack_id)) {
                        std::cerr << __FUNCTION__ << "() ilegal pin name: " << pin_name << std::endl;
                        continue;
                    }
                    auto &the_padstack = comp.getPadstack(padstack_id);
                    Pin the_pin{padstack_id, comp_id, the_instance.m_id};

                    if (the_padstack.getType() == padType::SMD) {
                        for (auto &&layer_node : pad_node.m_branches[5].m_branches) {
                            if (layer_node.m_value == "Top" || layer_node.m_value == "Bottom") {
                                the_pin.m_layers.push_back(this->getLayerId(layer_node.m_value));
                            }
                        }
                    } else if (the_padstack.getType() == padType::THRU_HOLE || the_padstack.getType() == padType::NP_THRU_HOLE) {
                        for (auto &&layer_node : pad_node.m_branches[6].m_branches) {
                            if (layer_node.m_value == "*.Cu") {
                                for (auto &layer : index_to_layer_map)
                                    the_pin.m_layers.push_back(layer.first);
                            }
                        }
                    } else {
                        for (auto &&layer_node : pad_node.m_branches[5].m_branches) {
                            auto layerIte = layer_to_index_map.find(layer_node.m_value);
                            if (layerIte != layer_to_index_map.end())
                                the_pin.m_layers.push_back(layerIte->second);
                        }
                    }

                    if (connected == true) {
                        the_instance.m_pin_net_map[pin_name] = net_index;
                        auto &the_net = getNet(net_name);
                        the_net.addPin(the_pin);
                    } else {
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
        else if (sub_node.m_value == "gr_line") {
        }
        // TODO: belongs to Net Instance
        else if (sub_node.m_value == "segment") {
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
        else if (sub_node.m_value == "via") {
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
                if (via_node.m_value == "at") {
                    get_2d(ss, begin(via_node.m_branches), p.m_x, p.m_y);
                } else if (via_node.m_value == "size") {
                    get_value(ss, begin(via_node.m_branches), size);
                } else if (via_node.m_value == "net") {
                    get_value(ss, begin(via_node.m_branches), netId);
                } else if (via_node.m_value == "layers") {
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
        else if (sub_node.m_value == "zone") {
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

    for (auto &inst : instances) {
        auto comp = components.at(inst.getComponentId());
        for (auto &padstack : comp.getPadstacks()) {
            auto m_x = padstack.m_pos.m_x;
            auto m_y = padstack.m_pos.m_y;
            auto s = sin(inst.m_angle * -M_PI / 180);
            auto c = cos(inst.m_angle * -M_PI / 180);
            auto px = double((c * m_x - s * m_y) + inst.m_x);
            auto py = double((s * m_x + c * m_y) + inst.m_y);
            auto layerId = 0.0;
            for (auto &&layer : padstack.m_layers) {
                layerId = (double)layer_to_index_map[layer];
                auto tp = point_3d{px, py, layerId};  // x, y, layer

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

                for (auto &&p1 : cords) {
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
    m_boundary.push_back(point_2d{minx, miny});
    m_boundary.push_back(point_2d{maxx, maxy});

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
    for (auto &&track : the_tracks) {
        std::cout << "(" << track.m_id << " " << track.m_track_radius << " "
                  << track.m_via_radius << " " << track.m_clearance << " (";
        for (auto i = 0; i < static_cast<int>(track.m_pads.size()); ++i) {
            auto &&term = track.m_pads[i];
            std::cout << "(" << term.m_radius << " " << term.m_clearance
                      << " (" << term.m_pos.m_x - minx
                      << " " << term.m_pos.m_y - miny
                      << " " << term.m_pos.m_z << ") (";
            for (auto j = 0; j < static_cast<int>(term.m_shape.size()); ++j) {
                auto cord = term.m_shape[j];
                std::cout << "(" << cord.m_x << " " << cord.m_y << ")";
            }
            std::cout << "))";
        }
        std::cout << ") (";
        for (auto i = 0; i < static_cast<int>(track.m_paths.size()); ++i) {
            std::cout << "(";
            auto &&p = track.m_paths.at(i);
            for (auto j = 0; j < static_cast<int>(p.size()); ++j) {
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

void kicadPcbDataBase::getBoardBoundaryByPinLocation(double &minX, double &maxX, double &minY, double &maxY) {
    minX = std::numeric_limits<double>::max();
    maxX = std::numeric_limits<double>::min();
    minY = std::numeric_limits<double>::max();
    maxY = std::numeric_limits<double>::min();

    for (auto &inst : instances) {
        auto &comp = getComponent(inst.getComponentId());
        auto &pads = comp.getPadstacks();
        for (auto &pad : pads) {
            point_2d pinLocation;
            getPinPosition(pad, inst, &pinLocation);
            minX = std::min(pinLocation.m_x, minX);
            maxX = std::max(pinLocation.m_x, maxX);
            minY = std::min(pinLocation.m_y, minY);
            maxY = std::max(pinLocation.m_y, maxY);
        }
    }
}

void kicadPcbDataBase::printSegment() {
    for (auto &&s : net_to_segments_map) {
        std::cout << "net: " << s.first << std::endl;
        for (auto &&path : s.second) {
            for (auto &&point : path)
                std::cout << "(" << point.m_x << "," << point.m_y << "," << point.m_z << ")" << std::endl;
        }
    }
}

void kicadPcbDataBase::printUnconnectedPins() {
    for (auto &&pin : unconnectedPins) {
        auto &&inst = instances[pin.m_inst_id];
        auto &&comp = components[pin.m_comp_id];
        padstack pad = comp.getPadstack(pin.m_padstack_id);
        std::cout << "comp: " << comp.getName() << " inst: " << inst.getName() << " pad: " << pad.getName() << std::endl;
    }
}

bool kicadPcbDataBase::isCopperLayer(const int id) {
    if (id <= MAX_COPPER_LAYER_ID && id >= MIN_COPPER_LAYER_ID) {
        return true;
    } else {
        return false;
    }
}

bool kicadPcbDataBase::isCopperLayer(const std::string &name) {
    const auto &layerIte = layer_to_index_map.find(name);
    if (layerIte == layer_to_index_map.end()) {
        std::cerr << __FUNCTION__ << "() No layer named: " << name << std::endl;
        return false;
    }

    if (layerIte->second <= MAX_COPPER_LAYER_ID && layerIte->second >= MIN_COPPER_LAYER_ID) {
        return true;
    } else {
        return false;
    }
}

void kicadPcbDataBase::printComp() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###             COMP              ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &comp : components) {
        std::cout << comp.getName() << ", Id:" << comp.getId()
                  << "====================== " << std::endl;
        std::cout << "\t-----LINE-----" << std::endl;
        for (size_t i = 0; i < comp.m_lines.size(); ++i) {
            std::cout << "\tstart: (" << comp.m_lines[i].m_start.m_x << "," << comp.m_lines[i].m_start.m_y << "), ";
            std::cout << "\tend: (" << comp.m_lines[i].m_end.m_x << "," << comp.m_lines[i].m_end.m_y << "), ";
            std::cout << "\twidth: " << comp.m_lines[i].m_width << std::endl;
        }

        std::cout << "\t-----CIRCLE-----" << std::endl;
        for (auto &c : comp.m_circles) {
            std::cout << "center: " << c.m_center << ", end: " << c.m_end << ", width: " << c.m_width << std::endl;
        }
        std::cout << "\t-----POLY-----" << std::endl;
        for (auto &p : comp.m_polys) {
            for (auto &po : p.m_shape) {
                std::cout << "point: " << po << ", ";
            }
            std::cout << "width: " << p.m_width << std::endl;
        }

        for (auto &pad : comp.m_pads) {
            std::cout << "\tpad: " << pad.m_name << ", padId: " << pad.m_id << " (" << pad.m_pos.m_x << "," << pad.m_pos.m_y << ") , angle: " << pad.m_angle << ", Type: " << (int)pad.getType() << std::endl;
            std::cout << "\t\tshape: " << (int)pad.m_shape << ", size: (" << pad.m_size.m_x << "," << pad.m_size.m_y << ")" << std::endl;
        }
    }
}

void kicadPcbDataBase::printLayer() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###             LAYER             ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (const auto &layerIte : index_to_layer_map) {
        std::cout << layerIte.first << " " << layerIte.second;
        std::cout << " " << isCopperLayer(layerIte.first);
        std::cout << " " << isCopperLayer(layerIte.second);
        std::cout << std::endl;
    }
}

void kicadPcbDataBase::printInst() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###             INST              ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &inst : instances) {
        point_2d instSize;
        getCompBBox(inst.getComponentId(), &instSize);
        std::cout << inst.getName() << ", instId: " << inst.getId() << ", compId: " << inst.getComponentId()
                  << ", layer: " << inst.getLayer()
                  << ", Bbox: " << instSize.m_x << " " << instSize.m_y
                  << "====================== " << std::endl;
        //TODO: API for below loop access
        for (auto &pin_it : inst.m_pin_net_map) {
            std::cout << "\tpinName: " << pin_it.first << " netId: " << pin_it.second << std::endl;
        }
    }
}

///// Bookshelf nodes format
void kicadPcbDataBase::printNodes() {
    /*std::cout << "UCLA nodes 1.0" << std::endl;
    std::cout << std::endl;
    std::cout << "# Created    :" << std::endl;
    std::cout << "# Created by :" << std::endl;
    std::endl;
    std::cout << "NumNodes : " << instances.size() << std::endl
    std::cout << "NumTerminals : 0" << std::endl;*/

    for (auto &inst : instances) {
        point_2d instSize;
        getCompBBox(inst.getComponentId(), &instSize);
        std::cout << inst.getName()
                  << "          " << instSize.m_x << "          " << instSize.m_y << std::endl;
    }
}

void kicadPcbDataBase::printNetclass() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###           NET CLASS           ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &netclass : netclasses) {
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

void kicadPcbDataBase::printNet() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###              NET              ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    for (auto &net : nets) {
        std::cout << net.getName() << ", netId: " << net.getId() << ", netDegree: "
                  << net.m_pins.size() << ", netclassId: " << net.getNetclassId() << std::endl;
        for (auto &pin : net.m_pins) {
            auto &inst = getInstance(pin.m_inst_id);
            auto &comp = getComponent(pin.m_comp_id);
            point_2d pos;
            getPinPosition(pin, &pos);

            auto &pad = comp.getPadstack(pin.m_padstack_id);
            std::cout << "\tinst name: " << inst.m_name << " pin name: " << pad.m_name << " comp name: " << comp.m_name << " pos: (" << pos.m_x << "," << pos.m_y << ")";
            double width = -1, height = -1;
            getPadstackRotatedWidthAndHeight(inst, pad, width, height);
            std::cout << " width: " << width << " height: " << height;
            std::cout << " padType: " << (int)pad.getType() << " layers:";
            // Show Pin layers
            for (auto layerId : pin.getLayers()) {
                std::cout << " " << layerId << "(" << this->getLayerName(layerId) << ")";
            }
            std::cout << std::endl;
        }

        for (auto &segment : net.m_segments) {
            points_2d p = segment.getPos();
            std::cout << "\tsegment id: " << segment.getId() << " pos start: (" << p[0].m_x << "," << p[0].m_y << ") end: (" << p[1].m_x << "," << p[1].m_y << ")";
            std::cout << " width: " << segment.getWidth() << " layer: " << segment.getLayer() << std::endl;
        }

        for (auto &via : net.m_vias) {
            point_2d p = via.getPos();
            std::vector<std::string> layers = via.getLayers();
            std::cout << "\tvia id: " << via.getId() << " pos : (" << p.m_x << "," << p.m_y << ") ";
            std::cout << " size: " << via.getSize() << " layer: " << layers[0] << " " << layers[1] << std::endl;
        }
    }
}

/* print info for Dongwon*/
void kicadPcbDataBase::printFile() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###              FILE             ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "#netId   instName   pinName   pinXPos   pinYPos   pinWidth   pinHeight   netType   diffPair   layerId   layerName" << std::endl;
    for (auto &net : nets) {
        //std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
        auto &pins = net.getPins();
        for (auto &&pin : pins) {
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
        for (auto &&path : keepout.second) {
            for (auto &&cord : path) {
                std::cout << i << " " << layer_to_index_map[keepout.first] << " " << keepout.first;
                std::cout << " " << cord.m_x << " " << cord.m_y << std::endl;
            }
            ++i;
        }
    }

    std::cout << "#unusedPinId   pinXPos   pinYPos   pinWidth   pinHeight   layerId   layerName" << std::endl;

    i = 0;
    for (auto &&pin : all_pads) {
        std::cout << i << " " << pin.m_pos.m_x << " " << pin.m_pos.m_y << " " << pin.m_size.m_x << " " << pin.m_size.m_y;
        std::cout << " " << pin.m_pos.m_z << " " << index_to_layer_map[pin.m_pos.m_z] << std::endl;
        ++i;
    }
}

// Warning! Doesn't count component rotation
void kicadPcbDataBase::printPcbRouterInfo() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###         ROUTER INFO           ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    // Nets
    for (auto net : nets) {
        std::cout << net.getName() << ", netDegree(#pins): " << net.getPins().size() << ", netId: " << net.getId() << std::endl;
        auto &pins = net.getPins();
        for (auto &pin : pins) {
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
    for (auto keepout : layer_to_keepout_map) {
        for (auto &path : keepout.second) {
            for (auto &cord : path) {
                std::cout << i << " " << layer_to_index_map[keepout.first] << " " << keepout.first;
                std::cout << " " << cord.m_x << " " << cord.m_y << std::endl;
            }
            ++i;
        }
    }

    // Unused Pins
    std::cout << std::endl;
    std::cout << "#unusedPinId   pinXPos   pinYPos   pinWidth   pinHeight   layerId   layerName" << std::endl;
    for (unsigned int i = 0; i < all_pads.size(); ++i) {
        auto &pin = all_pads.at(i);
        std::cout << i << " " << pin.m_pos << " " << pin.m_size.m_x << " " << pin.m_size.m_y;
        std::cout << " " << pin.m_pos.m_z << " " << index_to_layer_map[pin.m_pos.m_z] << std::endl;
    }
}

bool kicadPcbDataBase::getPcbRouterInfo(std::vector<std::set<std::pair<double, double>>> *routerInfo) {
    int numNet = nets.size();
    // netId = 0 is default
    routerInfo->resize(numNet + 1);
    //int netCounter = 0;

    for (auto &net : nets) {
        assert(net.getId() <= numNet);

        //std::cout << net_it->first << " " << net.getClearance() << " " << net.pins.size() << std::endl;
        auto &pins = net.getPins();
        for (auto &pin : pins) {
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

bool kicadPcbDataBase::getInstance(const std::string &name, instance *&inst) {
    auto ite = instance_name_to_id.find(name);
    if (ite != instance_name_to_id.end()) {
        inst = &(instances.at(ite->second));
        return true;
    } else {
        inst = nullptr;
        return false;
    }
}

bool kicadPcbDataBase::getComponent(const std::string &name, component *&comp) {
    auto ite = component_name_to_id.find(name);
    if (ite != component_name_to_id.end()) {
        comp = &(components.at(ite->second));
        return true;
    } else {
        comp = nullptr;
        return false;
    }
}

bool kicadPcbDataBase::getNet(const std::string &name, net *&net) {
    auto ite = net_name_to_id.find(name);
    if (ite != net_name_to_id.end()) {
        net = &(nets.at(ite->second));
        return true;
    } else {
        net = nullptr;
        return false;
    }
}

net &kicadPcbDataBase::getNet(const std::string &name) {
    auto ite = net_name_to_id.find(name);
    return nets.at(ite->second);
}

//TODO: consider pin shape;
bool kicadPcbDataBase::getPinPosition(const std::string &instName, const std::string &pinName, point_2d *pos) {
    if (!pos)
        return false;

    instance *inst;
    if (!getInstance(instName, inst)) {
        std::cerr << __FUNCTION__ << "() No Instance named: " << instName << std::endl;
        return false;
    }

    if (!isComponentId(inst->getComponentId())) {
        std::cerr << __FUNCTION__ << "() Ilegal Component Id: " << inst->getComponentId() << ", from Instance: " << inst->getName() << std::endl;
        return false;
    }

    auto &comp = getComponent(inst->getComponentId());

    padstack *pad;
    if (!comp.getPadstack(pinName, pad)) {
        std::cerr << __FUNCTION__ << "() No Padstack named: " << pinName << std::endl;
        return false;
    }
    getPinPosition(*pad, *inst, pos);
    return true;
}

bool kicadPcbDataBase::getPinPosition(const int inst_id, const int &pin_id, point_2d *pos) {
    if (!isInstanceId(inst_id)) {
        std::cerr << __FUNCTION__ << "() Ilegal Instance Id: " << inst_id << std::endl;
        return false;
    }

    auto &inst = getInstance(inst_id);
    if (!isComponentId(inst.getComponentId())) {
        std::cerr << __FUNCTION__ << "() Ilegal Component Id: " << inst.getComponentId() << ", from Instance: " << inst.getName() << std::endl;
        return false;
    }

    auto &comp = getComponent(inst.getComponentId());
    if (!comp.isPadstackId(pin_id)) {
        std::cerr << __FUNCTION__ << "() Ilegal Pin(Padstack) Id: " << pin_id << std::endl;
        return false;
    }

    auto &pad = comp.getPadstack(pin_id);
    getPinPosition(pad, inst, pos);
    return true;
}

void kicadPcbDataBase::getPinPosition(const padstack &pad, const instance &inst, point_2d *pos) {
    double padX = pad.m_pos.m_x, padY = pad.m_pos.m_y;
    auto instAngle = inst.m_angle * (-M_PI / 180.0);

    auto s = sin((float)instAngle);
    auto c = cos((float)instAngle);
    pos->m_x = double((c * padX - s * padY) + inst.m_x);
    pos->m_y = double((s * padX + c * padY) + inst.m_y);
}

bool kicadPcbDataBase::getPinPosition(const Pin &p, point_2d *pos) {
    //TODO: range checking
    auto &inst = getInstance(p.m_inst_id);
    auto &comp = getComponent(p.m_comp_id);
    auto &pad = comp.getPadstack(p.m_padstack_id);
    getPinPosition(pad, inst, pos);
    return true;
}

void kicadPcbDataBase::getPinShapeRelativeCoordsToModule(const padstack &pad, const instance &inst, const points_2d &coords, points_2d *coordsRe) {
    double padX = pad.m_pos.m_x, padY = pad.m_pos.m_y;
    auto instAngle = inst.m_angle * (-M_PI / 180.0);

    auto s = sin((float)instAngle);
    auto c = cos((float)instAngle);
    for (auto &&coor : coords) {
        auto p = point_2d{};
        p.m_x = double((c * padX - s * padY) + coor.m_x);
        p.m_y = double((s * padX + c * padY) + coor.m_y);
        coordsRe->push_back(p);
    }
}

bool kicadPcbDataBase::getCompBBox(const int compId, point_2d *bBox) {
    if (!bBox)
        return false;

    if (!isComponentId(compId))
        return false;

    auto &comp = getComponent(compId);
    auto minx = double(1000000.0);
    auto miny = double(1000000.0);
    auto maxx = double(-1000000.0);
    auto maxy = double(-1000000.0);

    for (size_t i = 0; i < comp.m_lines.size(); ++i) {
        auto start = comp.m_lines[i].m_start;
        auto end = comp.m_lines[i].m_end;
        auto width = comp.m_lines[i].m_width;
        minx = std::min(start.m_x - width, minx);
        maxx = std::max(start.m_x + width, maxx);
        minx = std::min(end.m_x - width, minx);
        maxx = std::max(end.m_x + width, maxx);

        miny = std::min(start.m_y - width, miny);
        maxy = std::max(start.m_y + width, maxy);
        miny = std::min(end.m_y - width, miny);
        maxy = std::max(end.m_y + width, maxy);
    }

    for (size_t i = 0; i < comp.m_circles.size(); ++i) {
        auto center = comp.m_circles[i].m_center;
        auto end = comp.m_circles[i].m_end;
        auto width = comp.m_circles[i].m_width;
        minx = std::min(center.m_x - end.m_x - width, minx);
        maxx = std::max(center.m_x + width + end.m_x, maxx);

        miny = std::min(center.m_y - end.m_y - width, miny);
        maxy = std::max(center.m_y + end.m_y + width, maxy);
    }

    /*
    for (size_t i = 0; i < comp.m_polys.size(); ++i)
    {
        for (size_t j = 0; j < comp.m_polys[i].m_shape.size(); ++j)
        {
            auto point = comp.m_polys[i].m_shape[j];
            auto width = comp.m_polys[i].m_width;
            minx = std::min(point.m_x - width, minx);
            maxx = std::max(point.m_x + width, maxx);
            miny = std::min(point.m_y - width, miny);
            maxy = std::max(point.m_y + width, maxy);
        }
    }
    */
    /*
    for (size_t i = 0; i < comp.m_arcs.size(); ++i)
    {
        auto start = comp.m_arcs[i].m_start;
        auto end = comp.m_arcs[i].m_end;
        auto width = comp.m_arcs[i].m_width;
        minx = std::min(start.m_x - width, minx);
        maxx = std::max(start.m_x + width, maxx);
        minx = std::min(end.m_x - width, minx);
        maxx = std::max(end.m_x + width, maxx);

        miny = std::min(start.m_y - width, miny);
        maxy = std::max(start.m_y + width, maxy);
        miny = std::min(end.m_y - width, miny);
        maxy = std::max(end.m_y + width, maxy);
    }
    */

    auto pads = comp.getPadstacks();
    for (auto &pad : pads) {
        auto pad_x = pad.m_pos.m_x;
        auto pad_y = pad.m_pos.m_y;
        auto size = pad.m_size;
        auto pad_angle = pad.m_angle;
        double w = size.m_x;
        double h = size.m_y;

        if (pad_angle == 0 || pad_angle == 180) {
            minx = std::min(pad_x - size.m_x, minx);
            maxx = std::max(pad_x + size.m_x, maxx);

            miny = std::min(pad_y - size.m_y, miny);
            maxy = std::max(pad_y + size.m_y, maxy);
        }

        if (pad_angle == 90 || pad_angle == 270) {
            miny = std::min(pad_y - size.m_x, miny);
            maxy = std::max(pad_y + size.m_x, maxy);

            minx = std::min(pad_x - size.m_y, minx);
            maxx = std::max(pad_x + size.m_y, maxx);
        }
    }

    double width = abs(maxx - minx);
    double height = abs(maxy - miny);
    bBox->m_x = width;
    bBox->m_y = height;

    return true;
}

void kicadPcbDataBase::getPadstackRotatedWidthAndHeight(const instance &inst, const padstack &pad, double &width, double &height) {
    double overallRot = inst.getAngle() + pad.getAngle();
    if ((int)(overallRot / 90.0) % 2 == 0) {
        width = pad.getSize().m_x;
        height = pad.getSize().m_y;
    } else {
        width = pad.getSize().m_y;
        height = pad.getSize().m_x;
    }
}

int kicadPcbDataBase::getLayerId(const std::string &layerName) {
    auto layerIte = layer_to_index_map.find(layerName);
    if (layerIte != layer_to_index_map.end()) {
        return layerIte->second;
    }
    return -1;
}

std::string kicadPcbDataBase::getLayerName(const int layerId) {
    auto layerIte = index_to_layer_map.find(layerId);
    if (layerIte != index_to_layer_map.end()) {
        return layerIte->second;
    }
    return "NO_THIS_LAYER";
}

std::vector<int> kicadPcbDataBase::getPinLayer(const int &instId, const int &padStackId) {
    auto &&inst = instances[instId];
    int compId = inst.getComponentId();
    auto &&comp = components[compId];
    auto &&pad = comp.getPadstack(padStackId);
    std::vector<int> layers;
    if (pad.getType() == padType::SMD) {
        layers.push_back(inst.getLayer());
        return layers;
    } else {
        for (auto &&layer : layer_to_index_map) {
            layers.push_back(layer.second);
        }
        return layers;
    }
}

void kicadPcbDataBase::printKiCad(const std::string folderName, const std::string fileNameStamp) {
    std::string instName;
    for (size_t i = 0; i < tree.m_branches.size(); ++i) {
        auto &sub_node = tree.m_branches[i];
        if (sub_node.m_value == "module") {
            for (auto &&module_node : sub_node.m_branches) {
                if (module_node.m_value == "fp_text" && module_node.m_branches[0].m_value == "reference") {
                    instName = module_node.m_branches[1].m_value;
                }
            }

            instance *inst;
            getInstance(instName, inst);

            if (sub_node.m_branches[1].m_value == "locked") {
                int l = inst->getLayer();
                std::string layer = getLayerName(l);
                sub_node.m_branches[2].m_branches[0].m_value = layer;
                sub_node.m_branches[5].m_branches[0].m_value = std::to_string(inst->getX());
                sub_node.m_branches[5].m_branches[1].m_value = std::to_string(inst->getY());
                if (int(sub_node.m_branches[5].m_branches.size()) == 3) {
                    if (inst->getAngle() == 0)
                        sub_node.m_branches[5].m_branches[2].m_value = "";
                    else
                        sub_node.m_branches[5].m_branches[2].m_value = std::to_string(inst->getAngle());
                } else {
                    if (inst->getAngle() != 0) {
                        auto t = Tree{std::to_string(inst->getAngle()), {}};
                        sub_node.m_branches[5].m_branches.push_back(t);
                    }
                }
            } else {
                int l = inst->getLayer();
                std::string layer = getLayerName(l);
                sub_node.m_branches[1].m_branches[0].m_value = layer;
                sub_node.m_branches[4].m_branches[0].m_value = std::to_string(inst->getX());
                sub_node.m_branches[4].m_branches[1].m_value = std::to_string(inst->getY());
                if (int(sub_node.m_branches[4].m_branches.size()) == 3) {
                    if (inst->getAngle() == 0)
                        sub_node.m_branches[4].m_branches[2].m_value = "";
                    else
                        sub_node.m_branches[4].m_branches[2].m_value = std::to_string(inst->getAngle());
                } else {
                    if (inst->getAngle() != 0) {
                        auto t = Tree{std::to_string(inst->getAngle()), {}};
                        sub_node.m_branches[4].m_branches.push_back(t);
                    }
                }
            }
        }

        if (sub_node.m_value == "segment" || sub_node.m_value == "via") {
            tree.m_branches.erase(tree.m_branches.begin() + i);
            --i;
        }
    }

    int num = 0;
    for (auto &&drc : clearanceDrcs) {
        auto &&obj1 = drc.first;
        auto &&obj2 = drc.second;
        polygon_t poly1 = obj1.getPoly();
        polygon_t poly2 = obj2.getPoly();
        std::deque<polygon_t> output;
        boost::geometry::intersection(poly1, poly2, output);

        auto &p = output.front();
        //BOOST_FOREACH (polygon_t const &p, output)
        // {
        int i = 0;
        double x = 0.0, y = 0.0;
        for (auto it = boost::begin(boost::geometry::exterior_ring(p)); it != boost::end(boost::geometry::exterior_ring(p)); ++it) {
            x += bg::get<0>(*it);
            y += bg::get<1>(*it);
            std::cout << "\t" << bg::get<0>(*it) << ", " << bg::get<1>(*it) << std::endl;
            if (i == 3)
                break;
            ++i;
        }
        x = x / 4;
        y = y / 4;

        std::cout << num << ":   x: " << x << ", y: " << y << ", area: " << boost::geometry::area(p) << std::endl;
        auto size = Tree{"size", {}};
        size.m_branches.push_back(Tree{"0.3", {}});
        size.m_branches.push_back(Tree{"0.3", {}});
        auto thickness = Tree{"thickness", {}};
        thickness.m_branches.push_back(Tree{"0.05", {}});
        auto font = Tree{"font", {}};
        font.m_branches.push_back(size);
        font.m_branches.push_back(thickness);
        auto effects = Tree{"effects", {}};
        effects.m_branches.push_back(font);
        auto layer = Tree{"layer", {}};
        layer.m_branches.push_back(Tree{"Dwgs.User", {}});
        auto at = Tree{"at", {}};
        at.m_branches.push_back(Tree{std::to_string(x), {}});
        at.m_branches.push_back(Tree{std::to_string(y), {}});

        auto gr = Tree{"gr_text", {}};
        gr.m_branches.push_back(Tree{std::to_string(num), {}});
        ++num;
        gr.m_branches.push_back(at);
        gr.m_branches.push_back(layer);
        gr.m_branches.push_back(effects);
        // }

        tree.m_branches.push_back(gr);
    }

    for (auto &net : nets) {
        for (auto &segment : net.m_segments) {
            points_2d p = segment.getPos();
            auto seg = Tree{"segment", {}};
            auto start = Tree{"start", {}};
            start.m_branches.push_back(Tree{std::to_string(p[0].m_x), {}});
            start.m_branches.push_back(Tree{std::to_string(p[0].m_y), {}});
            auto end = Tree{"end", {}};
            end.m_branches.push_back(Tree{std::to_string(p[1].m_x), {}});
            end.m_branches.push_back(Tree{std::to_string(p[1].m_y), {}});
            auto width = Tree{"width", {}};
            width.m_branches.push_back(Tree{std::to_string(segment.getWidth()), {}});
            auto layer = Tree{"layer", {}};
            layer.m_branches.push_back(Tree{segment.getLayer(), {}});
            auto n = Tree{"net", {}};
            n.m_branches.push_back(Tree{std::to_string(net.getId()), {}});
            //auto tstamp = Tree {"tstamp", {}};
            //tstamp.m_branches.push_back(Tree{"1C6BE40", {}});

            seg.m_branches.push_back(start);
            seg.m_branches.push_back(end);
            seg.m_branches.push_back(width);
            seg.m_branches.push_back(layer);
            seg.m_branches.push_back(n);
            //seg.m_branches.push_back(tstamp);
            tree.m_branches.push_back(seg);
        }

        for (auto &via : net.m_vias) {
            point_2d p = via.getPos();

            auto v = Tree{"via", {}};
            auto at = Tree{"at", {}};
            at.m_branches.push_back(Tree{std::to_string(p.m_x), {}});
            at.m_branches.push_back(Tree{std::to_string(p.m_y), {}});

            auto size = Tree{"size", {}};
            size.m_branches.push_back(Tree{std::to_string(via.getSize()), {}});
            //auto drill = Tree{"drill", {}};
            //drill.m_branches.push_back(Tree{"0.3937", {}});
            auto layer = Tree{"layers", {}};
            std::vector<std::string> layers = via.getLayers();
            layer.m_branches.push_back(Tree{layers[0], {}});
            layer.m_branches.push_back(Tree{layers[1], {}});
            auto n = Tree{"net", {}};
            n.m_branches.push_back(Tree{std::to_string(net.getId()), {}});

            v.m_branches.push_back(at);
            v.m_branches.push_back(size);
            //v.m_branches.push_back(drill);
            v.m_branches.push_back(layer);
            v.m_branches.push_back(n);
            tree.m_branches.push_back(v);
        }
    }

    // Handle output filename
    std::string fileExtension = utilParser::getFileExtension(m_fileName);
    std::string fileNameWoExtension = utilParser::getFileNameWoExtension(m_fileName);
    std::string fileNameExtraTag;
    if (!fileNameStamp.empty()) {
        fileNameExtraTag = "output." + fileNameStamp;
    } else {
        fileNameExtraTag = "output";
    }
    std::string outputFileName = fileNameExtraTag + "." + fileNameWoExtension + "." + fileExtension;
    outputFileName = utilParser::appendDirectory(folderName, outputFileName);
    std::cout << __FUNCTION__ << "() outputFileName: " << outputFileName << std::endl;

    kicadParser writer(outputFileName);
    writer.writeKicadPcb(tree);
}

void kicadPcbDataBase::addClearanceDrc(Object &obj1, Object &obj2) {
    int id1 = obj1.getId(), id2 = obj2.getId();
    for (auto &&drc : clearanceDrcs) {
        if (drc.first.getId() == id1 && drc.second.getId() == id2)
            return;
        else if (drc.first.getId() == id2 && drc.second.getId() == id1)
            return;
    }
    clearanceDrcs.push_back(std::make_pair(obj1, obj2));
}

void kicadPcbDataBase::printClearanceDrc() {
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "###           DRC INFO            ###" << std::endl;
    std::cout << "###                               ###" << std::endl;
    std::cout << "#####################################" << std::endl;
    int num = 0;
    for (auto &&drc : clearanceDrcs) {
        Object &obj1 = drc.first;
        Object &obj2 = drc.second;
        std::cout << "----------CONFLICT " << num << "---------- " << std::endl;
        std::cout << "obj1 id: " << obj1.getId() << ", obj2 id: " << obj2.getId() << std::endl;
        if (obj1.getType() == ObjectType::PIN) {
            auto compId = obj1.getCompId();
            auto instId = obj1.getInstId();
            auto padId = obj1.getDBId();
            component &comp = getComponent(compId);
            instance &inst = getInstance(instId);
            auto &pad = comp.getPadstack(padId);
            std::cout << "Component: " << comp.getName() << " Instance: " << inst.getName();
            std::cout << " Pad: " << pad.getName() << std::endl;
        } else if (obj1.getType() == ObjectType::SEGMENT) {
            auto dbId = obj1.getDBId();
            points_2d pos = obj1.getPos();
            std::cout << "segment: ";
            for (auto &&p : pos) {
                std::cout << "(" << p.m_x << "," << p.m_y << ") ";
            }
            std::cout << std::endl;
        } else if (obj1.getType() == ObjectType::VIA) {
            auto dbId = obj1.getDBId();
            points_2d pos = obj1.getPos();
            std::cout << "via: (" << pos[0].m_x << "," << pos[0].m_y << ")" << std::endl;
        }

        if (obj2.getType() == ObjectType::PIN) {
            auto compId = obj2.getCompId();
            auto instId = obj2.getInstId();
            auto padId = obj2.getDBId();
            component &comp = getComponent(compId);
            instance &inst = getInstance(instId);
            auto &pad = comp.getPadstack(padId);
            std::cout << "Component: " << comp.getName() << " Instance: " << inst.getName();
            std::cout << " Pad: " << pad.getName() << std::endl;
        }

        else if (obj2.getType() == ObjectType::SEGMENT) {
            auto dbId = obj2.getDBId();
            points_2d pos = obj2.getPos();
            std::cout << "segment: ";
            for (auto &&p : pos) {
                std::cout << "(" << p.m_x << "," << p.m_y << ") ";
            }
            std::cout << std::endl;
        } else if (obj2.getType() == ObjectType::VIA) {
            auto dbId = obj2.getDBId();
            points_2d pos = obj2.getPos();
            std::cout << "via: (" << pos[0].m_x << "," << pos[0].m_y << ")" << std::endl;
        }
        ++num;
    }

    std::cout << "##########SUMMARY##########" << std::endl;
    std::cout << "DRC Count: " << clearanceDrcs.size() << std::endl;
}
