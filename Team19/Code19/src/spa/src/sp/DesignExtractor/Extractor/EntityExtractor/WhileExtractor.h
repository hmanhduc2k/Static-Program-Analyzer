#pragma once

#include <stdexcept>
#include "sp/DesignExtractor/Extractor/EntityExtractor/IEntityExtractor.h"

/*
* Extracts the while statement from the AST and writes the information to the PKB.
* This class implements the IExtractor interface. The methods are documented
* from the base class at IExtractor.h
*/
class WhileExtractor : public IEntityExtractor {
public:
	WhileExtractor(std::shared_ptr<ASTNode> root, std::shared_ptr<WhileWriter> whileWriter);

	void extract() override;

private:
	std::shared_ptr<WhileWriter> whileWriter;
};
