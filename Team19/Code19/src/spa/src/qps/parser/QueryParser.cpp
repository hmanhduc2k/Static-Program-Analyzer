// Include the parser class header.
#include "QueryParser.h"
#include <stdexcept>
#include <unordered_set>
#include <iostream>
#include <regex>

// Constructor of the QueryParser class.
// Initializes the QueryParser with a vector of Token objects to be parsed.
QueryParser::QueryParser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0), parsingResult(), semanticError(false) {}

void QueryParser::setSemanticError() {
	semanticError = true;
}

// Parses the entire query and validate the syntax and semantics, stop parsing when there is an error.
// Processes declarations, select clause, and optional such that and pattern clauses.
ParsingResult QueryParser::makeResult(ParsingResult parsingResult) {
	if (semanticError) {
		parsingResult.setErrorMessage("SemanticError");
	}
	return parsingResult;
}

ParsingResult QueryParser::parse() {

    try {
        checkForExistingSyntaxError();
        if (match(TokenType::DesignEntity)) {
            parseDeclarations();
        }


        parseSelectClause();
        if (currentTokenIndex == tokens.size() - 1) {
            return makeResult(parsingResult);
        }
        advanceToken();

        while (currentTokenIndex <= tokens.size() - 1) {
            // check if there is a such that or pattern clause with such that coming first
            if (match(TokenType::SuchKeyword) || (match(TokenType::AndKeyword) && currentToken().getValue() == "such that")) {
                if (match(TokenType::SuchKeyword)) {
                    advanceToken();
                    ensureToken(TokenType::ThatKeyword);
                }

                parseSuchThatClause();
                if (currentTokenIndex == tokens.size() - 1) {
                    return makeResult(parsingResult);
                }
            }
            else if (match(TokenType::PatternKeyword) || (match(TokenType::AndKeyword) && currentToken().getValue() == "pattern")) {
                parsePatternClause();

                if (currentTokenIndex == tokens.size() - 1) {
                    return makeResult(parsingResult);
                }

            }
            else if (match(TokenType::WithKeyword) || (match(TokenType::AndKeyword) && currentToken().getValue() == "with")) {
                parseWithClause();

                if (currentTokenIndex == tokens.size() - 1) {
                    return makeResult(parsingResult);
                }
            }
            else {
                throwSyntaxError();
            }
			advanceToken();
		}
        if (currentTokenIndex == tokens.size() - 1) {
            return makeResult(parsingResult);
        }
        throwSyntaxError();
        return makeResult(parsingResult);
	}
    catch (const std::invalid_argument& e) {
        // Handle the exception
        parsingResult.setErrorMessage(e.what());
        return makeResult(parsingResult);
    }
}

// Parses declarations in the query.
// Processes design entities and their synonyms, and ensures correct syntax with semicolons.
void QueryParser::parseDeclarations() {
    int numberOfDeclarations = 0;
    while (!match(TokenType::SelectKeyword)) {
        parseDesignEntity();
        std::string assignmentType = currentToken().getValue();
        advanceToken();

        parseSynonym();
        if (parsingResult.isSynonymDeclared(currentToken().getValue())) {
            setSemanticError();
        }
        parsingResult.addDeclaredSynonym(currentToken().getValue(), assignmentType);
        advanceToken();

        while (match(TokenType::Comma)) {
            advanceToken();
            parseSynonym();
            // check if the token value is declared as synonyms in any of the set
            if (!parsingResult.getDeclaredSynonym(currentToken().getValue()).empty()) {
                setSemanticError();
            }
            parsingResult.addDeclaredSynonym(currentToken().getValue(), assignmentType);
            advanceToken();
        }

        ensureToken(TokenType::Semicolon);
        advanceToken();

        numberOfDeclarations++;
    }
    
}

// Parses a design entity in the query.
// Ensures the current token is a design entity and advances the token.
void QueryParser::parseDesignEntity() {
    if (currentToken().getType() != TokenType::DesignEntity) {
        throwSyntaxError();
    }
}


