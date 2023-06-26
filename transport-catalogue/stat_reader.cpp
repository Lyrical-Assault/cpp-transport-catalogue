#include "stat_reader.h"

namespace transport_catalogue::output{
    void StatReader(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output){
        int num_req;
        input >> num_req;
        input.ignore();
        while (num_req != 0){
            std::string query;
            std::getline(input, query);
            if (query.substr(0, 3) == "Bus") {
                std::string name = query.substr(4);
                if (catalogue.FindBus(name) == nullptr) {
                    output << "Bus " << name << ": not found" << std::endl;
                }
                else{
                    auto bus_info = catalogue.GetBusInfo(name);
                    output << "Bus " << name << ": " << std::get<0>(bus_info)<< " stops on route, "
                           << std::get<1>(bus_info) << " unique stops, "
                           << std::setprecision(6) << std::get<2>(bus_info) << " route length, "
                           << std::get<3>(bus_info) << " curvature" << std::endl;
                }
            }else if (query.substr(0, 4) == "Stop") {
                std::string name = query.substr(5);
                if(catalogue.FindStop(name) == nullptr){
                    output << "Stop " << name << ": not found" << std::endl;
                }
                else if(catalogue.GetStopInfo(name).empty()){
                    output << "Stop " << name << ": no buses" << std::endl;
                } else {
                    auto stop_info = catalogue.GetStopInfo(name);
                    output << "Stop " << name << ": buses";
                    for(const auto& bus : stop_info){
                        output << ' ' << bus;
                    }
                    output << std::endl;
                }
            }
            --num_req;
        }
    }
}