#include "stat_reader.h"

namespace transport_catalogue::output {

    void ReadAndOutputStats(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output) {
        int num_req;
        input >> num_req;
        input.ignore();
        while (num_req != 0) {
            std::string query;
            std::getline(input, query);
            if (query.substr(0, 3) == "Bus") {
                std::string name = query.substr(4);
                if (catalogue.FindBus(name) == nullptr) {
                    output << "Bus " << name << ": not found" << std::endl;
                } else {
                    auto bus_info = catalogue.GetBusInfo(name);
                    output << "Bus " << name << ": " << bus_info->stops_count << " stops on route, "
                           << bus_info->unique_stops_count << " unique stops, "
                           << std::setprecision(6) << bus_info->distance << " route length, "
                           << bus_info->curvature << " curvature" << std::endl;
                }
            } else if (query.substr(0, 4) == "Stop") {
                std::string name = query.substr(5);
                if(catalogue.FindStop(name) == nullptr) {
                    output << "Stop " << name << ": not found" << std::endl;
                }
                else if(catalogue.GetStopInfo(name).empty()) {
                    output << "Stop " << name << ": no buses" << std::endl;
                } else {
                    auto stop_info = catalogue.GetStopInfo(name);
                    output << "Stop " << name << ": buses";
                    for(const auto& bus : stop_info) {
                        output << ' ' << bus;
                    }
                    output << std::endl;
                }
            }
            --num_req;
        }
    }

} // namespace transport_catalogue::output