#include "json_reader.h"

using namespace::std::literals::string_view_literals;

namespace tc_project::json_reader {

    void JsonReader::RequestsProcessing(std::istream& input, std::ostream& output) {
        auto json_data = json::Load(input);
        for (const auto &[key, value]: json_data.GetRoot().AsDict()) {
            if (key == "base_requests"sv) {
                for (const auto &base_data: value.AsArray()) {
                    base_requests_.push_back(base_data.AsDict());
                }
            } else if (key == "stat_requests"sv) {
                for (const auto &stat_data: value.AsArray()) {
                    stat_requests_.push_back(stat_data.AsDict());
                }
            } else if (key == "render_settings"sv) {
                json_render_settings_ = value.AsDict();
            } else if (key == "routing_settings"sv) {
                auto settings = value.AsDict();
                for(const auto& [param, data] : settings) {
                    if(param == "bus_wait_time"sv) {
                        transport_router_.SetBusWaitTime(data.AsInt());
                    } else if(param == "bus_velocity"sv) {
                        transport_router_.SetBusVelocity(data.AsDouble());
                    }
                }
            }
        }
        BasesProcessing();
        RenderProcessing();
        StatProcessing(output);
    }

    void JsonReader::BasesProcessing() {
        for (const auto &stop_data: base_requests_) {
            for (const auto &[key, value]: stop_data) {
                if (key == "type"sv && value.AsString() == "Stop"sv) {
                    StopProcessing(stop_data);
                }
            }
        }
        for(const auto& data : road_distances_) {
            for(const auto& [name, dist] : data.second) {
                catalogue_.SetDistance(data.first, name, dist.AsInt());
            }
        }
        for (const auto &bus_data: base_requests_) {
            for (const auto &[key, value]: bus_data) {
                if (key == "type"sv && value.AsString() == "Bus"sv) {
                    BusProcessing(bus_data);
                }
            }
        }
    }

    void JsonReader::RenderProcessing() {
        map_renderer::RenderSettings settings;
        for(const auto& [param, data] : json_render_settings_) {
            if(param == "width"sv) {
                settings.width = data.AsDouble();
            } else if(param == "height"sv) {
                settings.height = data.AsDouble();
            } else if(param == "padding"sv) {
                settings.padding = data.AsDouble();
            } else if(param == "line_width"sv) {
                settings.line_width = data.AsDouble();
            } else if(param == "stop_radius"sv) {
                settings.stop_radius = data.AsDouble();
            } else if(param == "bus_label_font_size"sv) {
                settings.bus_label_font_size = data.AsInt();
            } else if(param == "bus_label_offset"sv) {
                for(const auto& coord : data.AsArray()) {
                    settings.bus_label_offset.push_back(coord.AsDouble());
                }
            } else if(param == "stop_label_font_size"sv) {
                settings.stop_label_font_size = data.AsInt();
            } else if(param == "stop_label_offset"sv) {
                for(const auto& coord : data.AsArray()) {
                    settings.stop_label_offset.push_back(coord.AsDouble());
                }
            } else if(param == "underlayer_color"sv) {
                if(data.IsString()){
                    settings.underlayer_color = data.AsString();
                } else if(data.IsArray()) {
                    if(data.AsArray().size() == 3) {
                        settings.underlayer_color = svg::Rgb{
                                static_cast<uint8_t>(data.AsArray()[0].AsInt()),
                                static_cast<uint8_t>(data.AsArray()[1].AsInt()),
                                static_cast<uint8_t>(data.AsArray()[2].AsInt())};
                    }
                    else if(data.AsArray().size() == 4) {
                        settings.underlayer_color = svg::Rgba{
                                static_cast<uint8_t>(data.AsArray()[0].AsInt()),
                                static_cast<uint8_t>(data.AsArray()[1].AsInt()),
                                static_cast<uint8_t>(data.AsArray()[2].AsInt()),
                                data.AsArray()[3].AsDouble()};
                    }
                }
            } else if(param == "underlayer_width"sv) {
                settings.underlayer_width = data.AsDouble();
            } else if(param == "color_palette"sv) {
                for(const auto& color : data.AsArray()) {
                    if(color.IsString()){
                        settings.color_palette.emplace_back(color.AsString());
                    } else if(color.IsArray()) {
                        if(color.AsArray().size() == 3) {
                            settings.color_palette.emplace_back(svg::Rgb{
                                    static_cast<uint8_t>(color.AsArray()[0].AsInt()),
                                    static_cast<uint8_t>(color.AsArray()[1].AsInt()),
                                    static_cast<uint8_t>(color.AsArray()[2].AsInt())});
                        }
                        else if(color.AsArray().size() == 4) {
                            settings.color_palette.emplace_back(svg::Rgba{
                                    static_cast<uint8_t>(color.AsArray()[0].AsInt()),
                                    static_cast<uint8_t>(color.AsArray()[1].AsInt()),
                                    static_cast<uint8_t>(color.AsArray()[2].AsInt()),
                                    color.AsArray()[3].AsDouble()});
                        }
                    }
                }
            }
        }
        renderer_.SetSettings(settings);
    }

    void JsonReader::StopProcessing(const Dict& stop) {
        std::string name{};
        double latitude = 0.0;
        double longitude = 0.0;
        std::map<std::string, Node> dist;
        for (const auto& [key, value] : stop) {
            if (key == "name"sv) {
                name = value.AsString();
            } else if (key == "latitude"sv) {
                latitude = value.AsDouble();
            } else if (key == "longitude"sv) {
                longitude = value.AsDouble();
            } else if (key == "road_distances") {
                dist = value.AsDict();
            }
        }
        road_distances_[name] = dist;
        catalogue_.AddStop(name, latitude, longitude);
    }

