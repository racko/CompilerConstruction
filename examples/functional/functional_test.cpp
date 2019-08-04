#include <catch.hpp>

#include "Functional.h"
#include "FunctionalAttribute.h"

namespace Functional {
TEST_CASE("Test0", "[Functional]") {
    std::string text = "main = 2;\xFF"; // @FIXME: \xFF is EOF. Currently necessary because the lexer requires EOF ...
    auto result = run(text.c_str());
    REQUIRE(*result == NumConst(2));
}

TEST_CASE("Test1", "[Functional]") {
    std::string text = "a = 1; b = 2; const x y = x; flip f x y = f y x; pair x y = \\f . f x y; fst p = p const; snd "
                       "p = p (flip const); main = snd (pair a b);\xFF"; // @FIXME: \xFF is EOF. Currently necessary
                                                                         // because the lexer requires EOF ...
    auto result = run(text.c_str());
    REQUIRE(*result == NumConst(2));
}

// main = (S (S O))
//  (λ x . (λ xN . (x) ((((λ xNNSFF . (λ xNNSFFN . (xNNSFF) ((((λ
//  xNNSFFNNSFF . (λ xNNSFFNNSFFN . xNNSFFNNSFFN))) (xNNSFF)) (xNNSFFN))))) (x))
//  (xN))))
//
// main = dec (S (S (S O)))
//  (λ x . (λ xN . (x) ((((λ xNNSFF . (λ xNNSFFN . (xNNSFF) ((((λ
//  xNNSFFNNSFF . (λ xNNSFFNNSFFN . (xNNSFFNNSFF) ((((λ xNNSFFNNSFFNNSFF . (λ
//  xNNSFFNNSFFNNSFFN . xNNSFFNNSFFNNSFFN))) (xNNSFFNNSFF)) (xNNSFFNNSFFN)))))
//  (xNNSFF)) (xNNSFFN))))) (x)) (xN))))
//
// After changing the 0 ("zero") in dec to O ("oh") ... no change!?:
// main = dec (S (S (S O)))
//  (λ x . (λ xN . (x) ((((λ xNNSFF . (λ xNNSFFN . (xNNSFF) ((((λ
//  xNNSFFNNSFF . (λ xNNSFFNNSFFN . (xNNSFFNNSFF) ((((λ xNNSFFNNSFFNNSFF . (λ
//  xNNSFFNNSFFNNSFFN . xNNSFFNNSFFNNSFFN))) (xNNSFFNNSFF)) (xNNSFFNNSFFN)))))
//  (xNNSFF)) (xNNSFFN))))) (x)) (xN))))

TEST_CASE("Test2", "[Functional][!mayfail]") {
    std::string text =
        "const x y = x; flip f x y = f y x; pair x y = \\f . f x y; fst p = p const; snd p = p (flip const); O = \\a b "
        ". b; S x = \\a b . a (x a b); inc x = S x; dec x = snd (x (\\p . pair (inc (fst p)) (fst p)) (pair O "
        "undefined)); main = dec (S (S (S O)));\xFF"; // @FIXME: \xFF is EOF. Currently necessary because the lexer
                                                      // requires EOF ...
    auto result = run(text.c_str());
    REQUIRE(*result == App(new Variable("S"),
                           new App(new Variable("S"),
                                   new Variable("O")))); // @TODO that's currently not the result of the run call :(
    // REQUIRE(*result == *reduce(App(new Variable("S"), new App(new Variable("S"), new Variable("O"))))); // @TODO
    // reduce currently doesn't work. More in definition of reduce.
}
} // namespace Functional
