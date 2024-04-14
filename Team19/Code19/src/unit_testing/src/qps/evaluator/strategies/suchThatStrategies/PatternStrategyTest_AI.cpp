#include "catch.hpp"
#include "pkb/PKBReaderManager.h"
#include "qps/parser/ParsingResult.h"
#include "qps/parser/QueryParser.h"
#include "pkb/PKBManager.h"
#include "qps/evaluator/QueryEvaluator.h"

TEST_CASE("src/qps/evaluator/suchThatStrategies/PatternStrategy") {
    std::shared_ptr<PKBManager> pkbManager = std::make_shared<PKBManager>();
    std::shared_ptr<PKBReaderManager> pkbReaderManager = pkbManager->getPKBReaderManager();
    std::shared_ptr<PKBCacheManager> pkbCacheManager = pkbManager->getPKBCacheManager();
    std::shared_ptr<PKBWriterManager> pkbWriterManager = pkbManager->getPKBWriterManager();

    // Preparing PKB with required data for the test
    std::shared_ptr<StatementWriter> statementWriter = pkbWriterManager->getStatementWriter();
    std::shared_ptr<AssignPatternWriter> assignPatternWriter = pkbWriterManager->getAssignPatternWriter();
    std::shared_ptr<VariableWriter> variableWriter = pkbWriterManager->getVariableWriter();
    std::shared_ptr<AssignWriter> assignWriter = pkbWriterManager->getAssignWriter();

    // Inserting statements and variables for the pattern
    statementWriter->insertStatement(1);
    statementWriter->insertStatement(2);
    statementWriter->insertStatement(3);
    statementWriter->insertStatement(4);
    statementWriter->insertStatement(6); // Adding statement 6 as expected

    std::string minus = "minus";
    std::string print = "print";
    variableWriter->insertVariable(minus);
    variableWriter->insertVariable(print);

    assignWriter->insertAssign(6); // Linking statement number 6 as an assignment
    assignPatternWriter->addAssignPattern(6, "a", "'print''minus''-'"); // Example pattern

    SECTION("Pattern matching with assignments selecting 'a'") {
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "assign"),
                Token(TokenType::IDENT, "a"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "a"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "a"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::QuoutIDENT, "\"print\""),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };

        QueryParser parser(tokens);
        auto parsingResult = parser.parse();
        QueryEvaluator evaluator(pkbReaderManager, pkbCacheManager, parsingResult);
        std::unordered_set<std::string> res = evaluator.evaluateQuery();

        REQUIRE(res == std::unordered_set<std::string>{"6"}); // Statement 6 expected to match the pattern
    }

    SECTION("QueryParser correctly parses Pattern with expressionSpec") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "assign"),
                Token(TokenType::IDENT, "a"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "a"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "a"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Comma, ","),
                Token(TokenType::ExpressionSpec, "\"x+1\""),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.isQueryValid() == true);
    }


    SECTION("QueryParser correctly parses Pattern with while syn") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "while"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "w");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "w");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "while");
        REQUIRE(patternClause.getFirstParam().getValue() == "\"x\"");
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue().empty());
    }

    SECTION("QueryParser throw semantic error when parsing Pattern with QuoutIDENT and wildcard") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "while"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Comma, ","),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "SyntaxError");
        REQUIRE(parsingResult.isQueryValid() == false);

    }


    SECTION("QueryParser correctly parses while Pattern with while syn with wildcards") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "while"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "w");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "w");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "while");
        REQUIRE(patternClause.getFirstParam().getValue() == "_");
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue().empty());

    }


    SECTION("QueryParser correctly parses while Pattern with while syn") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "while"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "w");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "w");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "while");
        REQUIRE(patternClause.getFirstParam().getValue() == "_");
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue().empty());


    }

    SECTION("QueryParser correctly parses while Pattern with var syn and wildcards") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "while"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::DesignEntity, "variable"),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "v");
        REQUIRE(parsingResult.getRequiredSynonymType(parsingResult.getRequiredSynonyms()[0]) == "variable");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "w");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "while");
        REQUIRE(patternClause.getFirstParam().getValue() == "v");
        REQUIRE(patternClause.getFirstParam().getType() == TokenType::IDENT);
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue().empty());

    }

    SECTION("QueryParser correctly parses while Pattern with var syn and wildcards") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "while"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::DesignEntity, "variable"),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "w"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "v");
        REQUIRE(parsingResult.getRequiredSynonymType(parsingResult.getRequiredSynonyms()[0]) == "variable");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "w");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "while");
        REQUIRE(patternClause.getFirstParam().getValue() == "v");
        REQUIRE(patternClause.getFirstParam().getType() == TokenType::IDENT);
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue().empty());

    }

    SECTION("QueryParser correctly parses Pattern with if syn") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "if"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "ifs");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "ifs");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "if");
        REQUIRE(patternClause.getFirstParam().getValue() == "\"x\"");
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue() == "_");
    }


    SECTION("QueryParser throw semantic error when parsing if Pattern with QuoutIDENT without wildcard") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "if"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Comma, ","),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Comma, ","),
                Token(TokenType::QuoutIDENT, "\"x\""),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "SyntaxError");
        REQUIRE(parsingResult.isQueryValid() == false);

    }

    SECTION("QueryParser correctly parses while Pattern with if syn with wildcards") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "if"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "ifs");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "ifs");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "if");
        REQUIRE(patternClause.getFirstParam().getValue() == "_");
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue() == "_");

    }

    SECTION("QueryParser correctly parses if Pattern with var syn and wildcards") {
        // Manually create the vector of tokens for the query
        std::vector<Token> tokens = {
                Token(TokenType::DesignEntity, "if"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::DesignEntity, "variable"),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::Semicolon, ";"),
                Token(TokenType::SelectKeyword, "Select"),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::PatternKeyword, "pattern"),
                Token(TokenType::IDENT, "ifs"),
                Token(TokenType::Lparenthesis, "("),
                Token(TokenType::IDENT, "v"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Comma, ","),
                Token(TokenType::Wildcard, "_"),
                Token(TokenType::Rparenthesis, ")")
        };
        QueryParser queryParser(tokens);
        // Parse the query
        ParsingResult parsingResult = queryParser.parse();


        // Verify that the parsing result indicates a valid query with no errors
        REQUIRE(parsingResult.getErrorMessage() == "");
        REQUIRE(parsingResult.isQueryValid() == true);
        REQUIRE(parsingResult.getRequiredSynonyms()[0] == "v");
        REQUIRE(parsingResult.getRequiredSynonymType(parsingResult.getRequiredSynonyms()[0]) == "variable");

        PatternClause patternClause = parsingResult.getPatternClauses()[0];
        REQUIRE(patternClause.getRelationship().getValue() == "ifs");
        REQUIRE(parsingResult.getPatternClauseType(patternClause) == "if");
        REQUIRE(patternClause.getFirstParam().getValue() == "v");
        REQUIRE(patternClause.getFirstParam().getType() == TokenType::IDENT);
        REQUIRE(patternClause.getSecondParam().getValue() == "_");
        REQUIRE(patternClause.getThirdParam().getValue() == "_");

    }
}




