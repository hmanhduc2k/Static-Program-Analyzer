#pragma once

#include <stdexcept>
#include "sp/DesignExtractor/Extractor/EntityExtractor/IEntityExtractor.h"

/*
* Extracts all the procedures from the AST and writes them to the PKB.
* A procedure is defined as a function or a procedure in the source code.
* Documentations for the methods can be found in IExtractor.h
*/
class ProcedureExtractor : public IEntityExtractor {
public:
	ProcedureExtractor(std::shared_ptr<ASTNode> root, std::shared_ptr<ProcedureWriter> procedureWriter);

	void extract() override;

private:
	std::shared_ptr<ProcedureWriter> procedureWriter;
};
