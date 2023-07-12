#pragma once

#include <string>
#include <vector>
#include "geo.h"

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

inline const double EPSILON = 1e-6;


