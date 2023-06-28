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
    input::ReadInput(catalogue, std::cin);
    output::ReadAndOutputStats(catalogue, std::cin, std::cout);
    return 0;
}