// Parses a synonym in the query.
// Ensures the current token is an identifier and advances the token.
void QueryParser::parseSynonym() {
    // check if it is an ident and if the synonym is already declared
    ensureToken(TokenType::IDENT);
}



// Parses the select clause in the query.
// Checks for the 'Select' keyword and ensures the following token is a valid identifier.
void QueryParser::parseSelectClause() {
    ensureToken(TokenType::SelectKeyword);
    advanceToken();

    if (match(TokenType::LeftAngleBracket)) {
        parseSelectTuple();
    } else {
        parseSelectSingle();
    }
}

void QueryParser::parseSelectTuple() {
    advanceToken();
    int numberOfSynonymsSelected = 0;

    while (!match(TokenType::RightAngleBracket)) {
        parseSelectTupleElement();
        numberOfSynonymsSelected++;

        if (match(TokenType::RightAngleBracket)) {
            break;
        }

        ensureToken(TokenType::Comma);
        advanceToken();
    }

    if (numberOfSynonymsSelected == 0) {
        throwSyntaxError();
    }
}

void QueryParser::parseSelectSingle() {
    currentSuchThatToken = currentToken();
    size_t startIndex = currentTokenIndex;

    if (tokens.size() > currentTokenIndex + 1 && peekNextToken(TokenType::Dot)) {
        parseAttrRef();
    } else {
        if (!match(TokenType::IDENT)) {
            ensureToken(TokenType::BooleanKeyword);
            parsingResult.setBooleanSelection();
        }
    }

    std::string concatenatedTokens = concatTokens(startIndex, currentTokenIndex);
    parsingResult.setRequiredSynonym(concatenatedTokens);

    if (parsingResult.getDeclaredSynonym(currentSuchThatToken.getValue()).empty() &&
        currentSuchThatToken.getValue() != "BOOLEAN") {
        setSemanticError();
    }
}

void QueryParser::parseSelectTupleElement() {
    currentSuchThatToken = currentToken();
    size_t startIndex = currentTokenIndex;

    if (peekNextToken(TokenType::Dot)) {
        parseAttrRef();
    } else {
        ensureToken(TokenType::IDENT);
    }

    std::string concatenatedTokens = concatTokens(startIndex, currentTokenIndex);
    parsingResult.setRequiredSynonym(concatenatedTokens);

    if (parsingResult.getDeclaredSynonym(currentSuchThatToken.getValue()).empty()) {
        setSemanticError();
    }

    advanceToken();
}


// Parses the 'such that' clause in the query.
// Ensures the correct syntax and calls the function to process relation references.
void QueryParser::parseSuchThatClause() {
    SuchThatClause clause;
    
    advanceToken();
    if (match(TokenType::NotKeyword)) {
        clause.setAsNegated();
		advanceToken();
    }
    parseRelRef(clause);
    parsingResult.addSuchThatClause(clause);
}

// Parses relation references in a 'such that' clause.
// Determines the type of relation and calls the appropriate parsing function.
void QueryParser::parseRelRef(SuchThatClause& clause) {
    if (isStmtRefStmtRef()) {
        clause.setRelationship(currentToken());
        advanceToken();
        parseStmtRefStmtRef(clause);
    } else if (isUsesOrModifies()) {
        parseUsesOrModifies(clause);
    } else if (isCalls()){
        parseCalls(clause);
    } else {
        throwSyntaxError();
    }

}

// Checks if the current context is a statement reference to statement reference relation.
bool QueryParser::isStmtRefStmtRef() {
    if (match(TokenType::Parent) || match(TokenType::ParentT) ||
        match(TokenType::Follows) || match(TokenType::FollowsT) ||
        match(TokenType::Next) || match(TokenType::NextT) || match(TokenType::Affects)) {
        currentSuchThatToken = currentToken();
        return true;
    }

    return false;
}

