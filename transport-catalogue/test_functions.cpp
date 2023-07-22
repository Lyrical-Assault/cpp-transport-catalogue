#include "test_functions.h"

using namespace::std::literals::string_view_literals;

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
namespace tc_project::transport_catalogue::tests{

    void AddBus() {
        TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 33.67674, 44.98942);
        catalogue.AddStop("Stop 2", 22.08086, 11.84844);
        catalogue.AddStop("Stop 3", 55.57754, 66.70692);
        catalogue.AddBus("Bus 1", {"Stop 1", "Stop 2", "Stop 3"}, false);
        const auto bus_test = catalogue.FindBus("Bus 1");
        ASSERT_HINT(bus_test != nullptr, "Bus not added!");
        ASSERT_EQUAL_HINT(bus_test->name, "Bus 1", "Wrong bus name!");
        ASSERT_EQUAL_HINT(bus_test->stops.size(), 5, "Wrong number of stops!");
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
        catalogue.AddBus("Bus 1", {"Stop 1", "Stop 2", "Stop 3"}, false);
        const auto bus_test = catalogue.FindBus("Bus 1");
        ASSERT_HINT(bus_test != nullptr, "Bus not added!");
        ASSERT_EQUAL_HINT(bus_test->name, "Bus 1", "Wrong name of bus!");
        ASSERT_EQUAL_HINT(bus_test->stops.size(), 5, "Wrong number of stops!");
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
        RUN_TEST(SetDistance);
        RUN_TEST(GetDistance);
    }
}

namespace tc_project::request_handler::tests {

    void GetBusInfo() {
        map_renderer::MapRenderer render;
        transport_catalogue::TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.123, 37.456);
        catalogue.AddStop("Stop 2", 55.789, 37.890);
        catalogue.AddStop("Stop 3", 55.555, 37.777);
        catalogue.SetDistance("Stop 1", "Stop 2", 1000);
        catalogue.SetDistance("Stop 2", "Stop 3", 2000);
        catalogue.SetDistance("Stop 3", "Stop 1", 3000);
        catalogue.AddBus("750", {"Stop 1", "Stop 2", "Stop 3", "Stop 1"}, false);
        RequestHandler handler(catalogue, render);
        const auto info_test = handler.GetBusInfo("750");
        ASSERT_EQUAL_HINT(info_test->stops_count, 7, "Wrong stops number!");
        ASSERT_EQUAL_HINT(info_test->unique_stops_count, 3, "Wrong unique-stops number!");
        ASSERT_EQUAL_HINT(info_test->distance, 12000, "Wrong distance!");
        ASSERT_HINT(std::abs(info_test->curvature - 0.0379589) < 1e-6, "Wrong curvature!");
    }

    void GetBusesByStop() {
        map_renderer::MapRenderer render;
        transport_catalogue::TransportCatalogue catalogue;
        catalogue.AddStop("Stop 1", 55.123, 37.456);
        catalogue.AddStop("Stop 2", 55.789, 37.890);
        catalogue.AddStop("Stop 3", 55.555, 37.777);
        catalogue.AddBus("Bus 1", {"Stop 1", "Stop 2"}, false);
        catalogue.AddBus("Bus 2", {"Stop 2", "Stop 3"}, false);
        RequestHandler handler(catalogue, render);
        const auto& buses_by_stop = catalogue.GetBusesByStop();
        const auto& stop_info = handler.GetBusesByStop("Stop 1");
        const auto& test = &(buses_by_stop.find(catalogue.FindStop("Stop 1"))->second);
        ASSERT_HINT(test == stop_info, "Wrong buses at stop!");
        const auto& test2 = &(buses_by_stop.find(catalogue.FindStop("Stop 2"))->second);
        const auto& stop_info2 = handler.GetBusesByStop("Stop 2");
        ASSERT_HINT(test2 == stop_info2, "Wrong buses at stop!");
    }

    void RenderMap() {
        map_renderer::RenderSettings settings;
        settings = {600, 400, 50, 5, 14,
                    20, {7, 15},20, {7, -3},
                    "255, 255, 255, 0.85",  3, {"green", "255, 160, 0", "red"}};
        map_renderer::MapRenderer render(settings);
        transport_catalogue::TransportCatalogue catalogue;
        catalogue.AddStop("Rivierskiy most", 43.587795, 39.716901);
        catalogue.AddStop("Morskoy vokzal", 43.581969, 39.719848);
        catalogue.AddBus("114", {"Morskoy vokzal", "Rivierskiy most", "Morskoy vokzal"}, false);
        std::string svg = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                          "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"
                          "  <polyline points=\"201.751,350 50,50 201.751,350 50,50 201.751,350\" fill=\"none\" stroke=\"green\" stroke-width=\"5\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n"
                          "  <text fill=\"255, 255, 255, 0.85\" stroke=\"255, 255, 255, 0.85\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"201.751\" y=\"350\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n"
                          "  <text fill=\"green\" x=\"201.751\" y=\"350\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n"
                          "  <circle cx=\"201.751\" cy=\"350\" r=\"14\" fill=\"white\"/>\n"
                          "  <circle cx=\"50\" cy=\"50\" r=\"14\" fill=\"white\"/>\n"
                          "  <text fill=\"255, 255, 255, 0.85\" stroke=\"255, 255, 255, 0.85\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"201.751\" y=\"350\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Morskoy vokzal</text>\n"
                          "  <text fill=\"black\" x=\"201.751\" y=\"350\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Morskoy vokzal</text>\n"
                          "  <text fill=\"255, 255, 255, 0.85\" stroke=\"255, 255, 255, 0.85\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Rivierskiy most</text>\n"
                          "  <text fill=\"black\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Rivierskiy most</text>\n"
                          "</svg>";
        RequestHandler handler(catalogue, render);
        auto test = handler.RenderMap();
        std::ostringstream out;
        test.Render(out);
        ASSERT_EQUAL_HINT(svg, out.str(), "Different SVG file!");
    }

    void RequestHandlerTest() {
        RUN_TEST(GetBusInfo);
        RUN_TEST(GetBusesByStop);
        RUN_TEST(RenderMap);
    }
}

