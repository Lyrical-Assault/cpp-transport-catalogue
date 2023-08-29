#include "json_reader.h"
#include <iostream>
using namespace std;

int main() {
    tc_project::transport_catalogue::TransportCatalogue catalogue;
    tc_project::map_renderer::MapRenderer renderer;
    tc_project::json_reader::JsonReader json_reader(catalogue, renderer);
    json_reader.RequestsProcessing(cin, cout);
}