#include "test_functions.h"

namespace transport_catalogue::tests{

    void AssertImpl(bool value, const std::string& expr_str, const std::string& file, const std::string& func, unsigned line,
                    const std::string& hint) {
        if (!value) {
            std::cerr << file << "("s << line << "): "s << func << ": "s;
            std::cerr << "ASSERT("s << expr_str << ") failed."s;
            if (!hint.empty()) {
                std::cerr << " Hint: "s << hint;
            }
            std::cerr << std::endl;
            abort();
        }
    }

    void AddBus() {
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 33.67674, 44.98942);
        catalogue.AddStop("Stop 2", 22.08086, 11.84844);
        catalogue.AddStop("Stop 3", 55.57754, 66.70692);
        catalogue.AddBus("Bus 1", {"Stop 1", "Stop 2", "Stop 3"});
        const auto bus_test = catalogue.FindBus("Bus 1");
        ASSERT_HINT(bus_test != nullptr, "Bus not added!");
        ASSERT_EQUAL_HINT(bus_test->name, "Bus 1", "Wrong bus name!");
        ASSERT_EQUAL_HINT(bus_test->stops.size(), 3, "Wrong number of stops!");
        ASSERT_EQUAL_HINT(bus_test->stops[0]->name, "Stop 1", "Wrong stop!");
        ASSERT_EQUAL_HINT(bus_test->stops[1]->name, "Stop 2", "Wrong stop!");
        ASSERT_EQUAL_HINT(bus_test->stops[2]->name, "Stop 3", "Wrong stop!");
    }

    void AddStop() {
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 33.3333, 44.4444);
        const auto stop_test = catalogue.FindStop("Stop 1");
        ASSERT_HINT(stop_test != nullptr, "Stop not added!");
        ASSERT_EQUAL_HINT(stop_test->name, "Stop 1", "Wrong stop name!");
        ASSERT_EQUAL_HINT(stop_test->coordinates.lat, 33.3333, "Wrong coordinate!");
        ASSERT_EQUAL_HINT(stop_test->coordinates.lng, 44.4444, "Wrong coordinate!");
    }

    void FindBus() {
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.0333, 44.9999);
        catalogue.AddStop("Stop 2", 50.0333, 40.9999);
        catalogue.AddStop("Stop 3", 60.0333, 30.9999);
        catalogue.AddBus("Bus 1", {"Stop 1", "Stop 2", "Stop 3"});
        const auto bus_test = catalogue.FindBus("Bus 1");
        ASSERT_HINT(bus_test != nullptr, "Bus not added!");
        ASSERT_EQUAL_HINT(bus_test->name, "Bus 1", "Wrong name of bus!");
        ASSERT_EQUAL_HINT(bus_test->stops.size(), 3, "Wrong number of stops!");
        ASSERT_EQUAL_HINT(bus_test->stops[0]->name, "Stop 1", "Wrong stop!");
        ASSERT_EQUAL_HINT(bus_test->stops[1]->name, "Stop 2", "Wrong stop!");
        ASSERT_EQUAL_HINT(bus_test->stops[2]->name, "Stop 3", "Wrong stop!");
        const auto non_bus = catalogue.FindBus("NonExisting");
        ASSERT_HINT(non_bus == nullptr, "Non existent bus found!");
    }

    void FindStop() {
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 33.3333, 44.4444);
        const auto stop_test = catalogue.FindStop("Stop 1");
        ASSERT_HINT(stop_test != nullptr, "Stop not added!");
        ASSERT_EQUAL_HINT(stop_test->name, "Stop 1", "Wrong name of bus stop!");
        ASSERT_EQUAL_HINT(stop_test->coordinates.lat, 33.3333, "Wrong latitude of bus stop!");
        ASSERT_EQUAL_HINT(stop_test->coordinates.lng, 44.4444, "Wrong longitude of bus stop!");
        const auto non_stop = catalogue.FindStop("NonExisting");
        ASSERT_HINT(non_stop == nullptr, "Non existent stop found!");
    }

    void GetBusInfo() {
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.123, 37.456);
        catalogue.AddStop("Stop 2", 55.789, 37.890);
        catalogue.AddStop("Stop 3", 55.555, 37.777);
        catalogue.SetDistance("Stop 1", "Stop 2", 1000);
        catalogue.SetDistance("Stop 2", "Stop 3", 2000);
        catalogue.SetDistance("Stop 3", "Stop 1", 3000);
        catalogue.AddBus("750", {"Stop 1", "Stop 2", "Stop 3", "Stop 2"});
        const auto info_test = catalogue.GetBusInfo("750");
        ASSERT_EQUAL_HINT(info_test->stops_count, 4, "Wrong stops number!");
        ASSERT_EQUAL_HINT(info_test->unique_stops_count, 3, "Wrong unique-stops number!");
        ASSERT_EQUAL_HINT(info_test->distance, 5000, "Wrong distance!");
        ASSERT_HINT(std::abs(info_test->curvature - 0.0376269) < 1e-6, "Wrong curvature!");
    }
    void GetStopInfo(){
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.123, 37.456);
        catalogue.AddStop("Stop 2", 55.789, 37.890);
        catalogue.AddStop("Stop 3", 55.555, 37.777);
        catalogue.AddBus("Bus 1", {"Stop 1", "Stop 2"});
        catalogue.AddBus("Bus 2", {"Stop 2", "Stop 3"});
        auto stop_info = catalogue.GetStopInfo("Stop 1");
        ASSERT_HINT(stop_info.size() == 1, "Wrong stops number!");
        ASSERT_EQUAL_HINT(stop_info[0], "Bus 1", "Wrong bus at stop!");
        stop_info = catalogue.GetStopInfo("Stop 2");
        ASSERT_HINT(stop_info.size() == 2, "Wrong stops number!");
        ASSERT_EQUAL_HINT(stop_info[0], "Bus 1", "Wrong bus at stop!");
        ASSERT_EQUAL_HINT(stop_info[1], "Bus 2", "Wrong bus at stop!");
        stop_info = catalogue.GetStopInfo("Stop 3");
        ASSERT_HINT(stop_info.size() == 1, "Wrong stops number!");
        ASSERT_EQUAL_HINT(stop_info[0], "Bus 2", "Wrong bus at stop!");
    }
    void SetDistance(){
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.123, 37.456);
        catalogue.AddStop("Stop 2", 55.789, 37.890);
        catalogue.AddStop("Stop 3", 55.555, 37.777);
        catalogue.SetDistance("Stop 1", "Stop 2", 10);
        const Stop* stop1 = catalogue.FindStop("Stop 1");
        const Stop* stop2 = catalogue.FindStop("Stop 2");
        const Stop* stop3 = catalogue.FindStop("Stop 3");
        ASSERT_HINT(catalogue.GetDistance(stop1, stop2) == 10, "Wrong distance!");
        ASSERT_HINT(catalogue.GetDistance(stop2, stop3) == 0, "Wrong distance!");
    }
    void GetDistance(){
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.123, 37.456);
        catalogue.AddStop("Stop 2", 55.789, 37.890);
        catalogue.SetDistance("Stop 1", "Stop 2", 10);
        const Stop* stop1 = catalogue.FindStop("Stop 1");
        const Stop* stop2 = catalogue.FindStop("Stop 2");
        ASSERT_HINT(catalogue.GetDistance(stop1, stop2) == 10, "Wrong distance!");
        const Stop* stop3 = catalogue.FindStop("NonExisting");
        ASSERT_HINT(stop3 == nullptr, "Non existent stop found!");
        ASSERT_HINT(catalogue.GetDistance(stop1, stop3) == 0, "Wrong distance!");
    }
    void TransportCatalogueTest() {
        RUN_TEST(AddBus);
        RUN_TEST(AddStop);
        RUN_TEST(FindStop);
        RUN_TEST(FindBus);
        RUN_TEST(GetBusInfo);
        RUN_TEST(GetStopInfo);
        RUN_TEST(SetDistance);
        RUN_TEST(GetDistance);
    }
}