namespace json::tests {
    json::Document LoadJSON(const std::string& s) {
        std::istringstream strm(s);
        return json::Load(strm);
    }

    std::string Print(const Node& node) {
        std::ostringstream out;
        Print(json::Document{node}, out);
        return out.str();
    }

    void MustFailToLoad(const std::string& s) {
        try {
            LoadJSON(s);
            std::cerr << "ParsingError exception is expected on '"sv << s << "'"sv << std::endl;
            assert(false);
        } catch (const json::ParsingError&) {
            // ok
        } catch (const std::exception& e) {
            std::cerr << "exception thrown: "sv << e.what() << std::endl;
            assert(false);
        } catch (...) {
            std::cerr << "Unexpected error"sv << std::endl;
            assert(false);
        }
    }

    template <typename Fn>
    void MustThrowLogicError(Fn fn) {
        try {
            fn();
            std::cerr << "logic_error is expected"sv << std::endl;
            assert(false);
        } catch (const std::logic_error&) {
            // ok
        } catch (const std::exception& e) {
            std::cerr << "exception thrown: "sv << e.what() << std::endl;
            assert(false);
        } catch (...) {
            std::cerr << "Unexpected error"sv << std::endl;
            assert(false);
        }
    }

    void TestNull() {
        Node null_node;
        assert(null_node.IsNull());
        assert(!null_node.IsInt());
        assert(!null_node.IsDouble());
        assert(!null_node.IsPureDouble());
        assert(!null_node.IsString());
        assert(!null_node.IsArray());
        assert(!null_node.IsDict());

        Node null_node1{nullptr};
        assert(null_node1.IsNull());

        assert(Print(null_node) == "null"s);
        assert(null_node == null_node1);
        assert(!(null_node != null_node1));

        const Node node = LoadJSON("null"s).GetRoot();
        assert(node.IsNull());
        assert(node == null_node);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        assert(LoadJSON(" \t\r\n\n\r null \t\r\n\n\r "s).GetRoot() == null_node);
    }

