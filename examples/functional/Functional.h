#pragma once

//#include <cstdint>
//#include <iosfwd>
//#include <string>

struct LambdaExpr;

const LambdaExpr* reduce(const LambdaExpr& p);
const LambdaExpr* run(const char* text);
