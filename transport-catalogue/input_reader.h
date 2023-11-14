#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <vector>
#include <string>
#include <tuple>

namespace transport_catalogue::input {

        void ReadInput(TransportCatalogue& catalogue, std::istream& input);

        std::tuple<double, double, std::vector<std::pair<std::string, int>>> ParseStop(const std::string& stop_info);

        std::vector<std::string> ParseBus(const std::string& bus_info);

}