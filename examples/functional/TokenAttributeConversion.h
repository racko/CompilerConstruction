#pragma once

#include <memory>

namespace Functional {
struct Attribute;
struct Token;

Attribute* toAttribute(const std::unique_ptr<Token>& t);
} // namespace Functional
