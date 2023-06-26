#include <iostream>
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "test_functions.h"

using namespace std;

int main() {
    using namespace transport_catalogue;
    TransportCatalogue catalogue;
    tests::TransportCatalogueTest();
    input::InputReader(catalogue, std::cin);
    output::StatReader(catalogue, std::cin, std::cout);
    return 0;
}