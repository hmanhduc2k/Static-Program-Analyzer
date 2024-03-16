#include "catch.hpp"
#include "sp/Parser/SimpleParserFacade.h"
#include <iostream>
#include <filesystem>
#include <fstream>

TEST_CASE("Program parsing throws an error for missing curly brace after procedure end.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure proc1 {"
		"	print x;"
		""
		"procedure proc2 "
		"	call y;"
		"}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for missing closing curly brace.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure invalidProcedure { call y;";
	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for missing curly braces.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure invalidProcedure call y;";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for extra variable in print statement") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure proc { print xyz call; }";
	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for missing parenthesis around !(cond_expr)") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure procedure {"
		"	while (!(read != 11) && !(read == while)) {" 
		"		print = 0;"
		"	}"
		"}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for extra parenthesis around cond_expr.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure procedure {"
		"	while ((read != 11)) {"
		"		print = 0;"
		"	}"
		"}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for invalid keyword.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure main { write x; }";
	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Program parsing throws an error for nested procedures.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure main { procedure nested { write x; }}";
	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	CHECK_THROWS_AS(parser.parse(), std::runtime_error);
	std::filesystem::remove(filename);
}

TEST_CASE("Single procedure, with read statement") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure main { read helloWorld; }";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();

	REQUIRE(tree_ptr->getType() == ASTNodeType::PROGRAMS);
	REQUIRE(tree_ptr->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::PROGRAMS)->second);

	std::vector<std::shared_ptr<ASTNode>> procedures = tree_ptr->getChildren();
	REQUIRE(procedures.size() == 1);

	REQUIRE(procedures[0]->getType() == ASTNodeType::PROCEDURE);
	REQUIRE(procedures[0]->getValue() == "main");

	std::shared_ptr<ASTNode> statement = procedures[0]->getChildByIndex(0)->getChildByIndex(0);
	REQUIRE(statement->getType() == ASTNodeType::READ);
	REQUIRE(statement->getLineNumber() == 1);
	REQUIRE(statement->getChildByIndex(0)->getValue() == "helloWorld");
	std::filesystem::remove(filename);
}

TEST_CASE("Single procedure, all possible conditional expressions in while statements") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure conditionalExpressions {"
		"	while(x == 2) { read x; }"
		"	while(x != 2) { read x; }"
		"	while(x < 2) { read x; }"
		"	while(x <= 2) { read x; }"
		"	while(x > 2) { read x; }"
		"	while(x >= 2) { read x; }"
		"	while(!(x == 2)) { read x; }"
		"	while(0 == 2147483647) { read x; }"
		"	while(0 != 2147483647) { read x; }"
		"	while(0 < 2147483647) { read x; }"
		"	while(0 <= 2147483647) { read x; }"
		"	while(0 > 2147483647) { read x; }"
		"	while(0 >= 2147483647) { read x; }"
		"	while(!(0 == 2147483647)) { read x; }"
		"	while((x == 2) && (x == 1)) { read x; }"
		"	while((x != 2) || (x != 2)) { read x; }"
		"	while((!(x == 2)) && (!(x == 1))) { read x; }"
		"	while( ((x == 2) && (x == 1)) || (x == 3)) { read x; }"
		"	while(!(!(!(!(!(!(!(!(!(!(x==3))))))))))) { read x; }"
		"	while(((x <= y) || (!(y == z))) && (!(z >= x))) { read x; }"
		"}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();
	std::shared_ptr<ASTNode> procedure = tree_ptr->getChildByIndex(0);

	// Checking procedure node
	REQUIRE(procedure->getType() == ASTNodeType::PROCEDURE);
	REQUIRE(procedure->getValue() == "conditionalExpressions");

	std::vector<std::shared_ptr<ASTNode>> statements = procedure->getChildByIndex(0)->getChildren();

	REQUIRE(statements.size() == 20);

	for (int i = 0; i < statements.size(); i++) {
		std::shared_ptr<ASTNode> statement = statements[i];
		REQUIRE(statement->getType() == ASTNodeType::WHILE);

		std::shared_ptr<ASTNode> contents = statement->getChildByIndex(1)->getChildByIndex(0);
		REQUIRE(contents->getType() == ASTNodeType::READ);
		REQUIRE(contents->getChildByIndex(0)->getValue() == "x");
	}
	std::filesystem::remove(filename);
}

