#include "sp/DesignExtractor/Extractor/FollowsExtractor.h"

void FollowsExtractor::extract() {
	// Extract all the follows relationships
	this->pkbWriterManager->getFollowsWriter()->addFollows(
		this->ast1->lineNumber, this->ast2->lineNumber
	);
}