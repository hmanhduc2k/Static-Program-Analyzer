#pragma once // Ensures the file is included only once during compilation

#include <string>
#include <unordered_set>
#include <iostream>
#include <memory> // For std::shared_ptr
#include "qps/evaluator/strategies/suchThatStrategies/StmtStmtStrategy.h"

/**
 * @file FollowsStrategy.h
 * @brief Defines the FollowsStrategy class that implements evaluation for Follows and Follows* relationships.
 *
 * The FollowsStrategy class extends StmtStmtStrategy to evaluate queries related to the Follows and Follows*
 * relationships between statements in a program. It supports handling different types of parameters including
 * statement numbers, statement synonyms, and wildcards.
 */

class FollowsStrategy : public StmtStmtStrategy {
private:
    std::shared_ptr<IRelationshipReader<int ,int>> reader; ///< Reader to access Follows relationship information.
    std::shared_ptr<StatementReader> statementReader; ///< Reader to access statement-related information.
    Token firstParam;
    Token secondParam;
    string variant;


public:
    /**
     * Evaluates a query based on the Follows or Follows* relationship.
     * @param pkbReaderManager Reference to the PKB reader manager for accessing program knowledge.
     * @param parsingResult The parsing result containing the details of the query.
     * @return A shared pointer to the result table generated by the evaluation.
     */
    std::shared_ptr<ResultTable> evaluateQuery(PKBReaderManager& pkbReaderManager, const ParsingResult& parsingResult, const Clause& clause) override;

    /**
     * Processes queries where both parameters are synonyms.
     * @param firstParam The first parameter token of the query.
     * @param secondParam The second parameter token of the query.
     * @param variant The variant of the Follows relationship (direct or transitive).
     * @param resultTable The result table to be populated with the evaluation results.
     */
    void processSynonyms(std::shared_ptr<ResultTable> resultTable, const ParsingResult& parsingResult, PKBReaderManager& pkbReaderManager);

    /**
     * Processes queries where the first parameter is a synonym and the second parameter is specific.
     * @param firstParam The first parameter token of the query.
     * @param secondParam The second parameter token of the query.
     * @param variant The variant of the Follows relationship (direct or transitive).
     * @param resultTable The result table to be populated with the evaluation results.
     */
    void processFirstParam(std::shared_ptr<ResultTable> resultTable, const ParsingResult& parsingResult, PKBReaderManager& pkbReaderManager) override;

    /**
     * Processes queries where the second parameter is a synonym and the first parameter is specific.
     * @param firstParam The first parameter token of the query.
     * @param secondParam The second parameter token of the query.
     * @param variant The variant of the Follows relationship (direct or transitive).
     * @param resultTable The result table to be populated with the evaluation results.
     */
    void processSecondParam(std::shared_ptr<ResultTable> resultTable, const ParsingResult& parsingResult, PKBReaderManager& pkbReaderManager) override;

    /**
     * Processes queries where both parameters are integers.
     * @param firstParam The first parameter token of the query.
     * @param secondParam The second parameter token of the query.
     * @param resultTable The result table to be populated with the evaluation results.
     */
    void processIntegerParams(std::shared_ptr<ResultTable> resultTable) override;
};