// Checks if the current context is a 'Uses' or 'Modifies' relation.
bool QueryParser::isUsesOrModifies() {
    if (match(TokenType::Uses) || match(TokenType::Modifies)) {
        return true;
    }
    return false;
}

bool QueryParser::isCalls() {
    if (match(TokenType::Calls) || match(TokenType::CallsT)) {
        return true;
    }
    return false;
}


void QueryParser::parseCalls(SuchThatClause& clause) {

    currentSuchThatToken = currentToken();
    clause.setRelationship(currentSuchThatToken);
    advanceToken();
    if (match(TokenType::Lparenthesis)) {
        advanceToken();
    }
    else {
        throwSyntaxError();
    }

    parseEntRef();
    ensureSynonymType("procedure");


    clause.setFirstParam(currentToken());
    advanceToken();

    if (!match(TokenType::Comma)) {
		throwSyntaxError();
    }
	advanceToken();


	parseEntRef();
    ensureSynonymType("procedure");
    clause.setSecondParam(currentToken());
    advanceToken();
    ensureToken(TokenType::Rparenthesis);
}
// Parses a 'Uses' or 'Modifies' relation in the query.
// Ensures correct syntax and processes statement and entity references.
// set the Token type for the such that clause relationship more specifically
void QueryParser::parseUsesOrModifies(SuchThatClause& clause) {
    // need to differentiate between usesS/UsesP and modifiesS/ModifiesP
    currentSuchThatToken = currentToken();


    advanceToken();
    if (!match(TokenType::Lparenthesis)) {
		throwSyntaxError();
	}

	advanceToken();

    if (match(TokenType::Wildcard)) {
        setSemanticError();
    }

    // check if it is synonym of stmt or ent/an integer or a quoted identifier
    if (checkIfStmt()) {
        parseStmtRef();
        if (currentSuchThatToken.getType() == TokenType::Uses) {
            currentSuchThatToken.setType(TokenType::UsesS);
        }
        else {
            currentSuchThatToken.setType(TokenType::ModifiesS);
        }
        clause.setRelationship(currentSuchThatToken);
        clause.setFirstParam(currentToken());
    } else if (checkIfEnt()) {
        parseEntRef(); // Attempt to parse an entity reference
        if (currentToken().getType() == TokenType::IDENT) {
            ensureSynonymType("procedure");
        }
        // If parseEntRef succeeds, control continues here
        if (currentSuchThatToken.getType() == TokenType::Uses) {
            currentSuchThatToken.setType(TokenType::UsesP);
        }
        else {
            currentSuchThatToken.setType(TokenType::ModifiesP);
        }
        clause.setRelationship(currentSuchThatToken);
        clause.setFirstParam(currentToken());
    } else {
        setSemanticError();
    }
	advanceToken();



    if (!match(TokenType::Comma)) {
		throwSyntaxError();
    }
	advanceToken();

	parseEntRef();
    if (currentToken().getType() == TokenType::IDENT) {
        ensureSynonymType("variable");
    }

    clause.setSecondParam(currentToken());
    advanceToken();
    ensureToken(TokenType::Rparenthesis);
}

// Parses a statement reference to statement reference relation.
// Ensures correct syntax and processes multiple statement references.
void QueryParser::parseStmtRefStmtRef(SuchThatClause& clause) {
    if (!match(TokenType::Lparenthesis)) {
        throwSyntaxError();
    }
	advanceToken();

    // stmtRef
    parseStmtRef();
    clause.setFirstParam(currentToken());
    advanceToken();


    //','
    if (!match(TokenType::Comma)) {
		throwSyntaxError();
    }
	advanceToken();

    parseStmtRef();
    clause.setSecondParam(currentToken());
    advanceToken();

    ensureToken(TokenType::Rparenthesis);
}

// Parses a statement reference in the query.
void QueryParser::parseStmtRef() {
    if (match(TokenType::Wildcard)) {
        return;
    } else if (match(TokenType::INTEGER)) {
        if(checkValidStmtNum()) {
            return;
        }
        setSemanticError();
    } else {
        parseStmtSynonyms();
    }


}

