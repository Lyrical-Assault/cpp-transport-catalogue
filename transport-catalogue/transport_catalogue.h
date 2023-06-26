#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <functional>
#include <numeric>
#include <unordered_set>
#include <tuple>
#include "geo.h"

namespace transport_catalogue{
    class TransportCatalogue{
    public:
        struct Stop{
            std::string name;
            double latitude;
            double longitude;
        };

        struct Bus{
            std::string name;
            std::vector<Stop*> stops;
        };

        struct TransportCatalogueHasher {
            std::size_t operator()(std::string_view str) const {
                return sw_hasher_(str);
            }

            std::size_t operator()(const std::pair<Stop*, Stop*>& key) const {
                std::size_t h1 = v_hasher_(key.first);
                std::size_t h2 = v_hasher_(key.second);
                return h1 + h2 * 37;
            }

            std::size_t operator()(const TransportCatalogue::Stop* stop) const {
                std::size_t h_name = sw_hasher_(stop->name);
                std::size_t h_lat = d_hasher_(stop->latitude);
                std::size_t h_lon = d_hasher_(stop->longitude);
                return h_name + h_lat * 37 + h_lon * (37 * 37);
            }

            std::size_t operator()(const TransportCatalogue::Bus* bus) const {
                return sw_hasher_(bus->name);
            }

        private:
            std::hash<const void*> v_hasher_;
            std::hash<std::string_view> sw_hasher_;
            std::hash<double> d_hasher_;
        };

        void AddStop(const std::string& name, double latitude, double longitude);
        Stop* FindStop(const std::string& name) const;
        void AddBus(const std::string& name, const std::vector<std::string>& stops);
        Bus* FindBus(const std::string& name) const;
        std::tuple<int, int, int, double> GetBusInfo(const std::string& name) const;
        std::vector<std::string> GetStopInfo(const std::string& name) const;
        void SetDistance(const std::string& src_name, const std::string& dest_name, int dist);
        int GetDistance(Stop* src, Stop* dest) const;

    private:
        std::deque<Stop> bus_stops_;
        std::unordered_map<std::string_view, Stop*, TransportCatalogueHasher> index_bus_stops_;
        std::deque<Bus> routes_;
        std::unordered_map<std::string_view, Bus*, TransportCatalogueHasher> index_routes_;
        std::unordered_map<Stop*, std::unordered_set<Bus*>, TransportCatalogueHasher> index_stop_to_buses_;
        std::unordered_map<std::pair<Stop*, Stop*>, int, TransportCatalogueHasher> index_stops_distance_;
    };
}