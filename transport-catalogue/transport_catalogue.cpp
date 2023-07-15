#include "transport_catalogue.h"

namespace tc_project::transport_catalogue {
    void TransportCatalogue::AddStop(std::string_view name, const double latitude, const double longitude) {
        bus_stops_.push_back({std::string(name), latitude, longitude});
        Stop* new_stop = &bus_stops_.back();
        index_bus_stops_[new_stop->name] = new_stop;
    }

    const Stop* TransportCatalogue::FindStop(const std::string& name) const {
        auto it = index_bus_stops_.find(name);
        if (it != index_bus_stops_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string>& stops, bool is_roundtrip) {
        routes_.push_back({std::string(name), {}, is_roundtrip});
        Bus* new_bus = &routes_.back();
        for(const auto& stop_name : stops){
            const auto stop = FindStop(stop_name);
            new_bus->stops.push_back(stop);
            index_stop_to_buses_[stop].insert(new_bus);
        }
        if(!is_roundtrip) {
            if (stops.size() != 2) {
                for (size_t i = stops.size() - 2; i > 0; --i) {
                    const auto stop = FindStop(stops[i]);
                    new_bus->stops.push_back(stop);
                }
            }
            const auto stop = FindStop(stops[0]);
            new_bus->stops.push_back(stop);
        }
        index_routes_[new_bus->name] = new_bus;
    }

    const Bus* TransportCatalogue::FindBus(const std::string& name) const {
        auto it = index_routes_.find(name);
        if (it != index_routes_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    void TransportCatalogue::SetDistance(const std::string& src_name, const std::string& dest_name, const int dist) {
        index_stops_distance_[{FindStop(src_name), FindStop(dest_name)}] = dist;
    }

    int TransportCatalogue::GetDistance(const Stop* src, const Stop* dest) const {
        auto it_fwd = index_stops_distance_.find({src, dest});
        auto it_bck = index_stops_distance_.find({dest, src});
        if(it_fwd != index_stops_distance_.end()) {
            return it_fwd->second;
        } else if(it_bck != index_stops_distance_.end()) {
            return it_bck->second;
        } else {
            return 0;
        }
    }

    const std::unordered_map<std::string_view, const Bus*, TransportCatalogue::TransportCatalogueHasher>& TransportCatalogue::GetIndexRoutes() const {
        return index_routes_;
    }

    const std::unordered_map<const Stop*, std::unordered_set<const Bus*>, TransportCatalogue::TransportCatalogueHasher>& TransportCatalogue::GetBusesByStop() const {
        return index_stop_to_buses_;
    }
}