// Parses an entity reference in the query.
// Handles different types of entity references like quoted identifier, wildcard, or synonym.
void QueryParser::parseEntRef() {
    if (!(match(TokenType::QuoutIDENT) || match(TokenType::Wildcard) || match(TokenType::IDENT))) {
        throwSyntaxError();
    }
    if (match(TokenType::QuoutIDENT) || match(TokenType::Wildcard)) {
        return;
    } else {
        parseEntSynonym();
    }

}

// Parses the pattern clause in the query.
// Ensures the correct syntax and processes entity references and expression specifications.
void QueryParser::parsePatternClause() {
    PatternClause clause;
    advanceToken();
    if (match(TokenType::NotKeyword)) {
        clause.setAsNegated();
        advanceToken();
    }
    // check if it is a syn-assign
    ensureToken(TokenType::IDENT);
    clause.setRelationship(currentToken());
    parsePatternSynonym();


    ensureToken(TokenType::Lparenthesis);

    advanceToken();


    parseEntRef();
    if (currentToken().getType() == TokenType::IDENT) {
        ensureSynonymType("variable");
    }
    clause.setFirstParam(currentToken());

    advanceToken();
    ensureToken(TokenType::Comma);
    advanceToken();
    parseExpressionSpec(clause);
    if (peekNextToken(TokenType::Comma)) {
        advanceToken();
        advanceToken();
        if (currentToken().getType() != TokenType::Wildcard) {
            throwSyntaxError();
        }
        clause.setThirdParam(currentToken());
    }

    advanceToken();
    ensureToken(TokenType::Rparenthesis);
    parsingResult.addPatternClause(clause);



    std::string patternType = parsingResult.getPatternClauseType(clause);
    ensureCorrectPatternParams(clause);
}

void QueryParser::ensureCorrectPatternParams(PatternClause &clause) {
    std::string patternType = parsingResult.getPatternClauseType(clause);
    TokenType secondParamType = clause.getSecondParam().getType();
    TokenType thirdParamType = clause.getThirdParam().getType();
    std::string thirdParamValue = clause.getThirdParam().getValue();
    if (patternType == "if") {

        if (thirdParamValue.empty()) {
            setSemanticError();
        } else if (secondParamType != TokenType::Wildcard || thirdParamType != TokenType::Wildcard) {
            throwSyntaxError();
        }

    } else if (patternType == "while") {
        // if third param exist its a semantic error
        if (secondParamType != TokenType::Wildcard) {
            throwSyntaxError();
        }

        if (!thirdParamValue.empty()) {
            setSemanticError();
        }
    } else if (patternType == "assign") {
        if (!thirdParamValue.empty()) {
            setSemanticError();
        }
    } else {
        setSemanticError();
    }
}





