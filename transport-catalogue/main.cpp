#include "test_functions.h"
#include "json_reader.h"

int main() {
    tc_project::transport_catalogue::TransportCatalogue catalogue;
    tc_project::map_renderer::MapRenderer renderer;
    tc_project::json_reader::RequestsProcessing(catalogue, renderer, std::cin, std::cout);
    tc_project::transport_catalogue::tests::TransportCatalogueTest();
    tc_project::request_handler::tests::RequestHandlerTest();
    tc_project::json_reader::tests::JsonReader();
    json::tests::JsonTests();
}