#include "transport_router.h"

namespace tc_project::transport_router {

    void TransportRouter::SetBusWaitTime(int bus_wait_time) {
        bus_wait_time_ = bus_wait_time;
    }

    void TransportRouter::SetBusVelocity(double bus_velocity) {
        bus_velocity_ = bus_velocity;
    }

    size_t TransportRouter::GetVertex(const std::string& name) const {
        return vertex_id_.at(name).first;
    }

    void TransportRouter::BuildGraph(const transport_catalogue::TransportCatalogue &catalogue) {
        graph::DirectedWeightedGraph<RouteWeight> graph(catalogue.GetIndexStops().size() * 2);
        size_t i = 0;
        for(const auto& [name, stop] : catalogue.GetIndexStops()) {
            graph.AddEdge({2 * i, 2 * i + 1,
                           {static_cast<double>(bus_wait_time_), 0, name, true}});
            vertex_id_[name] = {2 * i, 2 * i + 1};
            ++i;
        }
        for(const auto& [name, bus] : catalogue.GetIndexRoutes()) {
            for(auto it_from = bus->stops.begin(); it_from != bus->stops.end(); std::advance(it_from, 1)) {
                int span_count = 0;
                double distance = 0.0;
                for(auto it_to = std::next(it_from); it_to != bus->stops.end(); std::advance(it_to, 1)) {
                    if((*it_to)->name != (*it_from)->name) {
                        distance += catalogue.GetDistance(*std::prev(it_to), *it_to);
                        ++span_count;
                        graph.AddEdge({vertex_id_.at((*it_from)->name).second, vertex_id_.at((*it_to)->name).first,
                                       { distance / (bus_velocity_ / 60 * 1000), span_count, name, false}});
                    }
                }
            }
        }
        graph_ = std::make_unique<graph::DirectedWeightedGraph<RouteWeight>>(graph);
    }

    const graph::DirectedWeightedGraph<RouteWeight>& TransportRouter::GetGraph() const {
        return *graph_;
    }

    int TransportRouter::GetBusWaitTime() const {
        return bus_wait_time_;
    }

    double TransportRouter::GetBusVelocity() const {
        return bus_velocity_;
    }

    void TransportRouter::SetVertexNames(std::vector<std::string>&& names) {
        names_ = std::move(names);
    }

    void TransportRouter::SetGraph(const graph::DirectedWeightedGraph<RouteWeight>& graph) {
        graph_ = std::make_unique<graph::DirectedWeightedGraph<RouteWeight>>(graph);
    }

    void TransportRouter::SetVertex(std::string_view name, size_t from, size_t to) {
        vertex_id_[name] = {from, to};
    }

} // namespace tc_project::transport_router