void QueryParser::checkBracketsBalanced(const std::string& expr) {
    int count = 0;
    for (char c : expr) {
        if (c == '(') {
            count++;
        } else if (c == ')') {
            count--;
        }
        if (count < 0) {
            throwSyntaxError();
        }
    }
    if (count != 0) {
        throwSyntaxError();
    }
}
void QueryParser::checkValidOperand(const std::string &operand) {
    if (operand.empty()) {
        throwSyntaxError();
    }
    std::regex pattern("^(?:[a-zA-Z][a-zA-Z0-9]*|[1-9][0-9]*|0)$");
    if (!std::regex_match(operand, pattern)) throwSyntaxError();
}
void QueryParser::checkExprSyntax(const std::string &expr) {
    bool prevOperand = false;
    bool prevOperator = false;
    bool prevRightBracket = false;
    std::string operand;
    for (char c : expr) {
        if (c == '(') {
            if (prevOperand && !operand.empty() || prevRightBracket) {
                throwSyntaxError();
            }
            prevOperand = true;
            prevOperator = false;
            prevRightBracket = false;
        } else if (c == ')') {
            if (prevOperator) {
                throwSyntaxError();
            }
            if (prevOperand) {
                checkValidOperand(operand);
                operand = "";
            }
            prevOperand = false;
            prevOperator = false;
            prevRightBracket = true;
        } else if (c == '+' || c == '-' || c == '*' || c == '%' || c == '/') {
            if (prevOperator) {
                throwSyntaxError();
            }
            if (!prevOperand && !prevRightBracket) {
                throwSyntaxError();
            }
            if (prevOperand) {
                checkValidOperand(operand);
                operand = "";
            }
            prevOperand = false;
            prevOperator = true;
            prevRightBracket = false;
        } else if (c == ' ') {
            continue;
        } else {
            if (prevRightBracket) {
                throwSyntaxError();
            }
            operand += c;
            prevOperand = true;
            prevOperator = false;
            prevRightBracket = false;
        }
    }
    if (prevOperator) {
        throwSyntaxError();
    }
    if (prevOperand) {
        checkValidOperand(operand);
    }
}
void QueryParser::checkValidExpr(const std::string& expr) {
    // check if expr is at least len 2
    if (expr.length() <= 2) {
        throwSyntaxError();
    }
    // Removes '"' from front and back of expr
    std::string exprWithoutQuotes = expr.substr(1, expr.length() - 2);
    checkBracketsBalanced(exprWithoutQuotes);
    checkExprSyntax(exprWithoutQuotes);
}
// Parses the expression specification in the query.
// Handles different forms of expressions like quoted constants, wildcards, or quoted expressions.
void QueryParser::parseExpressionSpec(PatternClause &clause) {
    std::string expr;
    if (match(TokenType::QuoutConst) || match(TokenType::QuoutIDENT) || match(TokenType::ExpressionSpec)) {
        expr = currentToken().getValue();
        checkValidExpr(expr);
        clause.setSecondParam(Token(TokenType::ExpressionSpec, expr));
        return;
    } else if (match(TokenType::Wildcard)) {

        if (peekNextToken(TokenType::Rparenthesis) || peekNextToken(TokenType::Comma)) {
            clause.setSecondParam(Token(TokenType::Wildcard, currentToken().getValue()));
            return;
        }
        advanceToken();

        if (!match(TokenType::QuoutIDENT) && !match(TokenType::QuoutConst) && !match(TokenType::ExpressionSpec)) {
            throwSyntaxError();
        }
        expr = currentToken().getValue();
        checkValidExpr(expr);
        advanceToken();
        ensureToken(TokenType::Wildcard);
        clause.setSecondParam(Token(TokenType::PartialExpressionSpec, expr));

        // building expressionspec with wildcards
    } else {
        throwSyntaxError();
    }


}



// Parses an expression in the query.
// Processes terms and operators within the expression.
void QueryParser::parseExpression() {
    parseTerm();

    while (match(TokenType::Operator)) {
        if (currentToken().getValue() != "+" && currentToken().getValue() != "-") {
			throwSyntaxError();
        }
        advanceToken();
        parseTerm();
    }
}

// Parses a term in an expression.
// Processes factors and operators within the term.
void QueryParser::parseTerm() {
    parseFactor();

    while (match(TokenType::Operator)) {
        if (currentToken().getValue() == "*" || currentToken().getValue() == "/"
        || currentToken().getValue() == "%") {
            advanceToken();
        }
        parseFactor();
    }
}

// Parses a factor in a term.
// Handles parentheses, variable names, and constant values.
void QueryParser::parseFactor() {
    if (match(TokenType::Lparenthesis)) {
        advanceToken();
        parseExpression();
        ensureToken(TokenType::Rparenthesis);
    } else if (!isVarName() && !isConstValue()) {
		throwSyntaxError();
    }
	advanceToken();

}

// Checks if the current token is a constant value.
bool QueryParser::isConstValue() {
    return match(TokenType::INTEGER);
}

// Checks if the current token is a variable name.
bool QueryParser::isVarName() {
    return match(TokenType::IDENT);
}

