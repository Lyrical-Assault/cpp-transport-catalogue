#include "transport_catalogue.h"

namespace transport_catalogue{
    void TransportCatalogue::AddStop(std::string_view name, const double latitude, const double longitude){
        bus_stops_.push_back({std::string(name), latitude, longitude});
        Stop* new_stop = &bus_stops_.back();
        index_bus_stops_[new_stop->name] = new_stop;
    }

    const Stop* TransportCatalogue::FindStop(const std::string& name) const{
        auto it = index_bus_stops_.find(name);
        if (it != index_bus_stops_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string>& stops){
        routes_.push_back({std::string(name), {}});
        Bus* new_bus = &routes_.back();
        for(const auto& stop_name : stops){
            const auto stop = FindStop(stop_name);
            new_bus->stops.push_back(stop);
            index_stop_to_buses_[stop].insert(new_bus);
        }
        index_routes_[new_bus->name] = new_bus;
    }

    const Bus* TransportCatalogue::FindBus(const std::string& name) const{
        auto it = index_routes_.find(name);
        if (it != index_routes_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    std::optional<BusInfo> TransportCatalogue::GetBusInfo(const std::string& name) const{
        BusInfo bus_info;
        bus_info.stops_count = static_cast<int>(index_routes_.at(name)->stops.size());
        auto stops = index_routes_.at(name)->stops;
        bus_info.unique_stops_count = static_cast<int>(std::unordered_set<const Stop*>(stops.begin(), stops.end()).size());
        bus_info.distance = 0;
        double geo_dist = 0.0;
        for (size_t i = 0; i < bus_info.stops_count - 1; ++i) {
            const Stop* cur_stop = stops[i];
            const Stop* next_stop = stops[i + 1];
            geo_dist += detail::ComputeDistance({cur_stop->coordinates.lat, cur_stop->coordinates.lng},
                                         {next_stop->coordinates.lat, next_stop->coordinates.lng});
            bus_info.distance += GetDistance(cur_stop, next_stop);
        }
        bus_info.curvature = bus_info.distance/geo_dist;
        return bus_info;
    }

    std::vector<std::string> TransportCatalogue::GetStopInfo(const std::string& name) const{
        std::vector<std::string> result;
        auto it = index_stop_to_buses_.find(FindStop(name));
        if (it != index_stop_to_buses_.end()) {
            const auto& buses = it->second;
            for (const auto* bus : buses) {
                result.push_back(bus->name);
            }
            std::sort(result.begin(), result.end());
        }
        return result;
    }

    void TransportCatalogue::SetDistance(const std::string& src_name, const std::string& dest_name, const int dist){
        index_stops_distance_[{FindStop(src_name), FindStop(dest_name)}] = dist;
    }

    int TransportCatalogue::GetDistance(const Stop* src, const Stop* dest) const{
        auto it_fwd = index_stops_distance_.find({src, dest});
        auto it_bck = index_stops_distance_.find({dest, src});
        if(it_fwd != index_stops_distance_.end()){
            return it_fwd->second;
        }
        else if(it_bck != index_stops_distance_.end()){
            return it_bck->second;
        }
        else{
            return 0;
        }
    }
}