#include "jsonParser2.h"

value* Attributs::new_value() {
    if (next_value_index == 10000) {
        next_value_index = 0U;
        values.emplace_back();
    }
    return &values.back()[next_value_index++];
}

value* Attributes::reduce(NonterminalID<json::Grammar> A, uint32_t production, value** alpha, size_t) {
    switch(A) {
    case NT::START:
        return alpha[0];
    case NT::JSON_TEXT:
        return alpha[0];
    case NT::VALUE:
        return alpha[0];
    case NT::OBJECT:
        switch(production) {
        case 0: {
            auto val = new_value();
            *val = object(&boost::get<array>(*alpha[1]), alpha[2]);
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
            *val = array(alpha[1], alpha[2]);
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
        switch(production) {
        case 0: {
            return nullptr;
        }
        case 1: {
            auto val = new_value();
            *val = object(&boost::get<array>(*alpha[1]), alpha[2]);
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
            *val = array(alpha[1], alpha[2]);
            return val;
        }
        default:
            throw std::runtime_error("wrong production id");
        }
        break;
    case NT::MEMBER: {
            auto val = new_value();
            *val = array(alpha[0], alpha[2]);
            return val;
        }
    default:
        throw std::runtime_error("Unhandled reduction");
    }
}

value* Attributes::shift(Token&& token) {
    switch (token.first) {
        case json::T::NUM: {
            auto val = new_value();
            *val = boost::get<json::num_view>(token.second);
            return val;
        }
        case json::T::STRING: {
            auto val = new_value();
            *val = boost::get<boost::string_view>(token.second);
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
