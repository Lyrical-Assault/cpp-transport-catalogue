#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>
#include <tuple>

namespace tc_project::json_reader {

    using json::Document;

    using json::Dict;

    using json::Node;

    class JsonReader {
    public:

        template <typename TC, typename MR>
        explicit JsonReader(TC&& catalogue, MR&& renderer)
                        : catalogue_(std::forward<TC>(catalogue)), renderer_(std::forward<MR>(renderer)), handler_(catalogue_, renderer_) {}

        void RequestsProcessing(std::istream& input, std::ostream& output);

    private:

        void BasesProcessing();

        void StopProcessing(const Dict& stop);

        void BusProcessing(const Dict& bus);

        void StatProcessing(std::ostream& output);

        void ParseBus(const std::string& name, int id);

        void ParseStop(const std::string& name, int id);

        void RenderProcessing();

        void ParseMap(int id);

        void ParseRoute(const graph::Router<RouteWeight>& router, int id, const std::string& from, const std::string& to);

        transport_catalogue::TransportCatalogue catalogue_;

        map_renderer::MapRenderer renderer_;

        std::vector<Node> builder_data_;

        transport_router::TransportRouter transport_router_;

        request_handler::RequestHandler handler_;

        std::vector<Dict> base_requests_;

        std::vector<Dict> stat_requests_;

        Dict json_render_settings_;

        std::map<std::string , std::map<std::string, Node>> road_distances_;

        struct StatSettings {
            std::string type;
            std::string name;
            std::string from;
            std::string to;
            int id = 0;
        };
    };

}