    void JsonReader::BusProcessing(const Dict& bus) {
        std::string name{};
        bool is_roundtrip = false;
        std::vector<std::string> stops{};
        for (const auto &[key, value]: bus) {
            if (key == "is_roundtrip"sv) {
                is_roundtrip = value.AsBool();
            } else if (key == "name"sv) {
                name = value.AsString();
            } else if (key == "stops"sv) {
                for (const auto &stop: value.AsArray()) {
                    stops.push_back(stop.AsString());
                }
            }
        }
        catalogue_.AddBus(name, stops, is_roundtrip);
    }

    void JsonReader::StatProcessing(std::ostream& output) {
        Node json;
        std::vector<json::Node> builder_data;
        transport_router_.BuildGraph(catalogue_, catalogue_.GetIndexStops().size());
        graph::Router router(transport_router_.GetGraph());
        if(stat_requests_.empty()) {
            return;
        }
        for (const auto &data: stat_requests_) {
            StatSettings settings;
            for (const auto &[key, value]: data) {
                if (key == "id"sv) {
                    settings.id = value.AsInt();
                } else if (key == "type"sv) {
                    settings.type = value.AsString();
                } else if (key == "name"sv) {
                    settings.name = value.AsString();
                } else if (key == "from"sv) {
                    settings.from = value.AsString();
                } else if (key == "to"sv) {
                    settings.to = value.AsString();
                }
            }
            if (settings.type == "Bus"sv) {
                ParseBus(settings.name, settings.id);
            } else if (settings.type == "Stop"sv) {
                ParseStop(settings.name, settings.id);
            } else if (settings.type == "Map"sv) {
                ParseMap(settings.id);
            } else if (settings.type == "Route"sv) {
                ParseRoute(router, settings.id, settings.from, settings.to);
            }
        }
        auto array_builder = json::Builder{};
        array_builder.StartArray();
        for(const auto& data : builder_data_) {
            array_builder.Value(data.AsDict());
        }
        json = array_builder.EndArray().Build();
        json::Print(json::Document{json}, output);
    }

    void JsonReader::ParseBus(const std::string& name, int id) {
        Node json_bus_builder;
        auto info = handler_.GetBusInfo(name);
        if(!info) {
            json_bus_builder = json::Builder{}.StartDict().Key("request_id").Value(id)
                    .Key("error_message").Value("not found").EndDict().Build();
        } else {
            json_bus_builder = json::Builder{}.StartDict().Key("request_id").Value(id)
                    .Key("curvature").Value(info->curvature)
                    .Key("route_length").Value(info->distance)
                    .Key("stop_count").Value(info->stops_count)
                    .Key("unique_stop_count").Value(info->unique_stops_count).EndDict().Build();
        }
        builder_data_.push_back(json_bus_builder);
    }

    void JsonReader::ParseStop(const std::string& name, int id) {
        Node json_stop_builder;
        auto info = handler_.GetBusesByStop(name);
        if(info == std::nullopt) {
            json_stop_builder = json::Builder{}.StartDict().Key("request_id").Value(id)
                    .Key("error_message").Value("not found").EndDict().Build();
        } else if(info == nullptr) {
            json_stop_builder = json::Builder{}.StartDict().Key("request_id").Value(id)
                    .Key("buses").StartArray().EndArray().EndDict().Build();
        } else {
            const auto& buses = *info.value();
            std::vector<const Bus*> sorted_buses(buses.begin(), buses.end());
            std::sort(sorted_buses.begin(), sorted_buses.end(),
                      [](const Bus* a, const Bus* b) { return a->name < b->name; });
            auto array_builder = json::Builder{};
            array_builder.StartDict().Key("request_id").Value(id)
                    .Key("buses").StartArray();
            for (const auto& bus : sorted_buses) {
                array_builder.Value(bus->name);
            }
            json_stop_builder = array_builder.EndArray().EndDict().Build();
        }
        builder_data_.push_back(json_stop_builder);
    }

    void JsonReader::ParseMap(int id) {
        Node json_map_builder;
        auto doc = handler_.RenderMap();
        std::ostringstream map;
        doc.Render(map);
        json_map_builder = json::Builder{}.StartDict().Key("request_id").Value(id)
                .Key("map").Value(map.str()).EndDict().Build();
        builder_data_.push_back(json_map_builder);
    }

    void JsonReader::ParseRoute(const graph::Router<RouteWeight>& router, int id, const std::string& from, const std::string& to) {
        Node json_route_builder;
        auto route_builder = router.BuildRoute(transport_router_.GetVertex(from), transport_router_.GetVertex(to));
        if(route_builder == std::nullopt) {
            json_route_builder = json::Builder{}.StartDict().Key("request_id").Value(id)
                    .Key("error_message").Value("not found").EndDict().Build();
        } else {
            auto array_builder = json::Builder{};
            array_builder.StartDict().Key("request_id").Value(id).Key("total_time").Value(route_builder->weight.weight)
                    .Key("items").StartArray();
            for (const auto &edge: route_builder->edges) {
                auto item = transport_router_.GetGraph().GetEdge(edge).weight;
                if(item.is_waiting) {
                    array_builder.StartDict().Key("type").Value("Wait").Key("time").Value(item.weight).Key("stop_name").Value(std::string(item.name)).EndDict();
                } else {
                    array_builder.StartDict().Key("type").Value("Bus").Key("time").Value(item.weight).Key("bus").Value(std::string(item.name)).Key("span_count").Value(item.span_count).EndDict();
                }
            }
            json_route_builder = array_builder.EndArray().EndDict().Build();
        }
        builder_data_.push_back(json_route_builder);
    }
}