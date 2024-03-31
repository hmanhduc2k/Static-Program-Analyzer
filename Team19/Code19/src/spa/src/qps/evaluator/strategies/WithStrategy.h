#pragma once // Ensures the file is included only once during compilation

#include <memory> // For std::shared_ptr
#include <string>
#include <unordered_set>
#include <iostream>
#include <functional>
#include <map>
#include "qps/evaluator/strategies/suchThatStrategies/StmtStmtStrategy.h"

/**
 * @file WithStrategy.h
 * @brief Defines the WithStrategy class that implements equality between attributes.
 *
 * The WithStrategy class extends QueryEvaluationStrategy to evaluate queries 
 */

class WithStrategy : public QueryEvaluationStrategy {
private:
    Token firstParam;
    Token secondParam;
    ParsingResult parsingResult;

    /**
     * Retrieves the entities from the result table based on the given synonym and entity reader for integer entities.
     * @param synonym The synonym to retrieve the entities for.
     * @param resultTable The result table to retrieve the entities from.
     * @param entityReader The entity reader to use for retrieving the entities.
     * @return The set of entities retrieved from the result table.
     */
    std::unordered_set<std::string> retrieveIntEntities(std::string synonym, const std::shared_ptr<ResultTable>& resultTable, const std::shared_ptr<IEntityReader<int>>& entityReader);

    /**
     * Retrieves the entities from the result table based on the given synonym and entity reader for string entities.
     * @param synonym The synonym to retrieve the entities for.
     * @param resultTable The result table to retrieve the entities from.
     * @param entityReader The entity reader to use for retrieving the entities.
     * @return The set of entities retrieved from the result table.
     */
    std::unordered_set<std::string> retrieveStringEntities(std::string synonym, const std::shared_ptr<ResultTable>& resultTable, const std::shared_ptr<IEntityReader<std::string>>& entityReader);

    /**
     * Retrieves the links from the result table based on the given synonym and link reader for integer to string links.
     * @param synonym The synonym to retrieve the links for.
     * @param resultTable The result table to retrieve the links from.
     * @param linkReader The link reader to use for retrieving the links.
     * @return The set of links retrieved from the result table.
     */
    std::unordered_set<std::string> retrieveIntStringLinks(std::string synonym, const std::shared_ptr<ResultTable>& resultTable, const std::shared_ptr<ILinkReader<int, std::string>> linkReader);
public:
    /**
     * Evaluates a query based on the with clause.
     * @param pkbReaderManager Reference to the PKB reader manager for accessing program knowledge.
     * @param parsingResult The parsing result containing the details of the query.
     * @return A shared pointer to the result table generated by the evaluation.
     */
    std::shared_ptr<ResultTable> evaluateQuery(PKBReaderManager& pkbReaderManager, const ParsingResult& parsingResult, const Clause& clause, const std::shared_ptr<ResultTable>& originalResultTable) override;
    std::shared_ptr<ResultTable> evaluateQuery(PKBReaderManager& pkbReaderManager, const ParsingResult& parsingResult, const Clause& clause) override;


    /**
     * Processes queries where the first parameter is a synonym and the second parameter is specific.
     * @param pkbReaderManager Reference to the PKB reader manager for accessing program knowledge.
     * @param parsingResult The parsing result containing the details of the query.
     */
    std::unordered_set<std::string> processParam(Token param, PKBReaderManager& pkbReaderManager, const std::shared_ptr<ResultTable>& resultTable);


    std::pair<string, string> extractAttributes(Token param);
    bool isInteger(const std::string& str);
    bool isQuotedString(const std::string& str);

    std::vector<std::string> findIntersection(const std::unordered_set<std::string>& set1, const std::unordered_set<std::string>& set2);

