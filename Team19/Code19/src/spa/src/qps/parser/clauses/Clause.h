#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include "qps/parser/Token.h"

struct Clause {
    public:
    enum ClauseOperations {
        NOT,
        AND,
    };
    virtual ~Clause() = default;
    bool isNegated() const { return negated; }
    void setAsNegated() {
        negated = true;
        clauseOperation = NOT;
    }
    ClauseOperations getClauseOperation() const { return clauseOperation; }
    virtual std::string getTypeName() const = 0;
    std::vector<Token> getAllParams() const {
        return {firstParam, secondParam, relationship};
         }
    Token getRelationship() const { return relationship; }
    Token getFirstParam() const { return firstParam; }
    TokenType getFirstParamType() const { return firstParam.getType(); }
    Token getSecondParam() const { return secondParam; }
    TokenType getSecondParamType() const { return secondParam.getType(); }
    void setRelationship(const Token& rel) { relationship = rel; }
    void setFirstParam(const Token& param) { firstParam = param; }
    void setSecondParam(const Token& param) { secondParam = param; }
    virtual std::unordered_set<std::string> getAllSynonyms() const = 0;

    void calculatePenalties();

    int getPenalty();

private:
    Token firstParam;
    Token secondParam;
    Token relationship;
    int penalty = -1;


    // create an enum of clause type
    bool negated = false;

    ClauseOperations clauseOperation = AND;


    static const inline std::unordered_map<int, int> noOfSynonymsToPenaltiesMap {
            {0, 5},
            {1, 20},
            {2, 60},
    };

    static const inline std::unordered_map<std::string, int> clauseTypeToPenaltiesMap {
            {"SuchThatClause", 20},
            {"PatternClause", 10},
            {"WithClause", 5},
    };



    static const inline std::unordered_map<TokenType, int> suchThatTypeToPenaltiesMap {
            {TokenType::Affects, 1000},
            {TokenType::NextT, 800},
            {TokenType::FollowsT, 100},
            {TokenType::ParentT, 80},
            {TokenType::CallsT, 80},
            {TokenType::UsesP, 30},
            {TokenType::ModifiesP, 30},
            {TokenType::UsesS, 30},
            {TokenType::ModifiesS, 30},
            {TokenType::Calls, 30},
            {TokenType::Next, 20},
            {TokenType::Follows, 20},
            {TokenType::Parent, 20},
    };

};