    void TestNumbers() {
        const Node int_node{42};
        assert(int_node.IsInt());
        assert(int_node.AsInt() == 42);
        // целые числа являются подмножеством чисел с плавающей запятой
        assert(int_node.IsDouble());
        // Когда узел хранит int, можно получить соответствующее ему double-значение
        assert(int_node.AsDouble() == 42.0);
        assert(!int_node.IsPureDouble());
        assert(int_node == Node{42});
        // int и double - разные типы, поэтому не равны, даже когда хранят
        assert(int_node != Node{42.0});

        const Node dbl_node{123.45};
        assert(dbl_node.IsDouble());
        assert(dbl_node.AsDouble() == 123.45);
        assert(dbl_node.IsPureDouble());  // Значение содержит число с плавающей запятой
        assert(!dbl_node.IsInt());

        assert(Print(int_node) == "42"s);
        assert(Print(dbl_node) == "123.45"s);
        assert(Print(Node{-42}) == "-42"s);
        assert(Print(Node{-3.5}) == "-3.5"s);

        assert(LoadJSON("42"s).GetRoot() == int_node);
        assert(LoadJSON("123.45"s).GetRoot() == dbl_node);
        assert(LoadJSON("0.25"s).GetRoot().AsDouble() == 0.25);
        assert(LoadJSON("3e5"s).GetRoot().AsDouble() == 3e5);
        assert(LoadJSON("1.2e-5"s).GetRoot().AsDouble() == 1.2e-5);
        assert(LoadJSON("1.2e+5"s).GetRoot().AsDouble() == 1.2e5);
        assert(LoadJSON("-123456"s).GetRoot().AsInt() == -123456);
        assert(LoadJSON("0").GetRoot() == Node{0});
        assert(LoadJSON("0.0").GetRoot() == Node{0.0});
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        assert(LoadJSON(" \t\r\n\n\r 0.0 \t\r\n\n\r ").GetRoot() == Node{0.0});
    }

