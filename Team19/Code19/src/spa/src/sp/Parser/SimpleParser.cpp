#include "SimpleParser.h"
#include <stdexcept>
#include <iostream>

// ai-gen start(gpt,2,e)
// Prompt: https://platform.openai.com/playground/p/cJLjmmneCEs4z6ms7ZkBSxJB?model=gpt-4&mode=chat

SimpleParser::SimpleParser() {
	
}

SimpleParser::SimpleParser(std::string filename) {
	this->tokenStream = SPTokenizer::tokenize(SPTokenizer::readFileToString(filename));
	this->tokenIndex = 0;
}

std::shared_ptr<ASTNode> SimpleParser::parseProgram() {
	std::vector<std::shared_ptr<ASTNode>> procedures;

	while (this->hasTokensLeft()) {
		if (this->peekToken().getTokenType() == LexicalTokenType::WHITESPACE) {
			this->getToken();
			continue;
		}

		procedures.push_back(this->parseProcedure());
	}

	// Depending on how important the the number of lines is to the code, find out whether to further go ahead
	// with finding the number of lines.
	Program program = Program(-1, procedures);

	return program.buildTree();
}

bool SimpleParser::hasTokensLeft() const {
	return this->tokenIndex < this->tokenStream.size();
}

// Gets token, without advancing the index.
LexicalToken SimpleParser::peekToken() const {
	if (this->hasTokensLeft()) {
		return this->tokenStream[this->tokenIndex];
	}
	else {
		return LexicalToken(LexicalTokenType::NULL_TOKEN);
	}
}

// Gets token, and advance the index to look for the next token.
LexicalToken SimpleParser::getToken() {
	if (this->hasTokensLeft()) {
		LexicalToken token = this->tokenStream[this->tokenIndex];
		this->tokenIndex++;
		return token;
	}
	else {
		return LexicalToken(LexicalTokenType::NULL_TOKEN);
	}
}

void SimpleParser::assertToken(LexicalToken token, LexicalTokenType type) const {
	if (token.getTokenType() != type) {
		throw std::runtime_error("Error: Invalid SIMPLE syntax.");
	}
}

std::shared_ptr<ASTNode> SimpleParser::parseProcedure() {
	if (!this->hasTokensLeft()) {
		throw std::runtime_error("Error: SimpleParser::parseProcedure encounter empty statement.");
	}

	LexicalToken procedureKeyword = this->getToken();
	this->assertToken(procedureKeyword, LexicalTokenType::KEYWORD_PROCEDURE);

	LexicalToken procedureName = this->getToken();
	this->assertToken(procedureName, LexicalTokenType::NAME);
	this->assertToken(this->getToken(), LexicalTokenType::SYMBOL_OPEN_BRACE);

	// Parse Statement Lists;
	std::shared_ptr<ASTNode> statementList = this->parseStmtLst();

	LexicalToken closeBrace = this->getToken();
	this->assertToken(closeBrace, LexicalTokenType::SYMBOL_CLOSE_BRACE);

	Procedure procedure = Procedure(procedureKeyword.getLine(), closeBrace.getLine(), statementList);
	return procedure.buildTree();
}

std::shared_ptr<ASTNode> SimpleParser::parseStmtLst() {
	// Parse every statement until we see a closing bracket.
	std::vector<std::shared_ptr<ASTNode>> statements;
	int firstLine = this->peekToken().getLine();
	while (this->peekToken().getTokenType() != LexicalTokenType::SYMBOL_CLOSE_BRACE) {
		if (this->peekToken().getTokenType() == LexicalTokenType::WHITESPACE) {
			this->getToken();
			continue;
		}
		statements.push_back(this->parseStmt());
	}

	int lastLine = this->peekToken().getLine();

	StmtList stmtList = StmtList(firstLine, lastLine, statements);

	return stmtList.buildTree();
}

