#include <catch.hpp>

#include "Regex/variant.h" // for null
#include "json.h"
#include "json_token.h"                                 // for operator<<
#include "num_view.h"                                   // for num_view
#include <boost/blank.hpp>                              // for blank
#include <boost/variant/detail/apply_visitor_unary.hpp> // for apply_visitor
#include <boost/variant/get.hpp>                        // for get
#include <boost/variant/recursive_variant.hpp>          // for make_recursi...
#include <boost/variant/static_visitor.hpp>             // for static_visit...
#include <boost/variant/variant.hpp>                    // for variant
#include <catch.hpp>                                    // for AssertionHan...
#include <jsonValue.h>                                  // for array, object
#include <ostream>                                      // for operator<<
#include <string>                                       // for string, char...
#include <string_view>                                  // for string_view
#include <type_traits>                                  // for is_base_of_v
#include <unordered_map>                                // for unordered_map
#include <unordered_set>                                // for unordered_set
#include <utility>                                      // for pair
#include <vector>                                       // for vector
namespace boost {
struct recursive_variant_;
}

//#include "json.h"
#include <jsonLLParser.h>
#include <jsonLRParser.h>
#include <jsonLRParser2.h>
//#include <jsonValue.h>

namespace json {
static_assert(std::is_base_of_v<JsonParser::Parser, jsonLR2::Parser>);
using test_value_ = boost::make_recursive_variant<null,
                                                  bool,
                                                  num_view,
                                                  std::string_view,
                                                  std::vector<boost::recursive_variant_>,
                                                  std::unordered_map<std::string, boost::recursive_variant_>>::type;

using arr = std::vector<test_value_>;
using obj = std::unordered_map<std::string, test_value_>;
using str = std::string_view;
using num = num_view;

struct test_value : public test_value_ {
    using test_value_::test_value_;
    friend std::ostream& operator<<(std::ostream& s, const test_value& v);
    friend bool operator==(const test_value& a, const test_value& b);
};

bool operator==(const test_value& a, const test_value& b) {
    return static_cast<test_value_>(a) == static_cast<test_value_>(b);
}

std::ostream& operator<<(std::ostream& s, const test_value& v) {
    struct Printer : public boost::static_visitor<> {
        std::ostream& s_;
        Printer(std::ostream& s) : s_(s) {}
        void operator()(null) const { s_ << "null"; }
        void operator()(bool b) const { s_ << std::boolalpha << b << std::noboolalpha; }
        void operator()(const num_view& n) const { s_ << n.x; }
        void operator()(const std::string_view& ss) const { s_ << '"' << ss << '"'; }
        void operator()(const arr& a) const {
            s_ << '[';
            auto it = a.begin();
            if (it != a.end()) {
                boost::apply_visitor(*this, *it++);
                for (; it != a.end(); ++it) {
                    s_ << ',';
                    boost::apply_visitor(*this, *it);
                }
            }
            s_ << ']';
        }
        void operator()(const obj& o) const {
            s_ << '{';
            auto it = o.begin();
            if (it != o.end()) {
                s_ << '"' << it->first << "\":";
                boost::apply_visitor(*this, it->second);
                ++it;
                for (; it != o.end(); ++it) {
                    s_ << ',';
                    s_ << '"' << it->first << "\":";
                    boost::apply_visitor(*this, it->second);
                }
            }
            s_ << '}';
        }
    };
    boost::apply_visitor(Printer(s), v);
    return s;
}

test_value to_test_value(const value& v) {
    struct Converter : public boost::static_visitor<test_value> {
        test_value operator()(null) const { return null(); }
        test_value operator()(bool b) const { return b; }
        test_value operator()(const num_view& n) const { return n; }
        test_value operator()(const std::string_view& s) const { return s; }
        test_value operator()(const array& a) const {
            arr out;
            if (a.first != nullptr) {
                out.push_back(boost::apply_visitor(*this, *a.first));
                auto it = a.second;
                while (it != nullptr) {
                    auto& a_ = boost::get<array>(*it);
                    out.push_back(boost::apply_visitor(*this, *a_.first));
                    it = a_.second;
                }
            }
            return out;
        }
        test_value operator()(const object& o) const {
            obj out;
            if (o.first != nullptr) {
                out.emplace(boost::get<std::string_view>(boost::apply_visitor(*this, *o.first->first)),
                            boost::apply_visitor(*this, *o.first->second));
                auto it = o.second;
                while (it != nullptr) {
                    auto& o_ = boost::get<object>(*it);
                    out.emplace(boost::get<std::string_view>(boost::apply_visitor(*this, *o_.first->first)),
                                boost::apply_visitor(*this, *o_.first->second));
                    it = o_.second;
                }
            }
            return out;
        }
    };
    return boost::apply_visitor(Converter(), v);
}

template <typename ParserImpl>
JsonParser& GetParser() {
    static JsonParser parser{std::make_unique<ParserImpl>()};
    return parser;
}

// @FIXME: \xFF is EOF. Currently necessary because the lexer requires EOF ...

TEMPLATE_TEST_CASE("Json", "[json][!mayfail]", jsonLL::Parser, jsonLR::Parser, jsonLR2::Parser) {
    auto run = [](const std::string& text) { return GetParser<TestType>().run(text.c_str()); };
    SECTION("Empty object") {
        std::string text = "{}\xFF";
        auto result = run(text);
        REQUIRE(result == nullptr); // parser returns nullptr for empty objects
    }

    SECTION("Empty array") {
        std::string text = "[]\xFF";
        auto result = run(text);
        REQUIRE(result == nullptr); // parser returns nullptr for empty arrays
    }

    SECTION("null") {
        std::string text = "null\xFF";
        auto result = run(text);
        REQUIRE(result != nullptr);
        REQUIRE(*result == value(null()));
    }

    SECTION("0") {
        std::string text = "0\xFF";
        auto result = run(text);
        REQUIRE(result != nullptr);
        REQUIRE(*result == value(num_view("0")));
    }

    SECTION("true") {
        std::string text = "true\xFF";
        auto result = run(text);
        REQUIRE(result != nullptr);
        REQUIRE(*result == value(true));
    }

    SECTION("false") {
        std::string text = "false\xFF";
        auto result = run(text);
        REQUIRE(result != nullptr);
        REQUIRE(*result == value(false));
    }

    SECTION("Empty string") {
        std::string text = "\"\"\xFF";
        auto result = run(text);
        REQUIRE(result != nullptr);
        REQUIRE(*result == value(std::string_view("")));
    }

    SECTION("Singleton array: [null]") {
        std::string text = "[null]\xFF";
        auto result = run(text);
        REQUIRE(result != nullptr);
        auto actual = to_test_value(*result);

        auto expected = test_value(arr({null()}));

        REQUIRE(actual == expected);
    }

    SECTION("Test1") {
        std::string text =
            R"raw({ "totalCount_str": "177895", "pageCount": 18, "items": [ { "buy": false, "issued": "2016-04-02T01:40:43", "price": 3000, "volumeEntered": 833, "stationID": 60006484, "volume": 833, "range": "region", "minVolume": 1, "duration": 365, "type": 366699, "id": 4020240384 }, { "buy": false, "issued": "2016-04-02T01:50:24", "price": 3000, "volumeEntered": 833, "stationID": 60006487, "volume": 833, "range": "region", "minVolume": 1, "duration": 365, "type": 366699, "id": 4020240385 } ], "next": { "href": "https://api-sisi.testeveonline.com/market/10000001/orders/all/?page=2" }, "totalCount": 177895, "pageCount_str": "18" })raw";
        text += '\xFF';
        auto result = run(text);
        REQUIRE(result != nullptr);
        auto actual = to_test_value(*result);

        auto expected = test_value(obj({
            {"totalCount_str", str("177895")},
            {"pageCount", num("18")},
            {"items",
             arr({obj({
                      {"buy", false},
                      {"issued", str("2016-04-02T01:40:43")},
                      {"price", num("3000")},
                      {"volumeEntered", num("833")},
                      {"stationID", num("60006484")},
                      {"volume", num("833")},
                      {"range", str("region")},
                      {"minVolume", num("1")},
                      {"duration", num("365")},
                      {"type", num("366699")},
                      {"id", num("4020240384")},
                  }),
                  obj({
                      {"buy", false},
                      {"issued", str("2016-04-02T01:50:24")},
                      {"price", num("3000")},
                      {"volumeEntered", num("833")},
                      {"stationID", num("60006487")},
                      {"volume", num("833")},
                      {"range", str("region")},
                      {"minVolume", num("1")},
                      {"duration", num("365")},
                      {"type", num("366699")},
                      {"id", num("4020240385")},
                  })})},
            {"next", obj({{"href", str("https://api-sisi.testeveonline.com/market/10000001/orders/all/?page=2")}})},
            {"totalCount", num("177895")},
            {"pageCount_str", str("18")},
        }));

        REQUIRE(actual == expected);
    }
}
} // namespace json