    void TestStrings() {
        Node str_node{"Hello, \"everybody\""s};
        assert(str_node.IsString());
        assert(str_node.AsString() == "Hello, \"everybody\""s);

        assert(!str_node.IsInt());
        assert(!str_node.IsDouble());

        assert(Print(str_node) == "\"Hello, \\\"everybody\\\"\""s);

        assert(LoadJSON(Print(str_node)).GetRoot() == str_node);
        const std::string escape_chars
                = R"("\r\n\t\"\\")"s;  // При чтении строкового литерала последовательности \r,\n,\t,\\,\"
        // преобразовываться в соответствующие символы.
        // При выводе эти символы должны экранироваться, кроме \t.
        assert(Print(LoadJSON(escape_chars).GetRoot()) == "\"\\r\\n\t\\\"\\\\\""s);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        assert(LoadJSON("\t\r\n\n\r \"Hello\" \t\r\n\n\r ").GetRoot() == Node{"Hello"s});
    }

    void TestBool() {
        Node true_node{true};
        assert(true_node.IsBool());
        assert(true_node.AsBool());

        Node false_node{false};
        assert(false_node.IsBool());
        assert(!false_node.AsBool());

        assert(Print(true_node) == "true"s);
        assert(Print(false_node) == "false"s);

        assert(LoadJSON("true"s).GetRoot() == true_node);
        assert(LoadJSON("false"s).GetRoot() == false_node);
        assert(LoadJSON(" \t\r\n\n\r true \r\n"s).GetRoot() == true_node);
        assert(LoadJSON(" \t\r\n\n\r false \t\r\n\n\r "s).GetRoot() == false_node);
    }

    void TestArray() {
        Node arr_node{Array{1, 1.23, "Hello"s}};
        assert(arr_node.IsArray());
        const Array& arr = arr_node.AsArray();
        assert(arr.size() == 3);
        assert(arr.at(0).AsInt() == 1);

        assert(LoadJSON("[1,1.23,\"Hello\"]"s).GetRoot() == arr_node);
        assert(LoadJSON(Print(arr_node)).GetRoot() == arr_node);
        assert(LoadJSON(R"(  [ 1  ,  1.23,  "Hello"   ]   )"s).GetRoot() == arr_node);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        assert(LoadJSON("[ 1 \r \n ,  \r\n\t 1.23, \n \n  \t\t  \"Hello\" \t \n  ] \n  "s).GetRoot()
               == arr_node);
    }

    void TestMap() {
        Node dict_node{Dict{{"key1"s, "value1"s}, {"key2"s, 42}}};
        assert(dict_node.IsDict());
        const Dict& dict = dict_node.AsDict();
        assert(dict.size() == 2);
        assert(dict.at("key1"s).AsString() == "value1"s);
        assert(dict.at("key2"s).AsInt() == 42);

        assert(LoadJSON("{ \"key1\": \"value1\", \"key2\": 42 }"s).GetRoot() == dict_node);
        assert(LoadJSON(Print(dict_node)).GetRoot() == dict_node);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        assert(
                LoadJSON(
                        "\t\r\n\n\r { \t\r\n\n\r \"key1\" \t\r\n\n\r: \t\r\n\n\r \"value1\" \t\r\n\n\r , \t\r\n\n\r \"key2\" \t\r\n\n\r : \t\r\n\n\r 42 \t\r\n\n\r } \t\r\n\n\r"s)
                        .GetRoot()
                == dict_node);
    }

    void TestErrorHandling() {
        MustFailToLoad("["s);
        MustFailToLoad("]"s);

        MustFailToLoad("{"s);
        MustFailToLoad("}"s);

        MustFailToLoad("\"hello"s);  // незакрытая кавычка

        MustFailToLoad("tru"s);
        MustFailToLoad("fals"s);
        MustFailToLoad("nul"s);

        Node dbl_node{3.5};
        MustThrowLogicError([&dbl_node] {
            dbl_node.AsInt();
        });
        MustThrowLogicError([&dbl_node] {
            dbl_node.AsString();
        });
        MustThrowLogicError([&dbl_node] {
            dbl_node.AsArray();
        });

        Node array_node{Array{}};
        MustThrowLogicError([&array_node] {
            array_node.AsDict();
        });
        MustThrowLogicError([&array_node] {
            array_node.AsDouble();
        });
        MustThrowLogicError([&array_node] {
            array_node.AsBool();
        });
    }

    void Benchmark() {
        const auto start = std::chrono::steady_clock::now();
        Array arr;
        arr.reserve(1'000);
        for (int i = 0; i < 1'000; ++i) {
            arr.emplace_back(Dict{
                    {"int"s, 42},
                    {"double"s, 42.1},
                    {"null"s, nullptr},
                    {"string"s, "hello"s},
                    {"array"s, Array{1, 2, 3}},
                    {"bool"s, true},
                    {"map"s, Dict{{"key"s, "value"s}}},
            });
        }
        std::stringstream strm;
        json::Print(json::Document{arr}, strm);
        const auto doc = json::Load(strm);
        assert(doc.GetRoot() == arr);
        const auto duration = std::chrono::steady_clock::now() - start;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"sv
                  << std::endl;
    }

    void Tests(){
        Node arr_node = {Array {true, "42"s, Array{}}};
        assert(arr_node.IsArray());
        const Array& arr = arr_node.AsArray();
        auto arr_json = "[true,\"42\", []]"s;
        auto arr_json_minify = "[true,\"42\",[]]"s;
        assert(LoadJSON(arr_json).GetRoot() == arr_node);
        assert(LoadJSON(arr_json_minify).GetRoot() == arr_node);

        Node dict_node = Dict{{{"42"s, 42}, {"4.2"s, 4.2}, {"true"s, true}, {"string"s, "string"s}, {"[]"s, Array{}}}};
        assert(dict_node.IsDict());
        const Dict& dict = dict_node.AsDict();
        auto dict_json = "{\"42\": 42, \"4.2\": 4.2, \"true\": true, \"string\": \"string\", \"[]\": []}"s;
        auto dict_json_minify = "{\"42\":42,\"4.2\":4.2,\"true\":true,\"string\":\"string\",\"[]\":[]}"s;
        assert((LoadJSON(dict_json).GetRoot() == dict_node));
        assert((LoadJSON(dict_json_minify).GetRoot() == dict_node));
    }

    void JsonTests(){
        RUN_TEST(TestNull);
        RUN_TEST(TestNumbers);
        RUN_TEST(TestStrings);
        RUN_TEST(TestBool);
        RUN_TEST(TestArray);
        RUN_TEST(TestMap);
        RUN_TEST(TestErrorHandling);
        RUN_TEST(Benchmark);
        RUN_TEST(Tests);
    };

}