TEST_CASE("Multiple procedures, all names that may be potential keywords.") {
	// Generate test file
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure assign { read x; }"
		"procedure call { read x; }"
		"procedure ant { read x; }"
		"procedure while { read x; }"
		"procedure if { read x; }"
		"procedure else { read x; }"
		"procedure then { read x; }"
		"procedure print { read x; }"
		"procedure read { read x; }"
		"procedure stmtLst { read x; }"
		"procedure variable { read x; }"
		"procedure stmt { read x; }"
		"procedure procedure { read x; }"
		"procedure program { read x; }"
		"procedure Follows { read x; }"
		"procedure FollowsT { read x; }"
		"procedure Parent { read x; }"
		"procedure ParentT { read x; }"
		"procedure Uses { read x; }"
		"procedure UsesP { read x; }"
		"procedure UsesS { read x; }"
		"procedure Modifies { read x; }"
		"procedure ModifiesP { read x; }"
		"procedure ModifiesS { read x; }"
		"procedure Calls { read x; }"
		"procedure CallsT { read x; }"
		"procedure Next { read x; }"
		"procedure NextT { read x; }"
		"procedure Affects { read x; }"
		"procedure AffectsT { read x; }";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();

	REQUIRE(tree_ptr->getType() == ASTNodeType::PROGRAMS);
	REQUIRE(tree_ptr->getLineNumber() == -1);
	REQUIRE(tree_ptr->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::PROGRAMS)->second);

	REQUIRE(tree_ptr->getChildren().size() == 30);
	std::filesystem::remove(filename);
}

