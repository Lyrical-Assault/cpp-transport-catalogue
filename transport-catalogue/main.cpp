#include "test_functions.h"
#include "json_reader.h"

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    map_renderer::MapRenderer renderer;
    json::RequestsProcessing(catalogue, renderer, std::cin, std::cout);
    transport_catalogue::tests::TransportCatalogueTest();
    request_handler::tests::RequestHandlerTest();
    json::tests::JsonReader();
    json::tests::JsonTests();
}