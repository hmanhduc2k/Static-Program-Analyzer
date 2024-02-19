#include "sp/DesignExtractor/FacadeDesignExtractor.h"
#include "sp/AST/ASTNode.h"
#include "pkb/PKBManager.h"
#include "sp/Utility.h"
#include "catch.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <vector>


TEST_CASE("Tests for DesignExtractors for Procedure", "[DesignExtractor::extract]") {
	std::cout << "BEGIN DESIGN EXTRACTOR TEST FOR PROCEDURE" << std::endl;
	std::shared_ptr<ASTNode> root = std::make_shared<ASTNode>(ASTNode());
	std::shared_ptr<ASTNode> prog = std::make_shared<ASTNode>(ASTNode(ASTNodeType::PROGRAMS, 0, "prog"));
	std::shared_ptr<ASTNode> proc1 = std::make_shared<ASTNode>(ASTNode(ASTNodeType::PROCEDURE, 1, "proc1"));
	std::shared_ptr<ASTNode> proc2 = std::make_shared<ASTNode>(ASTNode(ASTNodeType::PROCEDURE, 2, "proc2"));

	prog->addChild(proc1);
	prog->addChild(proc2);
	root->addChild(prog);
	std::shared_ptr<PKBManager> pkb = std::make_shared<PKBManager>();
	std::shared_ptr<PKBWriterManager> pkbWriterManager = pkb->getPKBWriterManager();
	FDesignExtractor fde(root, pkbWriterManager);
	REQUIRE_NOTHROW(fde.extractAll());

	std::shared_ptr<PKBReaderManager> pkbReaderManager = pkb->getPKBReaderManager();
	std::unordered_set<std::string> expectedProcNames = { "proc1", "proc2"};

	auto val = pkbReaderManager->getProcedureReader()->getAllProcedures();
	REQUIRE(val == expectedProcNames);

	std::cout << "END DESIGN EXTRACTOR TEST FOR PROCEDURE" << std::endl;
}
