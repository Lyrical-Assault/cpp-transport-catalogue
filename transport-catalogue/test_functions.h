#pragma once

#include <numeric>
#include <cassert>
#include <stdexcept>
#include <string_view>
#include <iostream>
#include <iomanip>
#include "transport_catalogue.h"

using std::string_literals::operator""s;

namespace transport_catalogue::tests {

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

#define RUN_TEST(func)  RunTestImpl(func, #func)

    template<typename Func>
    void RunTestImpl(Func func, const std::string &str_func) {
        func();
        std::cerr << str_func << " OK"s << std::endl;
    }

    void
    AssertImpl(bool value, const std::string &expr_str, const std::string &file, const std::string &func, unsigned line,
               const std::string &hint);

    template<typename T, typename U>
    void
    AssertEqualImpl(const T &t, const U &u, const std::string &t_str, const std::string &u_str, const std::string &file,
                    const std::string &func, unsigned line, const std::string &hint) {
        if (t != u) {
            std::cerr << std::boolalpha;
            std::cerr << file << "("s << line << "): "s << func << ": "s;
            std::cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
            std::cerr << t << " != "s << u << "."s;
            if (!hint.empty()) {
                std::cerr << " Hint: "s << hint;
            }
            std::cerr << std::endl;
            abort();
        }
    }
        void AddStop();
        void AddBus();
        void FindStop();
        void FindBus();
        void GetBusInfo();
        void GetStopInfo();
        void SetDistance();
        void GetDistance();
        void TransportCatalogueTest();
}