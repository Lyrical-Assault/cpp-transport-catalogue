#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <string_view>

namespace tc_project {
    struct Stop {
        std::string name;
        geo::Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
        bool is_roundtrip;
    };

    struct BusInfo {
        int stops_count;
        int unique_stops_count;
        int distance;
        double curvature;
    };

    struct RouteWeight {
        double weight = 0.0;
        int span_count = 0;
        std::string_view name;
        bool is_waiting = false;

        bool operator<(const RouteWeight& other) const {
            return this->weight < other.weight;
        }

        bool operator>(const RouteWeight& other) const {
            return this->weight > other.weight;
        }

        RouteWeight operator+(const RouteWeight& other) const {
            RouteWeight result;
            result.weight = weight + other.weight;
            result.span_count = span_count + other.span_count;
            return result;
        }
    };

    inline const double EPSILON = 1e-6;
}
