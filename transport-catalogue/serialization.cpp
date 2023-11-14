#include "serialization.h"

using namespace::std::literals::string_view_literals;

namespace tc_project {

    void Serialization::BaseSerialization(const std::filesystem::path& path, const transport_catalogue::TransportCatalogue& transport_catalogue, const map_renderer::MapRenderer& renderer, const transport_router::TransportRouter& transport_router) {
        std::ofstream output_file(path, std::ios::binary);
        proto_transport_catalogue::ProtoData result;
        *result.mutable_transport_catalogue() = CatalogueSerialization(transport_catalogue);
        *result.mutable_render_settings() = MapSerialization(renderer);
        *result.mutable_router() = RoutingSerialization(transport_router);
        result.SerializeToOstream(&output_file);
    }

    proto_transport_catalogue::TransportCatalogue Serialization::CatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue) {
        proto_transport_catalogue::TransportCatalogue proto_transport_catalogue;
        for(const auto& [_, stop_ptr] : transport_catalogue.GetIndexStops()) {
            proto_transport_catalogue::Stop proto_stop;
            proto_stop.set_name(stop_ptr->name);
            proto_stop.set_latitude(stop_ptr->coordinates.lat);
            proto_stop.set_longitude(stop_ptr->coordinates.lng);
            proto_transport_catalogue.mutable_stops()->Add(std::move(proto_stop));
        }
        for (const auto &[_, bus_ptr]: transport_catalogue.GetIndexRoutes()) {
            proto_transport_catalogue::Bus proto_bus;
            proto_bus.set_name(bus_ptr->name);
            proto_bus.set_is_roudtrip(bus_ptr->is_roundtrip);
            for (const auto& stop_ptr: bus_ptr->stops) {
                proto_transport_catalogue::Stop proto_stop;
                proto_stop.set_name(stop_ptr->name);
                proto_stop.set_latitude(stop_ptr->coordinates.lat);
                proto_stop.set_longitude(stop_ptr->coordinates.lng);
                proto_bus.mutable_stops()->Add(std::move(proto_stop));
            }
            proto_transport_catalogue.mutable_buses()->Add(std::move(proto_bus));
        }
        for(const auto& [stop, dist] : transport_catalogue.GetIndexDistance()) {
            proto_transport_catalogue::Distance proto_distance;
            proto_distance.set_from(stop.first->name);
            proto_distance.set_to(stop.second->name);
            proto_distance.set_distance(dist);
            proto_transport_catalogue.mutable_distances()->Add(std::move(proto_distance));
        }
        return proto_transport_catalogue;
    }

    proto_map_renderer::RenderSettings Serialization::MapSerialization(const map_renderer::MapRenderer& renderer) {
        auto settings = renderer.GetSettings();
        proto_map_renderer::RenderSettings proto_settings;
        proto_settings.set_height(settings.height);
        proto_settings.set_width(settings.width);
        proto_settings.set_padding(settings.padding);
        proto_settings.set_line_width(settings.line_width);
        proto_settings.set_stop_radius(settings.stop_radius);
        proto_settings.set_bus_label_font_size(settings.bus_label_font_size);
        for(const auto& val : settings.bus_label_offset) {
            proto_settings.mutable_bus_label_offset()->Add(val);
        }
        proto_settings.set_stop_label_font_size(settings.stop_label_font_size);
        for(const auto& val : settings.stop_label_offset) {
            proto_settings.mutable_stop_label_offset()->Add(val);
        }
        if(std::holds_alternative<std::string>(settings.underlayer_color)) {
            proto_settings.mutable_underlayer_color()->set_color_string(std::get<std::string>(settings.underlayer_color));
        } else if(std::holds_alternative<svg::Rgb>(settings.underlayer_color)) {
            proto_svg::Color rgb_color;
            rgb_color.mutable_rgb_color()->set_red(std::get<svg::Rgb>(settings.underlayer_color).red);
            rgb_color.mutable_rgb_color()->set_green(std::get<svg::Rgb>(settings.underlayer_color).green);
            rgb_color.mutable_rgb_color()->set_blue(std::get<svg::Rgb>(settings.underlayer_color).blue);
            *proto_settings.mutable_underlayer_color() = rgb_color;
        } else if(std::holds_alternative<svg::Rgba>(settings.underlayer_color)) {
            proto_svg::Color rgba_color;
            rgba_color.mutable_rgba_color()->set_red(std::get<svg::Rgba>(settings.underlayer_color).red);
            rgba_color.mutable_rgba_color()->set_green(std::get<svg::Rgba>(settings.underlayer_color).green);
            rgba_color.mutable_rgba_color()->set_blue(std::get<svg::Rgba>(settings.underlayer_color).blue);
            rgba_color.mutable_rgba_color()->set_opacity(std::get<svg::Rgba>(settings.underlayer_color).opacity);
            *proto_settings.mutable_underlayer_color() = rgba_color;
        }
        proto_settings.set_underlayer_width(settings.underlayer_width);
        for(const auto& color : settings.color_palette) {
            if(std::holds_alternative<std::string>(color)) {
                proto_settings.mutable_color_palette()->Add()->set_color_string(std::get<std::string>(color));
            } else if(std::holds_alternative<svg::Rgb>(color)) {
                proto_svg::Color rgb_color;
                rgb_color.mutable_rgb_color()->set_red(std::get<svg::Rgb>(color).red);
                rgb_color.mutable_rgb_color()->set_green(std::get<svg::Rgb>(color).green);
                rgb_color.mutable_rgb_color()->set_blue(std::get<svg::Rgb>(color).blue);
                proto_settings.mutable_color_palette()->Add(std::move(rgb_color));
            } else if(std::holds_alternative<svg::Rgba>(color)) {
                proto_svg::Color rgba_color;
                rgba_color.mutable_rgba_color()->set_red(std::get<svg::Rgba>(color).red);
                rgba_color.mutable_rgba_color()->set_green(std::get<svg::Rgba>(color).green);
                rgba_color.mutable_rgba_color()->set_blue(std::get<svg::Rgba>(color).blue);
                rgba_color.mutable_rgba_color()->set_opacity(std::get<svg::Rgba>(color).opacity);
                proto_settings.mutable_color_palette()->Add(std::move(rgba_color));
            }
        }
        return proto_settings;
    }

    proto_transport_router::Router Serialization::RoutingSerialization(const transport_router::TransportRouter& transport_router) {
        proto_transport_router::RoutingSettings proto_settings;
        proto_settings.set_bus_wait_time(transport_router.GetBusWaitTime());
        proto_settings.set_bus_velocity(transport_router.GetBusVelocity());
        proto_graph::Graph proto_graph;
        for(const auto& edge : transport_router.GetGraph().GetEdges()) {
            proto_graph::Edge proto_edge;
            proto_edge.set_to(static_cast<int>(edge.to));
            proto_edge.set_from(static_cast<int>(edge.from));
            proto_graph::RouterWeight proto_weight;
            proto_weight.set_weight(edge.weight.weight);
            proto_weight.set_span_count(edge.weight.span_count);
            proto_weight.set_is_waiting(edge.weight.is_waiting);
            proto_weight.set_name(std::string(edge.weight.name));
            *proto_edge.mutable_weight() = proto_weight;
            proto_graph.mutable_edges()->Add(std::move(proto_edge));
        }
        proto_transport_router::Router proto_router;
        *proto_router.mutable_routing_settings() = proto_settings;
        *proto_router.mutable_graph() = proto_graph;
        return proto_router;
    }

    transport_catalogue::TransportCatalogue Serialization::CatalogueDeserialization(const std::filesystem::path& path) {
        proto_transport_catalogue::ProtoData proto_data;
        std::ifstream input_file(path, std::ios::binary);
        if (!proto_data.ParseFromIstream(&input_file)) {
            throw std::runtime_error("Failed to deserialize.");
        }
        transport_catalogue::TransportCatalogue transport_catalogue;
        for(const auto& tc_stop : proto_data.transport_catalogue().stops()) {
            transport_catalogue.AddStop(tc_stop.name(), tc_stop.latitude(), tc_stop.longitude());
        }
        for(const auto& tc_bus : proto_data.transport_catalogue().buses()) {
            std::vector<std::string> stops;
            if(tc_bus.is_roudtrip()) {
                for(const auto& stop : tc_bus.stops()) {
                    stops.push_back(stop.name());
                }
            } else {
                for(auto it = tc_bus.stops().begin(); it != std::next(tc_bus.stops().begin(), (tc_bus.stops().size() / 2) + 1); ++it) {
                    stops.push_back((*it).name());
                }
            }
            transport_catalogue.AddBus(tc_bus.name(), stops, tc_bus.is_roudtrip());
        }
        for(const auto& data : proto_data.transport_catalogue().distances()) {
            transport_catalogue.SetDistance(data.from(), data.to(), data.distance());
        }
        return transport_catalogue;
    }

    map_renderer::MapRenderer Serialization::MapDeserialization(const std::filesystem::path& path) {
        proto_transport_catalogue::ProtoData proto_data;
        std::ifstream input_file(path, std::ios::binary);
        if (!proto_data.ParseFromIstream(&input_file)) {
            throw std::runtime_error("Failed to deserialize.");
        }
        map_renderer::RenderSettings settings;
        settings.height = proto_data.render_settings().height();
        settings.width = proto_data.render_settings().width();
        settings.padding = proto_data.render_settings().padding();
        settings.line_width = proto_data.render_settings().line_width();
        settings.stop_radius = proto_data.render_settings().stop_radius();
        settings.bus_label_font_size = proto_data.render_settings().bus_label_font_size();
        for(const auto& val : proto_data.render_settings().bus_label_offset()) {
            settings.bus_label_offset.push_back(val);
        }
        if(proto_data.render_settings().underlayer_color().has_rgb_color()) {
            settings.underlayer_color = svg::Rgb{static_cast<uint8_t>(proto_data.render_settings().underlayer_color().rgb_color().red()),
                                                 static_cast<uint8_t>(proto_data.render_settings().underlayer_color().rgb_color().green()),
                                                 static_cast<uint8_t>(proto_data.render_settings().underlayer_color().rgb_color().blue())};
        } else if(proto_data.render_settings().underlayer_color().has_rgba_color()) {
            settings.underlayer_color = svg::Rgba{static_cast<uint8_t>(proto_data.render_settings().underlayer_color().rgba_color().red()),
                                                  static_cast<uint8_t>(proto_data.render_settings().underlayer_color().rgba_color().green()),
                                                  static_cast<uint8_t>(proto_data.render_settings().underlayer_color().rgba_color().blue()),
                                                  proto_data.render_settings().underlayer_color().rgba_color().opacity()};
        } else {
            settings.underlayer_color = proto_data.render_settings().underlayer_color().color_string();
        }
        settings.stop_label_font_size = proto_data.render_settings().stop_label_font_size();
        for(const auto& val : proto_data.render_settings().stop_label_offset()) {
            settings.stop_label_offset.push_back(val);
        }
        settings.underlayer_width = proto_data.render_settings().underlayer_width();
        for(const auto& color : proto_data.render_settings().color_palette()) {
            if(color.has_rgb_color()) {
                settings.color_palette.emplace_back(svg::Rgb({static_cast<uint8_t>(color.rgb_color().red()),
                                                              static_cast<uint8_t>(color.rgb_color().green()),
                                                              static_cast<uint8_t>(color.rgb_color().blue())}));
            } else if(color.has_rgba_color()) {
                settings.color_palette.emplace_back(svg::Rgba({static_cast<uint8_t>(color.rgba_color().red()),
                                                               static_cast<uint8_t>(color.rgba_color().green()),
                                                               static_cast<uint8_t>(color.rgba_color().blue()),
                                                               color.rgba_color().opacity()}));
            } else {
                settings.color_palette.emplace_back(color.color_string());
            }
        }
        map_renderer::MapRenderer map_renderer;
        map_renderer.SetSettings(settings);
        return map_renderer;
    }

    transport_router::TransportRouter Serialization::RouterDeserialization(const std::filesystem::path& path) {
        proto_transport_catalogue::ProtoData proto_data;
        std::ifstream input_file(path, std::ios::binary);
        if (!proto_data.ParseFromIstream(&input_file)) {
            throw std::runtime_error("Failed to deserialize.");
        }
        transport_router::TransportRouter transport_router;
        transport_router.SetBusWaitTime(proto_data.mutable_router()->mutable_routing_settings()->bus_wait_time());
        transport_router.SetBusVelocity(proto_data.mutable_router()->mutable_routing_settings()->bus_velocity());
        graph::DirectedWeightedGraph<RouteWeight> graph(proto_data.transport_catalogue().stops_size() * 2);
        std::vector<std::string> vertex_names;
        vertex_names.reserve(proto_data.router().graph().edges_size());
        for(const auto& proto_edge : proto_data.router().graph().edges()) {
            vertex_names.push_back(proto_edge.weight().name());
            if(proto_edge.weight().span_count() == 0) {
                transport_router.SetVertex(vertex_names.back(), proto_edge.from(), proto_edge.to());
            }
            graph::Edge<RouteWeight> edge;
            edge.from = proto_edge.from();
            edge.to = proto_edge.to();
            edge.weight = {proto_edge.weight().weight(), proto_edge.weight().span_count(), vertex_names.back(), proto_edge.weight().is_waiting()};
            graph.AddEdge(edge);
        }
        transport_router.SetVertexNames(std::move(vertex_names));
        transport_router.SetGraph(graph);
        return transport_router;
    }

} // namespace tc_project