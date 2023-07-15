#include "json_reader.h"

using namespace::std::literals::string_view_literals;

namespace tc_project::json_reader {

    Document LoadJSON(const std::string &s) {
        std::istringstream strm(s);
        return json::Load(strm);
    }

    void RequestsProcessing(transport_catalogue::TransportCatalogue &catalogue, map_renderer::MapRenderer& renderer, std::istream& input, std::ostream& output) {
        std::vector<Dict> base_requests;
        std::vector<Dict> stat_requests;
        Dict render_settings;
        auto json_data = json::Load(input);
        for (const auto &[key, value]: json_data.GetRoot().AsMap()) {
            if (key == "base_requests"sv) {
                for (const auto &base_data: value.AsArray()) {
                    base_requests.push_back(base_data.AsMap());
                }
            } else if (key == "stat_requests"sv) {
                for (const auto &stat_data: value.AsArray()) {
                    stat_requests.push_back(stat_data.AsMap());
                }
            } else if (key == "render_settings"sv) {
                render_settings = value.AsMap();
            }
        }
        BasesProcessing(catalogue, std::move(base_requests));
        RenderProcessing(renderer, std::move(render_settings));
        StatProcessing(catalogue, renderer, std::move(stat_requests), output);
    }

    void RenderProcessing(map_renderer::MapRenderer& renderer, Dict&&  json_render_settings) {
        map_renderer::RenderSettings settings;
        for(const auto& [param, data] : json_render_settings) {
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
                ParseColor(data, settings);
            } else if(param == "underlayer_width"sv) {
                settings.underlayer_width = data.AsDouble();
            } else if(param == "color_palette"sv) {
                for(const auto& color : data.AsArray()) {
                    ParseColor(color, settings);
                }
            }
        }
        renderer = map_renderer::MapRenderer{std::move(settings)};
    }

    void ParseColor(const Node& color, map_renderer::RenderSettings& settings) {
        if(color.IsString()){
            settings.color_palette.emplace_back(color.AsString());
        } else if(color.IsArray()) {
            if(color.AsArray().size() == 3) {
                settings.color_palette.emplace_back(svg::Rgb {
                        static_cast<uint8_t>(color.AsArray()[0].AsInt()),
                        static_cast<uint8_t>(color.AsArray()[1].AsInt()),
                        static_cast<uint8_t>(color.AsArray()[2].AsInt())});
            }
            else if(color.AsArray().size() == 4) {
                settings.color_palette.emplace_back(svg::Rgba {
                        static_cast<uint8_t>(color.AsArray()[0].AsInt()),
                        static_cast<uint8_t>(color.AsArray()[1].AsInt()),
                        static_cast<uint8_t>(color.AsArray()[2].AsInt()),
                        color.AsArray()[3].AsDouble()});
            }
        }
    }

    void BasesProcessing(transport_catalogue::TransportCatalogue &catalogue, std::vector<Dict> &&base_requests) {
        std::map<std::string , std::map<std::string, Node>> road_distances;
        for (const auto &stop_data: base_requests) {
            for (const auto &[key, value]: stop_data) {
                if (key == "type"sv && value.AsString() == "Stop"sv) {
                    StopProcessing(catalogue, stop_data, road_distances);
                }
            }
        }
        for(const auto& data : road_distances) {
             for(const auto& [name, dist] : data.second) {
                 catalogue.SetDistance(data.first, name, dist.AsInt());
             }
        }
        for (const auto &bus_data: base_requests) {
            for (const auto &[key, value]: bus_data) {
                if (key == "type"sv && value.AsString() == "Bus"sv) {
                    BusProcessing(catalogue, bus_data);
                }
            }
        }
    }

    void StopProcessing(transport_catalogue::TransportCatalogue &catalogue, const Dict &stop, std::map<std::string, std::map<std::string, Node>>& road_distances) {
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
                dist = value.AsMap();
            }
        }
        road_distances[name] = dist;
        catalogue.AddStop(name, latitude, longitude);
    }

    void BusProcessing(transport_catalogue::TransportCatalogue &catalogue, const Dict &bus) {
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
        catalogue.AddBus(name, stops, is_roundtrip);
    }

    void StatProcessing(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, std::vector<Dict> &&stat_requests, std::ostream& output) {
        std::string json_out;
        std::vector<std::string> parse_data;
        if(stat_requests.empty()) {
            return;
        }
        for (const auto &data: stat_requests) {
            std::string type;
            std::string name;
            int id = 0;
            for (const auto &[key, value]: data) {
                if (key == "id"sv) {
                    id = value.AsInt();
                } else if (key == "type"sv) {
                    type = value.AsString();
                } else if (key == "name"sv) {
                    name = value.AsString();
                }
            }
            if (type == "Bus") {
                ParseBus(catalogue, renderer, name, id, parse_data);
            } else if (type == "Stop") {
                ParseStop(catalogue, renderer, name, id, parse_data);
            } else if (type == "Map") {
                ParseMap(catalogue, renderer, id, parse_data);
            }
        }
        json_out += "[\n";
        for (const auto &out: parse_data) {
            json_out += out + ',';
        }
        json_out.erase(json_out.size() - 1);
        json_out += "\n]";
        Print(LoadJSON(json_out), output);
    }

    void ParseBus(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, const std::string& name, const int id, std::vector<std::string>& parse_data) {
        std::string json_bus;
        request_handler::RequestHandler handler(catalogue, renderer);
        auto info = handler.GetBusInfo(name);
        if(!info) {
            json_bus = "{\n\"request_id\": " + std::to_string(id) + ",\n\"error_message\": \"not found\"\n}";
        } else {
            json_bus =
                    "{\n\"request_id\": " + std::to_string(id)   + ",\n\"curvature\": " + std::to_string(info->curvature) + ",\n\"route_length\": " +
                    std::to_string(info->distance)
                    + ",\n\"stop_count\": " + std::to_string(info->stops_count) + ",\n\"unique_stop_count\": " +
                    std::to_string(info->unique_stops_count) + "\n}";
        }
        parse_data.push_back(json_bus);
    }

    void ParseStop(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, const std::string& name, const int id, std::vector<std::string>& parse_data) {
        std::string json_stop;
        request_handler::RequestHandler handler(catalogue, renderer);
        auto info = handler.GetBusesByStop(name);
        if(info == std::nullopt) {
            json_stop = "{\n\"request_id\": " + std::to_string(id)  + ",\n\"error_message\": \"not found\"\n}";
        } else if(info == nullptr) {
            json_stop = "{\n\"request_id\": " + std::to_string(id)  + ",\n\"buses\": [\t]\n}";
        } else {
            const auto& buses = *info.value();
            std::vector<const Bus*> sorted_buses(buses.begin(), buses.end());
            std::sort(sorted_buses.begin(), sorted_buses.end(),
                      [](const Bus* a, const Bus* b) { return a->name < b->name; });
            json_stop = "{\n\"request_id\": " + std::to_string(id)  + ",\n\"buses\": [";
            for (const auto& bus : sorted_buses) {
                json_stop += " \"" + bus->name + "\"";
            }
            json_stop += "\n]\n}";
        }
        parse_data.push_back(json_stop);
    }

    std::string Shielding(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '\"') {
                result += "\\\"";
            } else if (c == '\n') {
                result += "\\n";
            } else if (c == '\\') {
                result += "\\\\";
            } else if (c == '\r') {
                result += "\\r";
            } else {
                result.push_back(c);
            }
        }
        return result;
    }

    void ParseMap(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, int id, std::vector<std::string>& parse_data) {
        std::string json_map;
        request_handler::RequestHandler handler(catalogue, renderer);
        auto doc = handler.RenderMap();
        std::ostringstream map;
        doc.Render(map);
        json_map = "{\n\"request_id\": " + std::to_string(id) + ",\n\"map\": \"" + Shielding(map.str()) + "\"\n}";
        parse_data.push_back(json_map);
    }
}
