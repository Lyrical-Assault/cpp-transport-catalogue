#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include "transport_catalogue.h"

namespace transport_catalogue::input{
        void InputReader(TransportCatalogue& catalogue, std::istream& input);
        std::tuple<double, double, std::vector<std::pair<std::string, int>>> ParseStop(const std::string& stop_info);
        std::vector<std::string> ParseBus(const std::string& bus_info);
}