#include "input_reader.h"

namespace transport_catalogue::input{
        void InputReader(TransportCatalogue& catalogue, std::istream& input){
            int num_req;
            input >> num_req;
            input.ignore();
            std::vector<std::pair<std::string, std::tuple<double, double, std::vector<std::pair<std::string, int>>>>> stop_requests;
            std::vector<std::pair<std::string, std::vector<std::string>>> bus_requests;
            while (num_req != 0){
                std::string query;
                std::getline(std::cin, query);
                if (query.substr(0, 4) == "Stop") {
                    std::size_t pos_end = query.find(':');
                    std::string stop_name = query.substr(5, pos_end - 5);
                    std::string stop_info = query.substr(pos_end + 2);
                    auto info = ParseStop(stop_info);
                    stop_requests.emplace_back(stop_name, info);
                } else if (query.substr(0, 3) == "Bus") {
                    std::size_t pos_end = query.find(':');
                    std::string bus_name = query.substr(4, pos_end - 4);
                    std::string bus_info= query.substr(pos_end + 2);
                    auto stops = ParseBus(bus_info);
                    bus_requests.emplace_back(bus_name, stops);
                }
                --num_req;
            }
            for (const auto& stop_request : stop_requests) {
                catalogue.AddStop(stop_request.first, std::get<0>(stop_request.second), std::get<1>(stop_request.second));
            }
            for (const auto& stop_request : stop_requests) {
                for(const auto& dest : std::get<2>(stop_request.second)){
                    catalogue.SetDistance(stop_request.first, dest.first, dest.second);
                }
            }
            for (const auto& bus_request : bus_requests) {
                catalogue.AddBus(bus_request.first, bus_request.second);
            }
        }

        std::tuple<double, double, std::vector<std::pair<std::string, int>>> ParseStop(const std::string& stop_info) {
            std::size_t pos = stop_info.find_first_of(',');
            double latitude = std::stod(stop_info.substr(0, pos));
            std::string temp_str = stop_info.substr(pos + 2);
            pos = temp_str.find_first_of(',');
            double longitude = std::stod(temp_str.substr(0, pos));
            temp_str = temp_str.substr(pos + 2);
            std::vector<std::pair<std::string, int>> distance;
            while (pos != std::string::npos){
                pos = temp_str.find_first_of('m');
                int dist = std::stoi(temp_str.substr(0, pos));
                temp_str = temp_str.substr(pos + 5);
                pos = temp_str.find_first_of(',');
                distance.emplace_back(temp_str.substr(0, pos), dist);
                temp_str = temp_str.substr(pos + 2);
            }
            return {latitude, longitude, distance};
        }

        std::vector<std::string> ParseBus(const std::string& bus_info){
            std::vector<std::string> stops;
            size_t pos = bus_info.find(':');
            std::string str = bus_info.substr(pos + 1);
            if (bus_info.find('>') == std::string::npos) {
                size_t prev_pos = 0;
                pos = str.find(" - ");
                while (pos != std::string::npos) {
                    std::string stop = str.substr(prev_pos, pos - prev_pos);
                    stops.push_back(stop);
                    prev_pos = pos + 3;
                    pos = str.find(" - ", prev_pos);
                }
                auto second_part = stops;
                stops.push_back(str.substr(prev_pos));
                stops.insert(stops.end(), second_part.rbegin(), second_part.rend());
            } else {
                size_t prev_pos = 0;
                pos = str.find(" > ");
                while (pos != std::string::npos) {
                    std::string stop = str.substr(prev_pos, pos - prev_pos);
                    stops.push_back(stop);
                    prev_pos = pos + 3;
                    pos = str.find(" > ", prev_pos);
                }
                stops.push_back(str.substr(prev_pos));
            }
            return stops;
        }
    }