// Returns a constant reference to the current Token being parsed.
const Token& QueryParser::currentToken() const {
    return tokens[currentTokenIndex];
}



// Advances to the next token in the token sequence.
// Returns true if advancement is successful, false if it reaches the end of the token sequence.
void QueryParser::advanceToken() {
    if (currentTokenIndex < tokens.size() - 1) {
        ++currentTokenIndex;
    }
    else {
        throwSyntaxError();
    }
}

// Checks if the current token matches a given TokenType.
// Advances to the next token and returns true if it matches.
// Returns false if there is no match.
bool QueryParser::match(TokenType type) {
    if (currentToken().getType() == type) {
        return true;
    }
    return false;
}

// Ensures the next token matches the expected TokenType.
// Throws an error if the token does not match the expected type.
void QueryParser::ensureToken(TokenType expected) {
    if (!match(expected)) {
        throwSyntaxError();
    }
}

// Throws a standard invalid_argument exception with a custom error message for grammar error.
// Throws a standard invalid_argument exception with a custom error message for incomplete error.
void QueryParser::throwSyntaxError() {
    throw std::invalid_argument("SyntaxError");

}
// Parses a variable synonym in the query.
void QueryParser::parseEntSynonym() {
    ensureToken(TokenType::IDENT);
    // check if the token value is declared as variable synonyms in any of the set
    if (parsingResult.getDeclaredSynonym(currentToken().getValue()) != "variable" &&
        parsingResult.getDeclaredSynonym(currentToken().getValue()) != "procedure") {
        setSemanticError();
    }
}

void QueryParser::ensureSynonymType(std::string synType) {
    if (currentToken().getType() != TokenType::IDENT) {
        if (currentToken().getType() == TokenType::QuoutIDENT || currentToken().getType() == TokenType::Wildcard) {
            return;
        } else {
            setSemanticError();
        }
    }

    if (parsingResult.getDeclaredSynonym(currentToken().getValue()) != synType) {
        setSemanticError();
    }
}


// Define a mapping of token types to their valid statement synonyms


void QueryParser::parseStmtSynonyms() {
    ensureToken(TokenType::IDENT);
    TokenType currentTokenType = currentSuchThatToken.getType();
    const std::string currentValue = parsingResult.getDeclaredSynonym(currentToken().getValue());

    // Check if the current token type is in the map
    auto validTypesIt = validStmtSynonymsMap.find(currentTokenType);
    if (validTypesIt != validStmtSynonymsMap.end()) {
        // Found the token type, now check if the current value is in the set of valid synonyms for this type
        std::unordered_set<std::string> validSynonyms = validTypesIt->second;

        if (validSynonyms.find(currentValue) == validSynonyms.end()) {
            // The current value is not a valid synonym for the current token type
            setSemanticError();
        }
    }
}



// Parses an assignment synonym in the query.
void QueryParser::parsePatternSynonym() {
    ensureToken(TokenType::IDENT);
    if (!checkIfPatternSyn()) {
        setSemanticError();
    }
    advanceToken();
}



ParsingResult QueryParser::getParsingResult() {
    return makeResult(parsingResult);
}

// peek the next token
bool QueryParser::peekNextToken(TokenType type) {
    if (currentTokenIndex < tokens.size() - 1) {
        return tokens[currentTokenIndex + 1].getType() == type;
    }
    return false;
}

void QueryParser::ensureNextBlank() {
    if (currentTokenIndex < tokens.size() - 2 &&
    tokens[currentTokenIndex + 1].getType() == TokenType::Comma &&
    tokens[currentTokenIndex + 2].getType() == TokenType::Wildcard) {
        return;
    } else {
        throwSyntaxError();
    }

}

bool QueryParser::checkValidStmtNum() {
    std::string stringValue = currentToken().getValue();
    if (stoi(stringValue) <= 0) {
        return false;
    }
    return true;
}