    bool isIntegerStored(string synyonymType, string attribute);
    vector<std::string> mapStringSetToIntSet(PKBReaderManager& pkbReaderManager, const vector<string>& stringSet, string& synonymType);
    std::vector<std::pair<string, string>> populateResultTable(const std::shared_ptr<ResultTable>& resultTable, const std::vector<std::string>& intersection, Token param, PKBReaderManager& pkbReaderManager);
    const std::map<std::string, std::function<std::unordered_set<std::string>(std::string, PKBReaderManager&)>> refToOriginalValueMap = {
            {"stmt.stmt#", [](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<StatementReader> statemetnReader = pkbReaderManager.getStatementReader();
                if (statemetnReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},

            {"read.varName",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<ReadVarNameReader> readerVarNameReader = pkbReaderManager.getReadVarNameReader();
                std::unordered_set<int> readStmt = readerVarNameReader->getLinker(attr);
                std::unordered_set<std::string> readStmtString;
                QueryEvaluationStrategy::convertIntSetToStringSet(readStmt, readStmtString);
                return readStmtString;
            }},
            {"read.stmt#",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<ReadReader> readReader = pkbReaderManager.getReadReader();
                if (readReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"print.varName",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<PrintVarNameReader> printVarNameReader = pkbReaderManager.getPrintVarNameReader();
                std::unordered_set<int> printStmt = printVarNameReader->getLinker(attr);
                std::unordered_set<std::string> printStmtString;
                QueryEvaluationStrategy::convertIntSetToStringSet(printStmt, printStmtString);
                return printStmtString;
            }},
            {"print.stmt#",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<PrintReader> printReader = pkbReaderManager.getPrintReader();
                if (printReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"call.procName",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<CallProcNameReader> callProcNameReader = pkbReaderManager.getCallProcNameReader();
                std::unordered_set<int> callStmts = callProcNameReader->getLinker(attr);
                std::unordered_set<std::string> callStmtsString;
                QueryEvaluationStrategy::convertIntSetToStringSet(callStmts, callStmtsString);
                return callStmtsString;
            }},
            {"call.stmt#",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<CallReader> callReader = pkbReaderManager.getCallReader();
                if (callReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"while.stmt#",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<WhileReader> whileReader = pkbReaderManager.getWhileReader();
                if (whileReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"if.stmt#",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<IfReader> ifReader = pkbReaderManager.getIfReader();
                if (ifReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"assign.stmt#",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<AssignReader> assignReader = pkbReaderManager.getAssignReader();
                if (assignReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"variable.varName",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<VariableReader> variableReader = pkbReaderManager.getVariableReader();
                if (variableReader->contains(attr)) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"constant.value",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<ConstantReader> constantReader = pkbReaderManager.getConstantReader();
                if (constantReader->contains(std::stoi(attr))) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }},
            {"procedure.procName",[](std::string attr, PKBReaderManager& pkbReaderManager) {
                std::shared_ptr<ProcedureReader> procedureReader = pkbReaderManager.getProcedureReader();
                if (procedureReader->contains(attr)) {
                    return std::unordered_set<std::string>{attr};
                } else {
                    return std::unordered_set<std::string>{};
                }
            }}

    };

    const std::map<std::string, std::function<std::unordered_set<std::string>(std::string, PKBReaderManager&, const std::shared_ptr<ResultTable>&)>> attrToAllEntitiesMap = {
            {"stmt.stmt#",
             [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                 std::shared_ptr<StatementReader> statementReader = pkbReaderManager.getStatementReader();
                 return retrieveIntEntities(synonym, resultTable, statementReader);
             }},

            {"read.varName",
             [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                 std::shared_ptr<ReadVarNameReader> readerVarNameReader = pkbReaderManager.getReadVarNameReader();
                 return retrieveIntStringLinks(synonym, resultTable, readerVarNameReader);
             }},
            {"read.stmt#",
             [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                 std::shared_ptr<ReadReader> readReader = pkbReaderManager.getReadReader();
                 return retrieveIntEntities(synonym, resultTable, readReader);
             }},
            {"print.varName",
             [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                 std::shared_ptr<PrintVarNameReader> printVarNameReader = pkbReaderManager.getPrintVarNameReader();
                 return retrieveIntStringLinks(synonym, resultTable, printVarNameReader);
             }},
            {"print.stmt#",
             [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                 std::shared_ptr<PrintReader> printReader = pkbReaderManager.getPrintReader();
                 return retrieveIntEntities(synonym, resultTable, printReader);
             }
            },
            {"call.procName", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<CallProcNameReader> callProcNameReader = pkbReaderManager.getCallProcNameReader();
                return retrieveIntStringLinks(synonym, resultTable, callProcNameReader);
            }},
            {"call.stmt#", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<CallReader> callReader = pkbReaderManager.getCallReader();
                return retrieveIntEntities(synonym, resultTable, callReader);
            }},
            {"while.stmt#", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<WhileReader> whileReader = pkbReaderManager.getWhileReader();
                return retrieveIntEntities(synonym, resultTable, whileReader);
            }},
            {"if.stmt#", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<IfReader> ifReader = pkbReaderManager.getIfReader();
                return retrieveIntEntities(synonym, resultTable, ifReader);
            }},
            {"assign.stmt#", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<AssignReader> assignReader = pkbReaderManager.getAssignReader();
                return retrieveIntEntities(synonym, resultTable, assignReader);
            }},
            {"variable.varName", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<VariableReader> variableReader = pkbReaderManager.getVariableReader();
                return retrieveStringEntities(synonym, resultTable, variableReader);
            }},
            {"constant.value", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<ConstantReader> constantReader = pkbReaderManager.getConstantReader();
                return retrieveIntEntities(synonym, resultTable, constantReader);
            }},
            {"procedure.procName", [this](std::string synonym, PKBReaderManager &pkbReaderManager, const std::shared_ptr<ResultTable> &resultTable) {
                std::shared_ptr<ProcedureReader> procedureReader = pkbReaderManager.getProcedureReader();
                return retrieveStringEntities(synonym, resultTable, procedureReader);
            }},

    };
    std::string getAttrRefType(const Token& token);

    void populateWithIntersection(const std::vector<std::string>& intersection, PKBReaderManager& pkbReaderManager, const std::shared_ptr<ResultTable>& resultTable,
                                                const Token& firstParam, const Token& secondParam);

    void populateWithFirstParam(const std::shared_ptr<ResultTable>& resultTable, PKBReaderManager& pkbReaderManager, const Token& firstParam, const Token& secondParam);
    void populateWithSecondParam(const std::shared_ptr<ResultTable>& resultTable, PKBReaderManager& pkbReaderManager, const Token& firstParam, const Token& secondParam);
    bool isAttrRef(Token token);
    bool isAttrRefIdentical(const Token& firstParam, const Token& secondParam);


};