std::shared_ptr<ASTNode> SimpleParser::parseStmt() {
	// Add parsing logic for statement
	// If next token is '=', we are assigning. Call assign.
	// Then, Check keywords read/print/call.
	// Then, check keywords while/if.
	// If dont have keyword, this is an invalid statement.
	if (!this->hasTokensLeft()) {
		throw std::runtime_error("Error: SimpleParser::parseStmt encounter empty statement.");
	}

	LexicalToken firstToken = this->peekToken();
	if (firstToken.getTokenType() == LexicalTokenType::KEYWORD_CALL) {
		return this->parseCall();
	}

	if (firstToken.getTokenType() == LexicalTokenType::KEYWORD_PRINT) {
		return this->parsePrint();
	}

	if (firstToken.getTokenType() == LexicalTokenType::KEYWORD_READ) {
		return this->parseRead();
	}

	throw std::runtime_error("Error: SimpleParser only accepts READ,CALL,PRINT statements now.");
}

std::shared_ptr<ASTNode> SimpleParser::parseRead() {
	LexicalToken keyword = this->getToken();
	this->assertToken(keyword, LexicalTokenType::KEYWORD_READ);

	LexicalToken variable = this->getToken();
	this->assertToken(variable, LexicalTokenType::NAME);

	LexicalToken semicolon = this->getToken();
	this->assertToken(semicolon, LexicalTokenType::SYMBOL_SEMICOLON);

	ReadStmt readStmt = ReadStmt(variable, keyword.getLine(), semicolon.getLine());

	return readStmt.buildTree();
}

std::shared_ptr<ASTNode> SimpleParser::parsePrint() {
	LexicalToken keyword = this->getToken();
	this->assertToken(keyword, LexicalTokenType::KEYWORD_PRINT);

	LexicalToken variable = this->getToken();
	this->assertToken(variable, LexicalTokenType::NAME);

	LexicalToken semicolon = this->getToken();
	this->assertToken(semicolon, LexicalTokenType::SYMBOL_SEMICOLON);

	PrintStmt printStmt = PrintStmt(variable, keyword.getLine(), semicolon.getLine());

	return printStmt.buildTree();
}

// To change to void later when building trees.
std::shared_ptr<ASTNode> SimpleParser::parseCall() {
	LexicalToken keyword = this->getToken();
	this->assertToken(keyword, LexicalTokenType::KEYWORD_CALL);

	LexicalToken variable = this->getToken();
	this->assertToken(variable, LexicalTokenType::NAME);

	LexicalToken semicolon = this->getToken();
	this->assertToken(semicolon, LexicalTokenType::SYMBOL_SEMICOLON);

	CallStmt callStmt = CallStmt(variable, keyword.getLine(), semicolon.getLine());

	return callStmt.buildTree();
}

void SimpleParser::parseWhile() {
	// Add parsing logic for while
}

void SimpleParser::parseIf() {
	// Add parsing logic for if statement
}

void SimpleParser::parseAssign() {
	// Add parsing logic for assignment
}

void SimpleParser::parseCondExpr() {
	// Add parsing logic for condition expression
}

void SimpleParser::parseRelExpr() {
	// Add parsing logic for relational expression
}

void SimpleParser::parseRelFactor() {
	// Add parsing logic for relational factor (variable name or constant value or expression)
}

void SimpleParser::parseExpr() {
	// Add parsing logic for expression (term or expression "+" term or expression "-" term)
}

void SimpleParser::parseTerm() {
	// Add parsing logic for term (factor or term "*" factor or term "/" factor or term "%" factor)
}

void SimpleParser::parseFactor() {
	// Add parsing logic for factor (variable name or constant value or "(" expr ")")
}

void SimpleParser::parseVarName() {
	// Add parsing logic for variable name
}

void SimpleParser::parseProcName() {
	// Add parsing logic for procedure name
}

void SimpleParser::parseConstValue() {
	// Add parsing logic for constant value parsing
}
// ai-gen end