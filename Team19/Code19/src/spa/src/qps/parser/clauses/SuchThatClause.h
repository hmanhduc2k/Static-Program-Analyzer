#pragma once

#include "qps/parser/clauses/Clause.h"


struct SuchThatClause : public Clause {

private:





public:
    ~SuchThatClause() override = default;
    std::string getTypeName() const override {
        return "SuchThatClause";
    }
    std::unordered_set<std::string> getAllSynonyms() const override;




};
