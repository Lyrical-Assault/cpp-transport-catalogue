#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <utility>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string_view>
#include <transport_catalogue.pb.h>

namespace tc_project {

    class Serialization {
    public:
        static void BaseSerialization(const std::filesystem::path& path, const transport_catalogue::TransportCatalogue& transport_catalogue, const map_renderer::MapRenderer& renderer, const transport_router::TransportRouter& transport_router);

        static proto_transport_catalogue::TransportCatalogue CatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue);

        static proto_map_renderer::RenderSettings MapSerialization(const map_renderer::MapRenderer& renderer);

        static proto_transport_router::Router RoutingSerialization(const transport_router::TransportRouter& transport_router);

        static transport_catalogue::TransportCatalogue CatalogueDeserialization(const std::filesystem::path& path);

        static map_renderer::MapRenderer MapDeserialization(const std::filesystem::path& path);

        static transport_router::TransportRouter RouterDeserialization(const std::filesystem::path& path);

    };

} // namespace tc_project