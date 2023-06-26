#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include "transport_catalogue.h"

namespace transport_catalogue::output{
    void StatReader(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output);
}