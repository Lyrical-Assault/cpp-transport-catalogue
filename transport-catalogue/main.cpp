#include <fstream>
#include <iostream>
#include <string_view>
#include "json_reader.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {

        tc_project::transport_catalogue::TransportCatalogue catalogue;
        tc_project::map_renderer::MapRenderer renderer;
        tc_project::json_reader::JsonReader json_reader(catalogue, renderer);
        json_reader.Make_Base(std::cin);

    } else if (mode == "process_requests"sv) {

        tc_project::transport_catalogue::TransportCatalogue catalogue;
        tc_project::map_renderer::MapRenderer renderer;
        tc_project::json_reader::JsonReader json_reader(catalogue, renderer);
        json_reader.Process_Requests(std::cin, std::cout);

    } else {
        PrintUsage();
        return 1;
    }
}