namespace tc_project::json_reader::tests {

    void StopProcessing() {
        transport_catalogue::TransportCatalogue catalogue;
        catalogue.AddStop("Морской вокзал", 43.581969, 39.719848);
        std::map<std::string, std::map<std::string, ::json::Node>> road_distances{};
        std::string json = "{\n"
                           "      \"type\": \"Stop\",\n"
                           "      \"name\": \"Ривьерский мост\",\n"
                           "      \"latitude\": 43.587795,\n"
                           "      \"longitude\": 39.716901,\n"
                           "      \"road_distances\": {\"Морской вокзал\": 850}\n"
                           "    }";
        auto stop = json::tests::LoadJSON(json).GetRoot().AsDict();
        json_reader::StopProcessing(catalogue, stop, road_distances);
        auto stop_test = catalogue.FindStop("Ривьерский мост");
        ASSERT_HINT(stop_test != nullptr, "Stop not added!");
        ASSERT_EQUAL_HINT(stop_test->name, "Ривьерский мост", "Wrong stop name!");
        ASSERT_HINT(std::abs(stop_test->coordinates.lat - 43.587795) < 1e-6, "Wrong coordinate!");
        ASSERT_HINT(std::abs(stop_test->coordinates.lng - 39.716901) < 1e-6, "Wrong coordinate!");
    }

    void BusProcessing() {
        transport_catalogue::TransportCatalogue catalogue;
        catalogue.AddStop("Морской вокзал", 43.581969, 39.719848);
        catalogue.AddStop("Ривьерский мост", 43.587795, 39.716901);
        std::string json = "{\n"
                           "      \"type\": \"Bus\",\n"
                           "      \"name\": \"114\",\n"
                           "      \"stops\": [\"Морской вокзал\", \"Ривьерский мост\"],\n"
                           "      \"is_roundtrip\": false\n"
                           "    }";
        auto bus = json::tests::LoadJSON(json).GetRoot().AsDict();
        json_reader::BusProcessing(catalogue, bus);
        auto bus_test = catalogue.FindBus("114");
        ASSERT_HINT(bus_test != nullptr, "Bus not added!");
        ASSERT_EQUAL_HINT(bus_test->name, "114", "Wrong bus name!");
        ASSERT_EQUAL_HINT(bus_test->stops.size(), 3, "Wrong number of stops!");
        ASSERT_EQUAL_HINT(bus_test->stops[0]->name, "Морской вокзал", "Wrong stop!");
        ASSERT_EQUAL_HINT(bus_test->stops[1]->name, "Ривьерский мост", "Wrong stop!");
        ASSERT_EQUAL_HINT(bus_test->is_roundtrip, false, "Wrong trip type!");
    }

    void ParseStop() {
        transport_catalogue::TransportCatalogue catalogue;
        map_renderer::MapRenderer render;
        std::vector<json::Node> builder_data;
        catalogue.AddStop("Морской вокзал", 43.581969, 39.719848);
        catalogue.AddStop("Улица Докучаева", 43.585586, 39.733879);
        json_reader::ParseStop(catalogue, render, "Электросети", 1, builder_data);
        json_reader::ParseStop(catalogue, render, "Морской вокзал", 2, builder_data);
        catalogue.AddBus("14", {"Улица Докучаева", "Морской вокзал"}, true);
        json_reader::ParseStop(catalogue, render, "Улица Докучаева", 3, builder_data);
        auto test1 = json::Builder{}.StartDict().Key("request_id").Value(1).Key("error_message").Value("not found").EndDict().Build();
        auto test2 = json::Builder{}.StartDict().Key("request_id").Value(2).Key("buses").StartArray().EndArray().EndDict().Build();
        auto test3 = json::Builder{}.StartDict().Key("request_id").Value(3).Key("buses").StartArray().Value("14").EndArray().EndDict().Build();
        ASSERT_HINT(test1 == builder_data[0], "Wrong parsing!");
        ASSERT_HINT(test2 == builder_data[1], "Wrong parsing!");
        ASSERT_HINT(test3 == builder_data[2], "Wrong parsing!");
    }

