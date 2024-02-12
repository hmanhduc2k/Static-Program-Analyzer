// ai-gen start (gpt, 0, e)
// prompt https://chat.openai.com/share/a181de60-e76f-496c-9bee-7ea80f2be651

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "pkb/PKBWriter.h"
#include "sp/AST/ASTNode.h" // Assuming you have ASTNode defined in ASTNode.h

class DesignExtractor {
public:
    // Constructor taking in the root node of the AST
    DesignExtractor(std::shared_ptr<ASTNode> root, std::shared_ptr<PKBWriter> pkbWriter);

    const ASTNode& getRoot() const {
        return *root;
    }

    std::unordered_map<std::string, std::unordered_set<std::string>> designs;
    void extractAll();
    void populatePKB();


    // all the method below should be private in the future
    // Method to extract all variables from the AST
    void extractFollows();
    void extractFollowsStar();
    void extractParents();
    void extractParentsStar();
    void extractUses();
    void extractModifies();
    void extractAssigns();
    std::unordered_set<std::string> extractConstants();
    std::unordered_set<std::string> extractProcedures();
    std::unordered_set<std::string> extractStatements();
    void extractIf();
    void extractWhiles();
    std::unordered_set<std::string> extractCall();
    std::unordered_set<std::string> extractRead();
    std::unordered_set<std::string> extractPrint();
    std::unordered_set<std::string> extractVariables();
    std::shared_ptr<ASTNode> root;
    std::shared_ptr<PKBWriter> pkbWriter;

private:
    void entityRecursiveExtractor(const std::shared_ptr<ASTNode>& node, std::vector<ASTNode>& entities, ASTNodeType type);
    void statementRecursiveExtractor(const std::shared_ptr<ASTNode>& node, std::vector<ASTNode>& statements, ASTNodeType type);
    void procedureRecursiveExtractor(const std::shared_ptr<ASTNode>& node, std::vector<ASTNode>& procedures);

    // To be implemented later
    // void relationRecursiveExtractor(const std::shared_ptr<ASTNode>& node, std::vector<ASTNode>& relations, ASTNodeType type);

    // void recursivelyExtractVariables(const std::shared_ptr<ASTNode>& node, std::vector<ASTNode>& variables);
	// void recursivelyExtractConstants(const std::shared_ptr<ASTNode>& node, std::vector<ASTNode>& constants);
};