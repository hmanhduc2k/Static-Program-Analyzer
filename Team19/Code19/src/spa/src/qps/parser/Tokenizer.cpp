#include "Tokenizer.h"

#include <memory>
#include <string>

// Initializes the Tokenizer with a given query string.
Tokenizer::Tokenizer(const std::string& query) : query(query) {
    // Regex to split based on quouted text, whitespaces, parentheses, commas, semicolons
    tokenRegex = std::regex(R"("[^"]*"|[\w`~@#$%^&*+{}:-]+|_|[,;.<>()\"\'']|[=])");

}

// This method tokenizes the stored query and returns a vector of Token objects.
std::vector<Token> Tokenizer::tokenize() {
    splitQuery();  // Call the splitQuery method to get tokens.

    return tokens;
}

// This method splits the query into tokens based on the regular expression.
// It returns a vector of Token objects representing the individual tokens.
void Tokenizer::splitQuery() {
    // Iterator to go through each token found by the std::regex.
    std::sregex_token_iterator iter(query.begin(), query.end(), tokenRegex);
    std::sregex_token_iterator end;  // End iterator.

    for (; iter != end; ++iter) {  // Loop through all tokens.
        // Check if the next iterator is available.
        auto nextIter = std::next(iter, 1);
        nextToken = nextIter != end ? *nextIter : std::string("");
        auto tokenType = determineTokenType(*iter);  // Determine the type of each token.
        if (tokenType == TokenType::AndKeyword) {
            tokens.emplace_back(tokenType, lastRelationship);  // Add the token to the vector.
        }
        else {
            tokens.emplace_back(tokenType, removeSpaces(*iter));  // Add the token to the vector.
        }
    }
}

// Determines the type of token based on its string representation.
// Returns the TokenType of the token.

TokenType Tokenizer::determineTokenType(const std::string& tokenStr) {
    if (tokenStr == "_") {
        return TokenType::Wildcard;
    }

    // List of token determination functions
    std::vector<std::function<TokenType(Tokenizer*, const std::string&)>> determinationFunctions = {
            &Tokenizer::determineClauseKeywordToken,
            &Tokenizer::determineBooleanToken,
            &Tokenizer::determineAttrNameToken,
            &Tokenizer::determineQuoutToken,
            &Tokenizer::determineSingleCharToken,
            &Tokenizer::determineDesignEntityToken,
            &Tokenizer::determineRelRefToken
    };

    for (auto& func : determinationFunctions) {
        TokenType tokenType = func(this, tokenStr);
        if (tokenType != TokenType::SyntaxError) {
            return tokenType;
        }
    }

    // Additional checks if the token does not fit any predefined patterns
    if (std::regex_match(tokenStr, std::regex("^[a-zA-Z][a-zA-Z0-9]*$"))) {
        return TokenType::IDENT;
    }
    if (std::regex_match(tokenStr, std::regex("^(0|[1-9][0-9]*)$"))) {
        return TokenType::INTEGER;
    }
    if (std::regex_match(tokenStr, std::regex("^[+\\-*/%&|<>=!~^]$"))) {
        return TokenType::Operator;
    }

    return TokenType::SyntaxError;
}

bool Tokenizer::isSyntaxError(TokenType tokenType) {
    return tokenType == TokenType::SyntaxError;
}

TokenType Tokenizer::determineClauseKeywordToken(const std::string& tokenStr) {
    if (std::regex_match(tokenStr, std::regex("^(Select|pattern|such|that|with|and|not)$"))) {
        if (!tokens.empty() && (isSynonym() || tokens.back().getType() == TokenType::SelectKeyword)) {
            return TokenType::IDENT;
        }

        auto it = tokenFunctionMap.find(tokenStr);
        if (it != tokenFunctionMap.end()) {
            return it->second(); // Execute the function
        }
    }
    return TokenType::SyntaxError;
}


TokenType Tokenizer::determineBooleanToken(const std::string& tokenStr) {
    if (tokenStr == "BOOLEAN") {
        if (!tokens.empty() && (isSynonym() || (tokens.back().getType() == TokenType::SelectKeyword && isBooleanDeclared))) {
            isBooleanDeclared = true;
            return TokenType::IDENT;
        }
        return TokenType::BooleanKeyword;
    }
    return TokenType::SyntaxError;
}

TokenType Tokenizer::determineQuoutToken(const std::string& tokenStr) {
    if (std::regex_match(tokenStr, std::regex("^\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\"$"))) {
        return TokenType::QuoutIDENT;
    }
    if (std::regex_match(tokenStr, std::regex("^\"\\s*[0-9]+\\s*\"$"))) {
        return TokenType::QuoutConst;
    }
    if (std::regex_match(tokenStr, std::regex("\".*\""))) {
        return TokenType::ExpressionSpec;
    }
    return TokenType::SyntaxError;
}

TokenType Tokenizer::determineSingleCharToken(const std::string& tokenStr) {
    if (signMap.find(tokenStr) != signMap.end()) {
        return signMap[tokenStr];
    }
    return TokenType::SyntaxError;
}

TokenType Tokenizer::determineDesignEntityToken(const std::string& tokenStr) {
    if (std::regex_match(tokenStr, std::regex("^(stmt|read|print|while|if|assign|variable|constant|procedure|call)$"))) {
        if (!tokens.empty() && (isSynonym() || tokens.back().getType() == TokenType::SelectKeyword))  {
            return TokenType::IDENT;
        } else {
            return TokenType::DesignEntity;
        }
    }
    return TokenType::SyntaxError;
}

TokenType Tokenizer::determineRelRefToken(const std::string& tokenStr) {
    if (std::regex_match(tokenStr, std::regex("^(Follows|Follows\\*|Parent|Parent\\*|Uses|Modifies|Next|Next\\*|Calls|Calls\\*|Affects)$"))) {
        if (!tokens.empty() && (isSynonym() || tokens.back().getType() == TokenType::SelectKeyword)) {
            return TokenType::IDENT;
        }

        if (clauseTypeMap.find(tokenStr) != clauseTypeMap.end()) {
            return clauseTypeMap[tokenStr];
        }
    }
    return TokenType::SyntaxError;
}

TokenType Tokenizer::determineAttrNameToken(const std::string& tokenStr) {
    if (std::regex_match(tokenStr, std::regex("^(procName|varName|value|stmt#)$"))) {
        if (!tokens.empty() && (isSynonym() || tokens.back().getType() == TokenType::SelectKeyword)) {
            return TokenType::IDENT;
        } else {
            return TokenType::AttrName;
        }
    }
    return TokenType::SyntaxError;
}

// Check if the current token is a declaration token
bool Tokenizer::checkIfDeclaration() {
    int i = tokens.size() - 1;
    while(i >= 0) {
        if (tokens[i].getType() == TokenType::DesignEntity) {
            return true;
        }
        if (tokens[i].getType() == TokenType::Semicolon) {
            return false;
        }
        i--;
    }
    return false;
}

std::string Tokenizer::removeSpaces(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

bool Tokenizer::isSynonym() {
    return checkIfDeclaration() || tokens.back().getType() == TokenType::Lparenthesis
           || tokens.back().getType() == TokenType::Comma
           || nextToken == ")"
        || (tokens.back().getType() == TokenType::PatternKeyword && nextToken == "(")
        || tokens.back().getType() == TokenType::LeftAngleBracket
        || (tokens.back().getType() == TokenType::AndKeyword && (tokens.back().getValue() == "pattern" && nextToken == "("))
        || (tokens.back().getType() == TokenType::AndKeyword && tokens.back().getValue() == "with" && nextToken == "value")
        || (tokens.back().getType() == TokenType::WithKeyword && nextToken == "." || nextToken == "." );
}
