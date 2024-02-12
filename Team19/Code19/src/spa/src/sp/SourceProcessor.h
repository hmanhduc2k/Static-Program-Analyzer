#include "sp/Tokenizer.h"
#include "sp/LexicalToken.h"
#include "sp/DesignExtractor/DesignExtractor.h"
#include "sp/Parser/SimpleParser.h"
#include "sp/AST/ASTNode.h"
#include "pkb/PKBManager.h"

class SourceProcessor {
public:
	SimpleParser parser;
	std::shared_ptr<ASTNode> root;
	PKBManager pkbManager;

	SourceProcessor(std::string filename, PKBManager pkbManager);
	void reset();

	void parseSIMPLE();
	void buildAST();

	void sampleAST();	// for testing purposes
	void extractAndPopulate();
};