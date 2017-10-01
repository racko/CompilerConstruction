#pragma once

struct Attribute;
namespace Functional { struct Token; }

Attribute* toAttribute(const Functional::Token& t);
