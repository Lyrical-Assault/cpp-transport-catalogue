#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <tuple>

namespace json {
    Document LoadJSON(const std::string& s);

    std::string Shielding(const std::string& str);

    void RequestsProcessing(transport_catalogue::TransportCatalogue& catalogue, map_renderer::MapRenderer& renderer, std::istream& input, std::ostream& output);

    void BasesProcessing(transport_catalogue::TransportCatalogue& catalogue, std::vector<Dict>&& base_requests);

    void StopProcessing(transport_catalogue::TransportCatalogue& catalogue, const Dict& stop, std::map<std::string, std::map<std::string, Node>>& road_distances);

    void BusProcessing(transport_catalogue::TransportCatalogue& catalogue, const Dict& bus);

    void StatProcessing(const transport_catalogue::TransportCatalogue& catalogue, const map_renderer::MapRenderer& renderer, std::vector<Dict>&& stat_requests, std::ostream& output);

    void ParseBus(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, const std::string& name, int id, std::vector<std::string>& parse_data);

    void ParseStop(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, const std::string& name, int id, std::vector<std::string>& parse_data);

    void RenderProcessing(map_renderer::MapRenderer& renderer, Dict&&  render_settings);

    void ParseMap(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, int id, std::vector<std::string>& parse_data);
}