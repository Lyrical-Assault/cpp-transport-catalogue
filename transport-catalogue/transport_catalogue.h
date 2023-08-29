#pragma once

#include "geo.h"
#include "domain.h"

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <functional>
#include <numeric>
#include <unordered_set>
#include <tuple>
#include <optional>

namespace tc_project::transport_catalogue{
    class TransportCatalogue{
    private:
        struct TransportCatalogueHasher {
            std::size_t operator()(std::string_view str) const {
                return sw_hasher_(str);
            }

            std::size_t operator()(const std::pair<const Stop*, const Stop*>& key) const {
                std::size_t h1 = v_hasher_(key.first);
                std::size_t h2 = v_hasher_(key.second);
                return h1 + h2 * 37;
            }

            std::size_t operator()(const Stop* stop) const {
                std::size_t h_name = sw_hasher_(stop->name);
                std::size_t h_lat = d_hasher_(stop->coordinates.lat);
                std::size_t h_lon = d_hasher_(stop->coordinates.lng);
                return h_name + h_lat * 37 + h_lon * (37 * 37);
            }

            std::size_t operator()(const Bus* bus) const {
                return sw_hasher_(bus->name);
            }

            std::hash<const void*> v_hasher_;
            std::hash<std::string_view> sw_hasher_;
            std::hash<double> d_hasher_;
        };

    public:
        void AddStop(std::string_view name, double latitude, double longitude);

        const Stop* FindStop(const std::string& name) const;

        void AddBus(std::string_view name, const std::vector<std::string>& stops, bool is_roundtrip);

        const Bus* FindBus(const std::string& name) const;

        void SetDistance(const std::string& src_name, const std::string& dest_name, int dist);

        int GetDistance(const Stop* src, const Stop* dest) const;

        const std::unordered_map<std::string_view, const Bus*, TransportCatalogueHasher>& GetIndexRoutes() const;

        const std::unordered_map<std::string_view, const Stop*, TransportCatalogueHasher>& GetIndexStops() const;

        const std::unordered_map<const Stop*, std::unordered_set<const Bus*>, TransportCatalogueHasher>& GetBusesByStop() const;

    private:
        std::deque<Stop> bus_stops_;

        std::unordered_map<std::string_view, const Stop*, TransportCatalogueHasher> index_bus_stops_;

        std::deque<Bus> routes_;

        std::unordered_map<std::string_view, const Bus*, TransportCatalogueHasher> index_routes_;

        std::unordered_map<const Stop*, std::unordered_set<const Bus*>, TransportCatalogueHasher> index_stop_to_buses_;

        std::unordered_map<std::pair<const Stop*, const Stop*>, int, TransportCatalogueHasher> index_stops_distance_;
    };
}