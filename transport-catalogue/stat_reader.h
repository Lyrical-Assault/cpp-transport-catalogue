#pragma once

#include "transport_catalogue.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace transport_catalogue::output {

    void ReadAndOutputStats(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output);

} // namespace transport_catalogue::output