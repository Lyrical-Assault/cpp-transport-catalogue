#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>
#include <unordered_set>
#include <string_view>

namespace tc_project::request_handler {
    class RequestHandler {
    public:
        RequestHandler(const transport_catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& renderer);

        std::optional<BusInfo> GetBusInfo(const std::string_view& bus_name) const;

        std::optional<const std::unordered_set<const Bus*>*> GetBusesByStop(const std::string_view& stop_name) const;

        svg::Document RenderMap() const;

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const transport_catalogue::TransportCatalogue& db_;

        const map_renderer::MapRenderer& renderer_;
    };
}