void QueryParser::parseWithClause() {
    WithClause clause;

    clause.setRelationship(currentToken());
    advanceToken();
    if (match(TokenType::NotKeyword)) {
        clause.setAsNegated();
        advanceToken();
    }

    size_t startIndex = currentTokenIndex;
    TokenType firstRefType = parseRef();
    std::string concatenatedTokens = concatTokens(startIndex, currentTokenIndex);
    Token firstParam = Token(TokenType::Ref, concatenatedTokens);
    clause.setFirstParam(firstParam);

    advanceToken();
    ensureToken(TokenType::Equal);
    advanceToken();

    startIndex = currentTokenIndex;
    TokenType secondRefType = parseRef();
    concatenatedTokens = concatTokens(startIndex, currentTokenIndex);
    Token secondParam = Token(TokenType::Ref, concatenatedTokens);
    clause.setSecondParam(secondParam);
    if (firstRefType != secondRefType) {
        setSemanticError();
    }
    
    parsingResult.addWithClause(clause);
}




TokenType QueryParser::parseRef() {
    if (match(TokenType::QuoutIDENT)) {
        return TokenType::QuoutIDENT;
    }
    else if (match(TokenType::INTEGER)) {
        return TokenType::INTEGER;
    }
    else {
        parseAttrRef();
        return attrToTypeMap.find(currentToken().getValue())->second;
    }

}


void QueryParser::parseAttrRef() {
    if (!parsingResult.isSynonymDeclared(currentToken().getValue())) {
        setSemanticError();
    }
    parseAttr();
}

void QueryParser::parseAttr() {
    Token currentSynonym = currentToken();
    advanceToken();
    ensureToken(TokenType::Dot);
    advanceToken();
    if (!checkValidAttr(currentSynonym)) {
        setSemanticError();
    }
}


bool QueryParser::checkValidAttr(Token synToken) {
    ensureToken(TokenType::AttrName);
    std::string synValue = synToken.getValue();
    std::string synType = parsingResult.getRequiredSynonymType(synValue);
    std::string attrValue = currentToken().getValue();
    // find from validAttrMap if it of the right type
    auto validAttrIt = validAttrMap.find(synType);
    if (validAttrIt != validAttrMap.end()) {
        std::unordered_set<std::string> validAttrs = validAttrIt->second;
        if (validAttrs.find(attrValue) == validAttrs.end()) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}



bool QueryParser::checkIfStmt() {
    static const std::unordered_set<std::string> stmtTypes = {
            "stmt", "assign", "while", "if", "print", "call", "read"
    };

    if (match(TokenType::INTEGER)) {
        return true;
    }

    const auto& currentValue = parsingResult.getDeclaredSynonym(currentToken().getValue());
    return stmtTypes.find(currentValue) != stmtTypes.end();
}

bool QueryParser::checkIfEnt() {
    static const std::unordered_set<std::string> entTypes = {
            "variable", "procedure"
    };

    if (match(TokenType::QuoutIDENT)) {
        return true;
    }

    const auto& currentValue = parsingResult.getDeclaredSynonym(currentToken().getValue());
    return entTypes.find(currentValue) != entTypes.end();
}

bool QueryParser::checkIfPatternSyn() {
    static const std::unordered_set<std::string> patternTypes = {
            "assign", "while", "if"
    };
    const auto& currentValue = parsingResult.getDeclaredSynonym(currentToken().getValue());
    return patternTypes.find(currentValue) != patternTypes.end();
}

std::string QueryParser::concatTokens(size_t start, size_t end) {
    std::string concatenatedTokens;
    for (size_t i = start; i <= end; ++i) {
        concatenatedTokens += tokens[i].getValue();
    }
    return concatenatedTokens;
}

void QueryParser::checkForExistingSyntaxError() {
    if (tokens.empty()) {
        throwSyntaxError();
    }
    for (const Token &token : tokens) {
        if (token.getType() == TokenType::SyntaxError) {
            throwSyntaxError();
        }
    }
}