TEST_CASE("Parsing single program with all possible statements types.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure procedure {"
		"	while (!(read > procedure)) {"
		"	if = if;"
		"	} "
		""
		"	if (then < 2) then {"
		"		else = else;"
		"	} else {"
		"		while = then;"
		"	}"
		""
		"	read = 1 + program;"
		""
		"	call call;"
		"	print read;"
		"	read print;"
		"} procedure call { print k; }";
	
	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();

	std::vector<std::shared_ptr<ASTNode>> statements = tree_ptr->getChildByIndex(0)->getChildByIndex(0)->getChildren();
	REQUIRE(statements.size() == 6);

	SECTION("Testing While Statement") {
		std::shared_ptr<ASTNode> whileStatement = statements[0];
		REQUIRE(whileStatement->getType() == ASTNodeType::WHILE);
		REQUIRE(whileStatement->getLineNumber() == 1);
		REQUIRE(whileStatement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::WHILE)->second);

		std::vector<std::shared_ptr<ASTNode>> whileChildren = whileStatement->getChildren();
		REQUIRE(whileChildren.size() == 2);

		SECTION("Testing predicates tree child node") {
			REQUIRE(whileChildren[0]->getType() == ASTNodeType::NOT);

			std::shared_ptr<ASTNode> cond_expr = whileChildren[0]->getChildByIndex(0);

			REQUIRE(cond_expr->getType() == ASTNodeType::GREATER);
			REQUIRE(cond_expr->getLineNumber() == 1);
			REQUIRE(cond_expr->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::GREATER)->second);

			std::vector<std::shared_ptr<ASTNode>> children = cond_expr->getChildren();
			REQUIRE(children.size() == 2);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 1);
			REQUIRE(children[0]->getValue() == "read");

			REQUIRE(children[1]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[1]->getLineNumber() == 1);
			REQUIRE(children[1]->getValue() == "procedure");
		}

		SECTION("Testing Statement List tree child node") {
			REQUIRE(whileChildren[1]->getType() == ASTNodeType::STATEMENT_LIST);
			std::shared_ptr<ASTNode> statement = whileChildren[1]->getChildByIndex(0);

			REQUIRE(statement->getType() == ASTNodeType::ASSIGN);
			REQUIRE(statement->getLineNumber() == 2);
			REQUIRE(statement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::ASSIGN)->second);

			std::vector<std::shared_ptr<ASTNode>> children = statement->getChildren();
			REQUIRE(children.size() == 2);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 2);
			REQUIRE(children[0]->getValue() == "if");

			REQUIRE(children[1]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[1]->getLineNumber() == 2);
			REQUIRE(children[1]->getValue() == "if");
		}
	}

	SECTION("Testing if-then-else statement") {
		std::shared_ptr<ASTNode> ifStatement = statements[1];

		REQUIRE(ifStatement->getType() == ASTNodeType::IF_ELSE_THEN);
		REQUIRE(ifStatement->getLineNumber() == 3);
		REQUIRE(ifStatement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::IF_ELSE_THEN)->second);

		std::vector<std::shared_ptr<ASTNode>> ifChildren = ifStatement->getChildren();
		REQUIRE(ifChildren.size() == 3);

		SECTION("Testing predicates tree child node") {
			std::shared_ptr<ASTNode> cond_expr = ifChildren[0];

			REQUIRE(cond_expr->getType() == ASTNodeType::LESSER);
			REQUIRE(cond_expr->getLineNumber() == 3);
			REQUIRE(cond_expr->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::LESSER)->second);

			std::vector<std::shared_ptr<ASTNode>> children = cond_expr->getChildren();
			REQUIRE(children.size() == 2);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 3);
			REQUIRE(children[0]->getValue() == "then");

			REQUIRE(children[1]->getType() == ASTNodeType::CONSTANT);
			REQUIRE(children[1]->getLineNumber() == 3);
			REQUIRE(children[1]->getValue() == "2");
		}

		SECTION("Testing If-then statement list node") {
			REQUIRE(ifChildren[1]->getType() == ASTNodeType::STATEMENT_LIST);
			std::shared_ptr<ASTNode> statement = ifChildren[1]->getChildByIndex(0);

			REQUIRE(statement->getType() == ASTNodeType::ASSIGN);
			REQUIRE(statement->getLineNumber() == 4);
			REQUIRE(statement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::ASSIGN)->second);

			std::vector<std::shared_ptr<ASTNode>> children = statement->getChildren();
			REQUIRE(children.size() == 2);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 4);
			REQUIRE(children[0]->getValue() == "else");

			REQUIRE(children[1]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[1]->getLineNumber() == 4);
			REQUIRE(children[1]->getValue() == "else");
		}

		SECTION("Testing Else statement list node") {
			REQUIRE(ifChildren[1]->getType() == ASTNodeType::STATEMENT_LIST);
			std::shared_ptr<ASTNode> statement = ifChildren[2]->getChildByIndex(0);

			REQUIRE(statement->getType() == ASTNodeType::ASSIGN);
			REQUIRE(statement->getLineNumber() == 5);
			REQUIRE(statement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::ASSIGN)->second);

			std::vector<std::shared_ptr<ASTNode>> children = statement->getChildren();
			REQUIRE(children.size() == 2);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 5);
			REQUIRE(children[0]->getValue() == "while");

			REQUIRE(children[1]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[1]->getLineNumber() == 5);
			REQUIRE(children[1]->getValue() == "then");
		}
	}

	SECTION("Testing assign statement") {
		std::shared_ptr<ASTNode> assignStatement = statements[2];

		REQUIRE(assignStatement->getType() == ASTNodeType::ASSIGN);
		REQUIRE(assignStatement->getLineNumber() == 6);
		REQUIRE(assignStatement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::ASSIGN)->second);

		SECTION("Testing tree child node") {
			std::vector<std::shared_ptr<ASTNode>> children = assignStatement->getChildren();
			REQUIRE(children.size() == 2);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 6);
			REQUIRE(children[0]->getValue() == "read");

			REQUIRE(children[1]->getType() == ASTNodeType::ADD);
			REQUIRE(children[1]->getLineNumber() == 6);
			REQUIRE(children[1]->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::ADD)->second);

			SECTION("Testing expression of children") {
				std::vector<std::shared_ptr<ASTNode>> ants = children[1]->getChildren();
				REQUIRE(ants.size() == 2);
				REQUIRE(ants[0]->getType() == ASTNodeType::CONSTANT);
				REQUIRE(ants[0]->getLineNumber() == 6);
				REQUIRE(ants[0]->getValue() == "1");

				REQUIRE(ants[1]->getType() == ASTNodeType::VARIABLE);
				REQUIRE(ants[1]->getLineNumber() == 6);
				REQUIRE(ants[1]->getValue() == "program");
			}
		}
	}

	SECTION("Testing call statement") {
		std::shared_ptr<ASTNode> callStatement = statements[3];
		REQUIRE(callStatement->getType() == ASTNodeType::CALL);
		REQUIRE(callStatement->getLineNumber() == 7);
		REQUIRE(callStatement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::CALL)->second);

		SECTION("Testing tree child node") {
			std::vector<std::shared_ptr<ASTNode>> children = callStatement->getChildren();
			REQUIRE(children.size() == 1);
			REQUIRE(children[0]->getType() == ASTNodeType::PROCEDURE);
		}
	}

	SECTION("Testing print statement") {
		std::shared_ptr<ASTNode> printStatement = statements[4];
		REQUIRE(printStatement->getType() == ASTNodeType::PRINT);
		REQUIRE(printStatement->getLineNumber() == 8);
		REQUIRE(printStatement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::PRINT)->second);

		SECTION("Testing tree child node") {
			std::vector<std::shared_ptr<ASTNode>> children = printStatement->getChildren();
			REQUIRE(children.size() == 1);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 8);
			REQUIRE(children[0]->getValue() == "read");
		}
	}

	SECTION("Testing read statement") {
		std::shared_ptr<ASTNode> readStatement = statements[5];
		REQUIRE(readStatement->getType() == ASTNodeType::READ);
		REQUIRE(readStatement->getLineNumber() == 9);
		REQUIRE(readStatement->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::READ)->second);

		SECTION("Testing tree child node") {
			std::vector<std::shared_ptr<ASTNode>> children = readStatement->getChildren();
			REQUIRE(children.size() == 1);
			REQUIRE(children[0]->getType() == ASTNodeType::VARIABLE);
			REQUIRE(children[0]->getLineNumber() == 9);
			REQUIRE(children[0]->getValue() == "print");
		}
	}
	std::filesystem::remove(filename);
}

