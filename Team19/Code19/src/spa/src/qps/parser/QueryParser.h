#pragma once

// Include necessary headers.
#include <vector>
#include <string>
#include <unordered_set>
#include "qps/parser/Token.h"
#include "qps/parser/Tokenizer.h"
#include "qps/parser/ParsingResult.h"

// The parser class is used for parsing a sequence of tokens.
class QueryParser {
public:
    // Initializes the parser with a vector of tokens to be parsed.
    // The 'explicit' keyword prevents implicit conversions from vector<Token> to parser.
    explicit QueryParser(const std::vector<Token>& tokens);

    // The parse method initiates the parsing process.
    // Returns true if parsing is successful, false otherwise.
    ParsingResult parse();

    ParsingResult getParsingResult();  // Method to retrieve the result
private:
    bool semanticError;
    void checkValidOperand(const std::string& operand);
    void checkBracketsBalanced(const std::string& expr);
    void checkExprSyntax(const std::string& expr);
    void checkValidExpr(const std::string& expr);
    void throwSyntaxError();
    ParsingResult makeResult(ParsingResult parsingResult);
	void setSemanticError();
    const std::unordered_map<TokenType, std::unordered_set<std::string>> validStmtSynonymsMap = {
            {TokenType::Parent, {"stmt", "assign", "while", "if", "print", "read", "call"}},
            {TokenType::ParentT, {"stmt", "assign", "while", "if", "print", "read", "call"}},
            {TokenType::Follows, {"stmt", "assign", "while", "if", "print", "read", "call"}},
            {TokenType::FollowsT, {"stmt", "assign", "while", "if", "print", "read", "call"}},
            {TokenType::Uses, {"stmt", "print", "while", "if", "assign", "call", "read"}},
            {TokenType::Modifies, {"stmt", "print", "while", "if", "assign", "call", "read"}},
            {TokenType::Next, {"stmt", "assign", "while", "if", "print", "read", "call"}},
            {TokenType::NextT, {"stmt", "assign", "while", "if", "print", "read", "call"}},
            {TokenType::Affects, {"stmt", "assign", "while", "if", "print", "read", "call"}},
    };


    const std::unordered_map<std::string, std::unordered_set<std::string>> validAttrMap = {
            {"stmt", {"stmt#"}},
            {"assign", {"stmt#"}},
            {"while", {"stmt#"}},
            {"if", {"stmt#"}},
            {"print", {"stmt#", "varName"}},
            {"read", {"stmt#", "varName"}},
            {"call", {"stmt#", "procName"}},
            {"variable", {"varName"}},
            {"procedure", {"procName"}},
            {"constant", {"value"}}
    };

    const std::unordered_map<std::string, TokenType> attrToTypeMap = {
            {"stmt#", TokenType::INTEGER},
            {"varName", TokenType::QuoutIDENT},
            {"procName", TokenType::QuoutIDENT},
            {"value", TokenType::INTEGER}
    };
    // Vector of tokens to be parsed.
    std::vector<Token> tokens;

    // Current index in the tokens vector.
    size_t currentTokenIndex;
    ParsingResult parsingResult;

    Token currentSuchThatToken;

    // Private methods for parsing different parts of the input.
    void parseDeclarations();
    void parseSelectClause();
    void parseSelectTuple();
    void parseSelectSingle();
    void parseSelectTupleElement();
    void parseSuchThatClause();
    void parsePatternClause();
    void parseWithClause();

    bool isUsesOrModifies();
    bool isStmtRefStmtRef();
    bool isCalls();
    bool isVarName();
    bool isConstValue();

    void parseSynonym();
    void parseEntRef();
    void parseExpressionSpec(PatternClause &clause);
    void parseRelRef(SuchThatClause& clause);
    void parseStmtRef();
    void parseDesignEntity();
    void parseUsesOrModifies(SuchThatClause& clause);
    void parseStmtRefStmtRef(SuchThatClause& clause);
    void parseCalls(SuchThatClause& clause);
    void parseExpression();
    void parseTerm();
    void parseFactor();
    TokenType parseRef();
    void parseAttrRef();
    void parseAttr();


    // Method to get the current token.
    // Returns a constant reference to the current Token object.
    const Token& currentToken() const;

    // Method to advance to the next token in the sequence.
    void advanceToken();

    // Method to check if the next token matches a given TokenType.
    bool peekNextToken(TokenType type);

    // Method to check if the current token matches a given TokenType.
    // Returns true if it matches, false otherwise.
    bool match(TokenType type);

    void parseEntSynonym();
    void parseStmtSynonyms();
    void parsePatternSynonym();
    void ensureSynonymType(std::string synType);

    // helper function to replace concatenation of tokens
    std::string concatTokens(size_t start, size_t end);

    void ensureToken(TokenType expected);
    bool checkValidStmtNum();
    bool checkValidAttr(Token synToken);
    bool checkIfStmt();
    bool checkIfEnt();
    bool checkIfPatternSyn();
    void ensureNextBlank();
    void ensureCorrectPatternParams(PatternClause &clause);
    void checkForExistingSyntaxError();

};
