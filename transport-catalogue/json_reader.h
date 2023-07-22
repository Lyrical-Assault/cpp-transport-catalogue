#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <tuple>

namespace tc_project::json_reader {

    using json::Document;

    using json::Dict;

    using json::Node;

    void RequestsProcessing(transport_catalogue::TransportCatalogue& catalogue, map_renderer::MapRenderer& renderer, std::istream& input, std::ostream& output);

    void BasesProcessing(transport_catalogue::TransportCatalogue& catalogue, std::vector<Dict>&& base_requests);

    void StopProcessing(transport_catalogue::TransportCatalogue& catalogue, const Dict& stop, std::map<std::string, std::map<std::string, Node>>& road_distances);

    void BusProcessing(transport_catalogue::TransportCatalogue& catalogue, const Dict& bus);

    void StatProcessing(const transport_catalogue::TransportCatalogue& catalogue, const map_renderer::MapRenderer& renderer, std::vector<Dict>&& stat_requests, std::ostream& output);

    void ParseBus(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, const std::string& name, int id, std::vector<json::Node>& builder_data);

    void ParseStop(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, const std::string& name, int id, std::vector<json::Node>& builder_data);

    void RenderProcessing(map_renderer::MapRenderer& renderer, Dict&&  render_settings);

    void ParseMap(const transport_catalogue::TransportCatalogue &catalogue, const map_renderer::MapRenderer& renderer, int id, std::vector<json::Node>& builder_data);

}