TEST_CASE("Calling parseProgram for complex procedure", "[parse][program]") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure computeCentroid {"
		"	count = 0;"
		"	cenX = 0;"
		"	cenY = 0;"
		"	while ((x != 0) && (y != 0)) {"
		"		count = count + 1;"
		"		cenX = cenX + x;"
		"		cenY = cenY + y;"
		"		call readPoint;"
		"	}"
		"	if (count == 0) then {"
		"		flag = 1;"
		"	} else {"
		"		cenX = cenX / count ;"
		"		cenY = cenY / count ;"
		"	}"
		"	normSq = cenX * cenX + cenY * cenY;"
		"}"
		""
		"procedure procedure {"
		"	print k;"
		"}"
		""
		"procedure readPoint {"
		"	read x;"
		"	read y;"
		"}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();

	REQUIRE(tree_ptr->getType() == ASTNodeType::PROGRAMS);
	std::shared_ptr<ASTNode> procedure = tree_ptr->getChildByIndex(0);

	// Checking procedure node
	REQUIRE(procedure->getType() == ASTNodeType::PROCEDURE);
	REQUIRE(procedure->getValue() == "computeCentroid");

	std::vector<std::shared_ptr<ASTNode>> statements = procedure->getChildByIndex(0)->getChildren();

	// Check the statements of the parsed code
	REQUIRE(statements.size() == 6);

	std::shared_ptr<ASTNode> firstStmt = statements[0];

	// Check the first statement
	REQUIRE(firstStmt->getType() == ASTNodeType::ASSIGN);
	std::vector<std::shared_ptr<ASTNode>> firstStmtChildren = firstStmt->getChildren();

	REQUIRE(firstStmtChildren.size() == 2);
	REQUIRE(firstStmtChildren[0]->getType() == ASTNodeType::VARIABLE);
	REQUIRE(firstStmtChildren[0]->getValue() == "count");
	REQUIRE(firstStmtChildren[1]->getType() == ASTNodeType::CONSTANT);
	REQUIRE(firstStmtChildren[1]->getValue() == "0");

	std::shared_ptr<ASTNode> lastStmt = statements[5];

	// Check the last statement
	REQUIRE(lastStmt->getType() == ASTNodeType::ASSIGN);
	std::vector<std::shared_ptr<ASTNode>> lastStmtChildren = lastStmt->getChildren();
	REQUIRE(lastStmtChildren.size() == 2);
	REQUIRE(lastStmtChildren[0]->getType() == ASTNodeType::VARIABLE);
	REQUIRE(lastStmtChildren[0]->getValue() == "normSq");
	REQUIRE(lastStmtChildren[1]->getType() == ASTNodeType::ADD);

	std::vector<std::shared_ptr<ASTNode>> additionValues = lastStmtChildren[1]->getChildren();
	REQUIRE(additionValues.size() == 2);
	REQUIRE(additionValues[0]->getType() == ASTNodeType::MULTIPLY);
	REQUIRE(additionValues[1]->getType() == ASTNodeType::MULTIPLY);

	std::vector<std::shared_ptr<ASTNode>> firstExpr = additionValues[0]->getChildren();
	REQUIRE(firstExpr.size() == 2);
	REQUIRE(firstExpr[0]->getType() == ASTNodeType::VARIABLE);
	REQUIRE(firstExpr[0]->getValue() == "cenX");
	REQUIRE(firstExpr[1]->getType() == ASTNodeType::VARIABLE);
	REQUIRE(firstExpr[1]->getValue() == "cenX");

	std::vector<std::shared_ptr<ASTNode>> lastExpr = additionValues[1]->getChildren();
	REQUIRE(lastExpr.size() == 2);
	REQUIRE(lastExpr[0]->getType() == ASTNodeType::VARIABLE);
	REQUIRE(lastExpr[0]->getValue() == "cenY");
	REQUIRE(lastExpr[1]->getType() == ASTNodeType::VARIABLE);
	REQUIRE(lastExpr[1]->getValue() == "cenY");
	std::filesystem::remove(filename);
}

