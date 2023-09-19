#include "request_handler.h"

namespace tc_project::request_handler {

    RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& renderer)
            : db_(db), renderer_(renderer) {
    }

    std::optional<BusInfo> RequestHandler::GetBusInfo(const std::string_view& name) const {
        BusInfo bus_info{};
        auto bus = db_.FindBus(std::string(name));
        if(!bus) {
            return std::nullopt;
        }
        auto index_routes = db_.GetIndexRoutes();
        int stops_count = static_cast<int>(index_routes.at(name)->stops.size());
        bus_info.stops_count = stops_count;
        auto stops = index_routes.at(name)->stops;
        bus_info.unique_stops_count = static_cast<int>(std::unordered_set<const Stop*>(stops.begin(), stops.end()).size());
        bus_info.distance = 0;
        double geo_dist = 0.0;
        for (int i = 0; i < stops_count - 1; ++i) {
            const Stop* cur_stop = stops[i];
            const Stop* next_stop = stops[i + 1];
            geo_dist += geo::ComputeDistance({cur_stop->coordinates.lat, cur_stop->coordinates.lng},
                                             {next_stop->coordinates.lat, next_stop->coordinates.lng});
            bus_info.distance += db_.GetDistance(cur_stop, next_stop);
        }
        bus_info.curvature = bus_info.distance/geo_dist;
        return bus_info;
    }

    std::optional<const std::unordered_set<const Bus*>*>  RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        const auto& buses_by_stop = db_.GetBusesByStop();
        auto stop = db_.FindStop(std::string(stop_name));
        if(!stop) {
            return std::nullopt;
        }
        auto it = buses_by_stop.find(stop);
        if (it != buses_by_stop.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    svg::Document RequestHandler::RenderMap() const {
        auto info = db_.GetIndexRoutes();
        std::vector<const Bus*> buses;
        for(const auto [name, bus_ptr] : info) {
            buses.push_back(bus_ptr);
        }
        return renderer_.MapRender(buses);
    }

}
