#include "catch.hpp"
#include "sp/SPTokenizer/LexicalToken.h"

// Test the constructor with only token type.
TEST_CASE("Constructor with only type", "[LexicalToken]")
{
    LexicalToken token(LexicalTokenType::SYMBOL_OPEN_BRACE,"(");
    REQUIRE(token.getTokenType() == LexicalTokenType::SYMBOL_OPEN_BRACE);
}

// Test getting the token type.
TEST_CASE("Get TokenType", "[LexicalToken]")
{
    LexicalToken token(LexicalTokenType::OPERATOR_PLUS,"+");
    REQUIRE(token.getTokenType() == LexicalTokenType::OPERATOR_PLUS);
}
