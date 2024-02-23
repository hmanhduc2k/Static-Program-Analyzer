#pragma once

#include <memory>
#include <vector>
#include "sp/AST/ast.h"
#include "sp/SPTokenizer/SPTokenizer.h"
#include "sp/SPTokenizer/LexicalToken.h"
#include "sp/SPTokenizer/FileProcessor.h"
#include "sp/Utility.h"
#include "sp/AST/ASTHelper.h"

// ai-gen start(gpt,2,e)
// Prompt: https://platform.openai.com/playground/p/cJLjmmneCEs4z6ms7ZkBSxJB?model=gpt-4&mode=chat
class SimpleParser {
public:
	SimpleParser(std::string filename);
	std::shared_ptr<ASTNode> parseProgram();

private:
	std::vector<LexicalToken> tokenStream;
	int tokenIndex;
	void assertToken(LexicalToken token, LexicalTokenType type) const;
	bool hasTokensLeft() const;
	LexicalToken peekNextToken();
	LexicalToken getNextToken();
	LexicalToken peekNextNextToken();
	std::shared_ptr<ASTNode> parseProcedure();
	std::shared_ptr<ASTNode> parseStmtLst();
	std::shared_ptr<ASTNode> parseStmt();
	std::shared_ptr<ASTNode> parseRead();
	std::shared_ptr<ASTNode> parsePrint();
	std::shared_ptr<ASTNode> parseCall();
	std::shared_ptr<ASTNode> parseWhile();
	std::shared_ptr<ASTNode> parseIf();
	std::shared_ptr<ASTNode> parseAssign();
	std::shared_ptr<ASTNode> parseCondExpr();
	std::shared_ptr<ASTNode> parseRelExpr();
	std::shared_ptr<ASTNode> parseRelFactor();
	std::shared_ptr<ASTNode> parseExpr();
	std::shared_ptr<ASTNode> parseTerm();
	std::shared_ptr<ASTNode> parseFactor();
	std::shared_ptr<ASTNode> parseVarName();
	std::shared_ptr<ASTNode> parseProcName();
	std::shared_ptr<ASTNode> parseConstValue();
};
// ai-gen end