TEST_CASE("Parsing single procedure that contains 20 nested while loops.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure nestedWhile {"
		"while (x == 2) {"
		"while (x != 2) {"
		"while (x < 2) {"
		"while (x <= 2) {"
		"while (x > 2) {"
		"while (x >= 2) {"
		"while (!(x == 2)) {"
		"while (0 == 2147483647) {"
		"while (0 != 2147483647) {"
		"while (0 < 2147483647) {"
		"while (0 <= 2147483647) {"
		"while (0 > 2147483647) {"
		"while (0 >= 2147483647) {"
		"while (!(0 == 2147483647)) {"
		"while ((x == 2) && (x == 1)) {"
		"while ((x != 2) || (x != 2)) {"
		"while ((!(x == 2)) && (!(x == 1))) {"
		"while (((x == 2) && (x == 1)) || (x == 3)) {"
		"while (!(!(!(!(!(!(!(!(!(!(x == 3))))))))))) {"
		"while (((x <= y) || (!(y == z))) && (!(z >= x))) {"
		"read x; }}}}}}}}}}}}}}}}}}}}}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();

	std::shared_ptr<ASTNode> loop = tree_ptr->getChildByIndex(0)->getChildByIndex(0)->getChildByIndex(0);
	int line = 1;

	while (line <= 20) {
		REQUIRE(loop->getType() == ASTNodeType::WHILE);
		REQUIRE(loop->getLineNumber() == line);
		loop = loop->getChildByIndex(1)->getChildByIndex(0);
		line++;
	}

	REQUIRE(loop->getType() == ASTNodeType::READ);
	REQUIRE(loop->getLineNumber() == line);
	std::filesystem::remove(filename);
}

