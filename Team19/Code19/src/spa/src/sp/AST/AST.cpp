#include "AST.h"

AST::AST(std::unique_ptr<ASTNode> root) : root(std::move(root)) {}

const ASTNode* AST::getRoot() const {
    return root.get();
}