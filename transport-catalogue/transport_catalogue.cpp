#include "transport_catalogue.h"

namespace transport_catalogue{
    void TransportCatalogue::AddStop(const std::string& name, const double latitude, const double longitude){
        bus_stops_.push_back({name, latitude, longitude});
        Stop* new_stop = &bus_stops_.back();
        index_bus_stops_[new_stop->name] = new_stop;
    }

    TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name) const{
        auto it = index_bus_stops_.find(name);
        if (it != index_bus_stops_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stops){
        routes_.push_back({name, {}});
        Bus* new_bus = &routes_.back();
        for(const auto& stop_name : stops){
            auto stop = FindStop(stop_name);
            new_bus->stops.push_back(stop);
            index_stop_to_buses_[stop].insert(new_bus);
        }
        index_routes_[new_bus->name] = new_bus;
    }

    TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name) const{
        auto it = index_routes_.find(name);
        if (it != index_routes_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    std::tuple<int, int, int, double> TransportCatalogue::GetBusInfo(const std::string& name) const{
        std::size_t num_stops = index_routes_.at(name)->stops.size();
        auto stops = index_routes_.at(name)->stops;
        std::size_t unique_stops = std::unordered_set<Stop*>(stops.begin(), stops.end()).size();
        int distance = 0;
        double curvature = 0.0;
        for (size_t i = 0; i < num_stops - 1; ++i) {
            Stop* cur_stop = stops[i];
            Stop* next_stop = stops[i + 1];
            curvature += detail::ComputeDistance({cur_stop->latitude, cur_stop->longitude},
                                         {next_stop->latitude, next_stop->longitude});
            distance += GetDistance(cur_stop, next_stop);
        }
        return {num_stops, unique_stops, distance, distance/curvature};
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

    int TransportCatalogue::GetDistance(Stop* src, Stop* dest) const{
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