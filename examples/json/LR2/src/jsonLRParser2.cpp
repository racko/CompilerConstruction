#include "jsonLRParser2.h"

namespace jsonLR2 {
json::value* Attributes::new_value() {
    if (next_value_index == 10000) {
        next_value_index = 0U;
        values.emplace_back();
    }
    return &values.back()[next_value_index++];
}

json::value* Attributes::reduce(NonterminalID A_, uint32_t production, json::value** alpha, size_t) {
    json::NonterminalID A(A_.x);
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
            *val = json::object(&boost::get<json::array>(*alpha[1]), alpha[2]);
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
            *val = json::object(&boost::get<json::array>(*alpha[1]), alpha[2]);
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

json::value* Attributes::shift(Token&& token) {
    switch (token.first) {
    case json::T::NUM: {
        auto val = new_value();
        *val = boost::get<num_view>(token.second);
        return val;
    }
    case json::T::STRING: {
        auto val = new_value();
        *val = boost::get<std::string_view>(token.second);
        return val;
    }
    case json::T::TRUE:
    case json::T::FALSE: {
        auto val = new_value();
        *val = boost::get<bool>(token.second);
        return val;
    }
    case json::T::NULL: {
        auto val = new_value();
        *val = boost::blank();
        return val;
    }
    default:
        return nullptr;
    }
}
} // namespace jsonLR2
