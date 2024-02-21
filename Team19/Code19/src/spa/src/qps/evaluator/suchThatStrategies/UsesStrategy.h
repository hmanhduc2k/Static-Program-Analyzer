#pragma
#define USESSTRATEGY_H

#include "qps/evaluator/suchThatStrategies/StmtEntStrategy.h"


using namespace std;

class UsesStrategy : public StmtEntStrategy {
private:
    std::shared_ptr<UsesSReader> UsesSReader;
    PKBReaderManager& pkbReaderManager;


public:
    std::shared_ptr<ResultTable> evaluateQuery(PKBReaderManager& pkbReaderManager, const ParsingResult& parsingResult) override;
    void processBothSynonyms(const Token& firstParam, const Token& secondParam, const ParsingResult& parsingResult
            ,std::shared_ptr<ResultTable> resultTable) override;
    void processFirstParam(const Token& firstParam, const Token& secondParam, const ParsingResult& parsingResult
            ,std::shared_ptr<ResultTable> resultTable) override ;
    void processSecondParam(const Token& firstParam, const Token& secondParam, const ParsingResult& parsingResult
            ,std::shared_ptr<ResultTable> resultTable) override;
    void processBothConstants(const Token& firstParam, const Token& secondParam, const ParsingResult& parsingResult
            ,std::shared_ptr<ResultTable> resultTable) override;
    void processBothWildCards(std::shared_ptr<ResultTable> resultTable) override;


};