    void ParseBus() {
        transport_catalogue::TransportCatalogue catalogue;
        map_renderer::MapRenderer render;
        std::vector<json::Node> builder_data;
        catalogue.AddStop("Морской вокзал", 43.581969, 39.719848);
        catalogue.AddStop("Улица Докучаева", 43.585586, 39.733879);
        catalogue.SetDistance("Морской вокзал", "Улица Докучаева", 850);
        catalogue.AddBus("14", {"Улица Докучаева", "Морской вокзал"}, true);
        json_reader::ParseBus(catalogue, render, "114", 1, builder_data);
        json_reader::ParseBus(catalogue, render, "14", 2, builder_data);
        auto test1 = json::Builder{}.StartDict().Key("request_id").Value(1).Key("error_message").Value("not found").EndDict().Build();
        auto test2 = json::Builder{}.StartDict().Key("request_id").Value(2).Key("curvature").Value(0.708586)
                                                      .Key("route_length").Value(850).Key("stop_count").Value(2)
                                                      .Key("unique_stop_count").Value(2).EndDict().Build();
        ASSERT_HINT(test1 == builder_data[0], "Wrong parsing!");
        for(const auto& [key, value] : builder_data[1].AsDict()){
            if(value.IsInt()){
                ASSERT_HINT(test2.AsDict().at(key).AsInt() == value.AsInt(), "Wrong parsing!");
            }
            if(value.IsDouble()){
                ASSERT_HINT(std::abs(test2.AsDict().at(key).AsDouble() - value.AsDouble()) < EPSILON, "Wrong parsing!");
            }
        }
    }

    void ParseMap() {
        transport_catalogue::TransportCatalogue catalogue;
        map_renderer::RenderSettings settings;
        settings = {600, 400, 50, 5, 14,
                    20, {7, 15},20, {7, -3},
                    "255, 255, 255, 0.85",  3, {"green", "255, 160, 0", "red"}};
        map_renderer::MapRenderer render(settings);
        std::vector<json::Node> builder_data;
        catalogue.AddStop("Морской вокзал", 43.581969, 39.719848);
        catalogue.AddStop("Улица Докучаева", 43.585586, 39.733879);
        catalogue.SetDistance("Морской вокзал", "Улица Докучаева", 850);
        catalogue.AddBus("14", {"Улица Докучаева", "Морской вокзал"}, true);
        json_reader::ParseMap(catalogue, render, 1, builder_data);
        auto test = json::Builder{}.StartDict().Key("request_id").Value(1).Key("map").Value( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"550,50 50,178.893\" fill=\"none\" stroke=\"green\" stroke-width=\"5\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <text fill=\"255, 255, 255, 0.85\" stroke=\"255, 255, 255, 0.85\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"550\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"green\" x=\"550\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <circle cx=\"50\" cy=\"178.893\" r=\"14\" fill=\"white\"/>\n  <circle cx=\"550\" cy=\"50\" r=\"14\" fill=\"white\"/>\n  <text fill=\"255, 255, 255, 0.85\" stroke=\"255, 255, 255, 0.85\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"178.893\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Морской вокзал</text>\n  <text fill=\"black\" x=\"50\" y=\"178.893\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Морской вокзал</text>\n  <text fill=\"255, 255, 255, 0.85\" stroke=\"255, 255, 255, 0.85\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"550\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Улица Докучаева</text>\n  <text fill=\"black\" x=\"550\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Улица Докучаева</text>\n</svg>").EndDict().Build();
        ASSERT_HINT(test == builder_data[0], "Wrong parsing!");
    }

    void JsonReader() {
        RUN_TEST(StopProcessing);
        RUN_TEST(BusProcessing);
        RUN_TEST(ParseStop);
        RUN_TEST(ParseBus);
        RUN_TEST(ParseMap);
    }
}