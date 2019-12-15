#include "jsonLRParser.h"

#include "Regex/variant.h" // for get, null
#include <stdexcept>       // for runtime_error
#include <string_view>     // for string_view
#include <vector>          // for vector

struct num_view;

namespace jsonLR {
Parser::Parser() = default;
Parser::~Parser() = default;

json::value* Parser::new_value() {
    if (next_value_index == 10000) {
        next_value_index = 0U;
        values.emplace_back();
    }
    return &values.back()[next_value_index++];
}

json::value* Parser::reduce(json::NonterminalID A, uint32_t production, json::value** alpha, size_t) {
    switch (A) {
    case NT::START:
        return alpha[0];
    case NT::JSON_TEXT:
        return alpha[0];
    case NT::VALUE:
        return alpha[0];
    case NT::OBJECT:
        switch (production) {
        case 0: {
            auto val = new_value();
            *val = json::object(&get<json::array>(*alpha[1]), alpha[2]);
            return val;
        }
        case 1: {
            return nullptr;
        }
        default:
            throw std::runtime_error("wrong production id");
        }
        break;
    case NT::ARRAY:
        switch (production) {
        case 0: {
            auto val = new_value();
            *val = json::array(alpha[1], alpha[2]);
            return val;
        }
        case 1: {
            return nullptr;
        }
        default:
            throw std::runtime_error("wrong production id");
        }
        break;
    case NT::MEMBERS:
        switch (production) {
        case 0: {
            return nullptr;
        }
        case 1: {
            auto val = new_value();
            *val = json::object(&get<json::array>(*alpha[1]), alpha[2]);
            return val;
        }
        default:
            throw std::runtime_error("wrong production id");
        }
        break;
    case NT::VALUES:
        switch (production) {
        case 0: {
            return nullptr;
        }
        case 1: {
            auto val = new_value();
            *val = json::array(alpha[1], alpha[2]);
            return val;
        }
        default:
            throw std::runtime_error("wrong production id");
        }
        break;
    case NT::MEMBER: {
        auto val = new_value();
        *val = json::array(alpha[0], alpha[2]);
        return val;
    }
    default:
        throw std::runtime_error("Unhandled reduction");
    }
}

json::value* Parser::shift(Token&& token) {
    switch (token.first) {
    case T::NUM: {
        auto val = new_value();
        *val = get<num_view>(token.second);
        return val;
    }
    case T::STRING: {
        auto val = new_value();
        *val = get<std::string_view>(token.second);
        return val;
    }
    case T::TRUE:
    case T::FALSE: {
        auto val = new_value();
        *val = get<bool>(token.second);
        return val;
    }
    case T::NIL: {
        auto val = new_value();
        *val = null();
        return val;
    }
    default:
        return nullptr;
    }
}
} // namespace jsonLR
