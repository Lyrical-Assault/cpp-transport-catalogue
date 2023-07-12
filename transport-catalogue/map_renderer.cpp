#include "map_renderer.h"

namespace map_renderer {

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    std::vector<svg::Circle> MapRenderer::StopsCircleRender(const std::vector<const Stop*>& stops) const {
        std::vector<svg::Circle> result;
        std::vector<geo::Coordinates> geo_coords;
        for(const auto& stop : stops) {
            geo_coords.push_back({stop->coordinates.lat, stop->coordinates.lng});
        }
        SphereProjector projector {
                geo_coords.begin(), geo_coords.end(), settings_.width, settings_.height, settings_.padding
        };
        for(int i = 0; i < static_cast<int>(stops.size()); ++i) {
            svg::Circle point;
            const svg::Point screen_coord = projector(geo_coords[i]);
            point.SetRadius(settings_.stop_radius).SetCenter({screen_coord.x, screen_coord.y}).SetFillColor("white");
            result.push_back(point);
        }
        return result;
    }

    std::vector<svg::Text> MapRenderer::StopsTextRender(const std::vector<const Stop*>& stops) const {
        std::vector<svg::Text> result;
        std::vector<geo::Coordinates> geo_coords;
        for(const auto& stop : stops) {
            geo_coords.push_back({stop->coordinates.lat, stop->coordinates.lng});
        }
        SphereProjector projector {
                geo_coords.begin(), geo_coords.end(), settings_.width, settings_.height, settings_.padding
        };
        for(int i = 0; i < static_cast<int>(stops.size()); ++i) {
            svg::Text text;
            svg::Text stroke;
            const svg::Point screen_coord = projector(geo_coords[i]);
            text.SetData(stops[i]->name)
                    .SetPosition(screen_coord)
                    .SetFontFamily("Verdana")
                    .SetFontSize(settings_.stop_label_font_size)
                    .SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
                    .SetFillColor("black");
            stroke.SetData(stops[i]->name)
                    .SetPosition(screen_coord)
                    .SetFontFamily("Verdana")
                    .SetFontSize(settings_.stop_label_font_size)
                    .SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
                    .SetFillColor(settings_.underlayer_color)
                    .SetStrokeWidth(settings_.underlayer_width)
                    .SetStrokeColor(settings_.underlayer_color)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            result.push_back(stroke);
            result.push_back(text);
        }
        return result;
    }

    std::vector<svg::Polyline> MapRenderer::BusesLineRender(const std::vector<const Bus*>& buses) const {
        std::vector<svg::Polyline> result;
        std::vector<geo::Coordinates> geo_coords;
        for(const auto& bus : buses) {
            for(const auto& stop : bus->stops) {
                geo_coords.push_back({stop->coordinates.lat, stop->coordinates.lng});
            }
        }
        SphereProjector projector {
                geo_coords.begin(), geo_coords.end(), settings_.width, settings_.height, settings_.padding
        };
        int current_bus_stops_offset = 0;
        int color_index = 0;
        for(auto bus : buses) {
            if (bus->stops.empty()) {
                continue;
            }
            svg::Polyline line;
            for(int j = 0; j < static_cast<int>(bus->stops.size()); ++j) {
                const svg::Point screen_coord = projector(geo_coords[j + current_bus_stops_offset]);
                line.AddPoint({screen_coord.x, screen_coord.y});
            }
            line.SetStrokeColor(settings_.color_palette[color_index]).SetFillColor("none").SetStrokeWidth(settings_.line_width);
            line.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            result.push_back(line);
            current_bus_stops_offset += static_cast<int>(bus->stops.size());
            color_index = (color_index + 1) % settings_.color_palette.size();
        }
        return result;
    }


    std::vector<svg::Text> MapRenderer::BusesTextRender(const std::vector<const Bus*>& buses) const {
        std::vector<svg::Text> result;
        std::vector<geo::Coordinates> geo_coords;
        for(const auto& bus : buses) {
            for(const auto& stop : bus->stops) {
                geo_coords.push_back({stop->coordinates.lat, stop->coordinates.lng});
            }
        }
        SphereProjector projector {
                geo_coords.begin(), geo_coords.end(), settings_.width, settings_.height, settings_.padding
        };
        int current_bus_stops_offset = 0;
        int color_index = 0;
        for(const auto& bus : buses) {
            if (bus->stops.empty()) {
                continue;
            }
            svg::Text text;
            svg::Text stroke;
            for(int j = 0; j < static_cast<int>(bus->stops.size()); ++j) {
                if(j == 0) {
                    text.SetPosition(projector(geo_coords[j + current_bus_stops_offset]))
                            .SetData(bus->name)
                            .SetFontFamily("Verdana")
                            .SetFontWeight("bold")
                            .SetFontSize(settings_.bus_label_font_size)
                            .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                            .SetFillColor(settings_.color_palette[color_index]);
                    stroke.SetPosition(projector(geo_coords[j + current_bus_stops_offset]))
                            .SetData(bus->name)
                            .SetFontFamily("Verdana")
                            .SetFontWeight("bold")
                            .SetFontSize(settings_.bus_label_font_size)
                            .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                            .SetFillColor(settings_.underlayer_color)
                            .SetStrokeWidth(settings_.underlayer_width)
                            .SetStrokeColor(settings_.underlayer_color)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                    result.push_back(stroke);
                    result.push_back(text);
                }
                if(!bus->is_roundtrip && j == bus->stops.size()/2 && (bus->stops[0] != bus->stops[bus->stops.size()/2])) {
                    text.SetPosition(projector(geo_coords[j + current_bus_stops_offset]))
                            .SetData(bus->name)
                            .SetFontFamily("Verdana")
                            .SetFontWeight("bold")
                            .SetFontSize(settings_.bus_label_font_size)
                            .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                            .SetFillColor(settings_.color_palette[color_index]);
                    stroke.SetPosition(projector(geo_coords[j + current_bus_stops_offset]))
                            .SetData(bus->name)
                            .SetFontFamily("Verdana")
                            .SetFontWeight("bold")
                            .SetFontSize(settings_.bus_label_font_size)
                            .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                            .SetFillColor(settings_.underlayer_color)
                            .SetStrokeWidth(settings_.underlayer_width)
                            .SetStrokeColor(settings_.underlayer_color)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                    result.push_back(stroke);
                    result.push_back(text);
                }
            }
            current_bus_stops_offset += static_cast<int>(bus->stops.size());
            color_index = (color_index + 1) % settings_.color_palette.size();
        }
        return result;
    }
}