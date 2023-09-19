#pragma once

#include "domain.h"
#include "router.h"
#include "graph.h"
#include "transport_catalogue.h"

#include <string_view>
#include <unordered_map>
#include <memory>

#include <vector>
#include <string>

namespace tc_project::transport_router {
    class TransportRouter {
    private:

        struct TransportRouterHasher {
            std::size_t operator()(std::string_view str) const {
                return sw_hasher_(str);
            }

            std::size_t operator()(const std::pair<size_t , size_t>& key) const {
                std::size_t h1 = s_hasher_(key.first);
                std::size_t h2 = s_hasher_(key.second);
                return h1 + h2 * 37;
            }

            std::hash<std::string_view> sw_hasher_;
            std::hash<size_t> s_hasher_;
        };

    public:

        void SetBusWaitTime(int bus_wait_time);

        void SetBusVelocity(double bus_velocity);

        void BuildGraph(const transport_catalogue::TransportCatalogue &catalogue);

        size_t GetVertex(const std::string& name) const;

        const graph::DirectedWeightedGraph<RouteWeight>& GetGraph() const;

        int GetBusWaitTime() const;

        double GetBusVelocity() const;

        void SetGraph(const graph::DirectedWeightedGraph<RouteWeight>& graph);

        void SetVertexNames(std::vector<std::string>&& names);

        void SetVertex(std::string_view name, size_t from, size_t to);

    private:

        std::vector<std::string> names_;

        std::unordered_map<std::string_view, std::pair<size_t, size_t>, TransportRouterHasher> vertex_id_;

        int bus_wait_time_ = 0;

        double bus_velocity_ = 0.0;

        std::unique_ptr<graph::DirectedWeightedGraph<RouteWeight>> graph_;

    };

}