TEST_CASE("Parsing single procedure with nested while and if.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = 
		"procedure nestedIfWhile {"
		"	while (x == y) {"
		"		if (x == y) then {"
		"			while(x > y) {"
		"				read x;"
		"			}"
		""
		"			while (y > x) {"
		"				read y;"
		"			}"
		"		} else {"
		"			if (x != y) then {"
		"				while(!(x == y)) {"
		"					read y;"
		"				}"
		"			} else {"
		"				while (x >= y) {"
		"					read y;"
		"				}"
		"			}"
		""
		"			if (x >= y) then {"
		"				while(x <= y) {"
		"					read y;"
		"				}"
		"			} else {"
		"				while ((y > x) || (!(y == x))) {"
		"					read y;"
		"				}"
		"			}"
		"		}"
		"	}"
		"}";

	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();
	std::shared_ptr<ASTNode> loop = tree_ptr->getChildByIndex(0)->getChildByIndex(0)->getChildByIndex(0);
	REQUIRE(loop->getType() == ASTNodeType::WHILE);
	REQUIRE(loop->getLineNumber() == 1);
	loop = loop->getChildByIndex(1)->getChildByIndex(0);

	REQUIRE(loop->getType() == ASTNodeType::IF_ELSE_THEN);
	REQUIRE(loop->getLineNumber() == 2);

	SECTION("Testing internal If-then.") {
		std::vector<std::shared_ptr<ASTNode>> ifThenStatementList = loop->getChildByIndex(1)->getChildren();
		REQUIRE(ifThenStatementList.size() == 2);

		REQUIRE(ifThenStatementList[0]->getType() == ASTNodeType::WHILE);
		REQUIRE(ifThenStatementList[0]->getLineNumber() == 3);

		REQUIRE(ifThenStatementList[1]->getType() == ASTNodeType::WHILE);
		REQUIRE(ifThenStatementList[1]->getLineNumber() == 5);
	}

	SECTION("Testing internal Else.") {
		std::vector<std::shared_ptr<ASTNode>> elseStatementList = loop->getChildByIndex(2)->getChildren();
		REQUIRE(elseStatementList.size() == 2);

		SECTION("Testing first if-else.") {
			std::shared_ptr<ASTNode> if1 = elseStatementList[0];
			REQUIRE(if1->getType() == ASTNodeType::IF_ELSE_THEN);
			REQUIRE(if1->getLineNumber() == 7);

			std::shared_ptr<ASTNode> if1while1 = if1->getChildByIndex(1)->getChildByIndex(0);
			REQUIRE(if1while1->getType() == ASTNodeType::WHILE);
			REQUIRE(if1while1->getLineNumber() == 8);

			std::shared_ptr<ASTNode> if1while2 = if1->getChildByIndex(2)->getChildByIndex(0);
			REQUIRE(if1while2->getType() == ASTNodeType::WHILE);
			REQUIRE(if1while2->getLineNumber() == 10);
		}

		SECTION("Testing second if-else.") {
			std::shared_ptr<ASTNode> if2 = elseStatementList[1];
			REQUIRE(if2->getType() == ASTNodeType::IF_ELSE_THEN);
			REQUIRE(if2->getLineNumber() == 12);

			std::shared_ptr<ASTNode> if2while1 = if2->getChildByIndex(1)->getChildByIndex(0);
			REQUIRE(if2while1->getType() == ASTNodeType::WHILE);
			REQUIRE(if2while1->getLineNumber() == 13);

			std::shared_ptr<ASTNode> if2while2 = if2->getChildByIndex(2)->getChildByIndex(0);
			REQUIRE(if2while2->getType() == ASTNodeType::WHILE);
			REQUIRE(if2while2->getLineNumber() == 15);
		}
	}
	std::filesystem::remove(filename);
}

TEST_CASE("Parse program with whitespace in the token stream.") {
	std::string filename = "sample.txt";
	std::string sampleProgram = "procedure main { read helloWorld; } \n\n";
	std::ofstream file;
	file.open(filename);
	file << sampleProgram;
	file.close();
	REQUIRE(std::filesystem::exists(filename));
	SimpleParserFacade parser(filename);
	std::shared_ptr<ASTNode> tree_ptr = parser.parse();

	REQUIRE(tree_ptr->getType() == ASTNodeType::PROGRAMS);
	REQUIRE(tree_ptr->getValue() == ASTUtility::getASTNodeType.find(ASTNodeType::PROGRAMS)->second);

	std::vector<std::shared_ptr<ASTNode>> procedures = tree_ptr->getChildren();
	REQUIRE(procedures.size() == 1);

	REQUIRE(procedures[0]->getType() == ASTNodeType::PROCEDURE);
	REQUIRE(procedures[0]->getValue() == "main");

	std::shared_ptr<ASTNode> statement = procedures[0]->getChildByIndex(0)->getChildByIndex(0);
	REQUIRE(statement->getType() == ASTNodeType::READ);
	REQUIRE(statement->getLineNumber() == 1);
	REQUIRE(statement->getChildByIndex(0)->getValue() == "helloWorld");

	std::filesystem::remove(filename);
}