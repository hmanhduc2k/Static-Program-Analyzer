#include "FollowsStrategy.h"
#include <memory>
#include <unordered_set>
#include <string>

// Standard library dependencies


std::shared_ptr<ResultTable> FollowsStrategy::evaluateQueryOptimised(PKBReaderManager &pkbReaderManager,
                                                                    const ParsingResult &parsingResult,
                                                                    const Clause &clause,
                                                                    std::shared_ptr<ResultTable> result) {
    setIntermediateResultTable(result);
    return evaluateQuery(pkbReaderManager, parsingResult, clause);
}
/**
 * Evaluates Follows or Follows* queries between two statements.
 * Determines the relationship based on the parsing result and updates the result table accordingly.
 *
 * @param pkbReaderManager Provides access to the program knowledge base.
 * @param parsingResult Contains the parsed query details.
 * @return A shared pointer to the populated result table.
 */
std::shared_ptr<ResultTable> FollowsStrategy::evaluateQuery(PKBReaderManager& pkbReaderManager,
	const ParsingResult& parsingResult, const Clause& clause) {
    auto resultTable = std::make_shared<ResultTable>();


    const SuchThatClause* suchClause = dynamic_cast<const SuchThatClause*>(&clause);
    setBothParams(clause);
    this->variant   = suchClause->getRelationship().getValue();
    std::shared_ptr<IRelationshipReader<int, int>> reader;
    if (this->variant == "Follows") {
        reader = pkbReaderManager.getFollowsReader();
    } else {
        reader = pkbReaderManager.getFollowsTReader();
    }
    setReader(reader);



	// Handling different parameter types for the Follows relationship
	if (isBothParamsSynonym(firstParam, secondParam)) {
		processSynonyms(resultTable, parsingResult, pkbReaderManager);
	}
	else if (firstParam.getType() == TokenType::IDENT) {
		processFirstParam(resultTable, parsingResult, pkbReaderManager);
	}
	else if (secondParam.getType() == TokenType::IDENT) {
		processSecondParam(resultTable, parsingResult, pkbReaderManager);
	}
	else {
		processIntegerParams(resultTable);
	}

	return resultTable;
}

