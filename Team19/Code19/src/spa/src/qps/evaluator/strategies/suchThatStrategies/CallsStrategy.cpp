#include "CallsStrategy.h"

#include <string>
#include <unordered_set>
#include <memory>


std::shared_ptr<ResultTable> CallsStrategy::evaluateQueryOptimised(PKBReaderManager &pkbReaderManager,
                                                                    const ParsingResult &parsingResult,
                                                                    const Clause &clause,
                                                                    std::shared_ptr<ResultTable> result) {
    const auto* suchClause = dynamic_cast<const SuchThatClause*>(&clause);
    this->variant = suchClause->getRelationship().getValue();
    setBothParams(clause);
    std::shared_ptr<IRelationshipReader<std::string, std::string>> reader;
    if (variant == "Calls") {
        reader = pkbReaderManager.getCallsReader();
    } else if (variant == "Calls*") {
        reader = pkbReaderManager.getCallsTReader();
    }
    setReader(reader);

    return getOptimallyEvaluatedResultTable(parsingResult, pkbReaderManager, clause, result);
}

std::shared_ptr<ResultTable> CallsStrategy::evaluateQuery(PKBReaderManager& pkbReaderManager, const ParsingResult& parsingResult, const Clause& clause)
{
    auto resultTable = std::make_shared<ResultTable>();

    const SuchThatClause* suchClause = dynamic_cast<const SuchThatClause*>(&clause);
    setBothParams(clause);
    this-> variant = suchClause->getRelationship().getValue();
    std::shared_ptr<IRelationshipReader<std::string, std::string>> reader;
    if (this->variant == "Calls") {
        reader = pkbReaderManager.getCallsReader();
    } else {
        reader = pkbReaderManager.getCallsTReader();
    }
    setReader(reader);
    return getEvaluatedResultTable(pkbReaderManager, parsingResult, resultTable);

}

