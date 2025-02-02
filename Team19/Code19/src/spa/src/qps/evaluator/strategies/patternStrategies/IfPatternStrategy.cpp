#include "IfPatternStrategy.h"
#include <memory>
#include <string>
#include <unordered_set>


std::shared_ptr<ResultTable> IfPatternStrategy::evaluateQueryOptimised(PKBReaderManager &pkbReaderManager,
                                                                     const ParsingResult &parsingResult,
                                                                     const Clause &clause,
                                                                     std::shared_ptr<ResultTable> result) {

    setIntermediateResultTable(result);
    return evaluateQuery(pkbReaderManager, parsingResult, clause);
}

std::shared_ptr<ResultTable> IfPatternStrategy::evaluateQuery(PKBReaderManager& pkbReaderManager, const ParsingResult& parsingResult, const Clause& clause) {
    auto resultTable = std::make_shared<ResultTable>();
    ifPatternReader = pkbReaderManager.getIfPatternReader();


    const PatternClause *patternClause = dynamic_cast<const PatternClause *>(&clause);
    setBothParams(clause);
    Token relationship = patternClause->getRelationship();
    setRelationship(relationship);
    return getEvaluatedResultTable(pkbReaderManager, parsingResult, resultTable);

}

void IfPatternStrategy::processSynonyms(ParsingResult parsingResult, std::shared_ptr<ResultTable> resultTable) {

    std::string firstColName = relationship.getValue();
    std::string secondColName = firstParam.getValue();
    insertColsToTable(relationship, firstParam, resultTable);
    insertRowsWithTwoCols(firstParam, relationship, parsingResult ,ifPatternReader, resultTable);
}

void IfPatternStrategy::processQuotedIdent(ParsingResult parsingResult, std::shared_ptr<ResultTable> result) {

    std::string firstColName = relationship.getValue();
    insertSingleColToTable(relationship, result);
    std::unordered_set<int> allStmts;
    std::string firstParamValue;
    std::string secondParamValue;
    firstParamValue = extractQuotedExpression(firstParam);

//    secondParamValue = ShuntingYard::convertToPostfix(secondParamValue);
    allStmts = ifPatternReader->getStatementNumbersWithControlPattern(firstParamValue);
    insertStmtRowsWithSingleCol(allStmts, result, firstColName);
}

void IfPatternStrategy::processWildcard(ParsingResult parsingResult, std::shared_ptr<ResultTable> result) {

    std::string firstColName = relationship.getValue();
    insertSingleColToTable(relationship, result);
    std::unordered_set<int> allStmts;
    allStmts = ifPatternReader->getAllStatementNumbersOfIfControlVariables();
    insertStmtRowsWithSingleCol(allStmts, result, firstColName);
}






