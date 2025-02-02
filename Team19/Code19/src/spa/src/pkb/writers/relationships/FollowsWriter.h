#pragma once

#include <memory>

#include "pkb/stores/relationships/IRelationshipWriter.h"
#include "pkb/stores/relationships/types/FollowsStore.h"

// ai-gen start(copilot, 2, e)
// prompt: used copilot
class FollowsWriter : public IRelationshipWriter<int, int> {
private:
    std::shared_ptr<FollowsStore> followsStore;

public:
    explicit FollowsWriter(std::shared_ptr<FollowsStore> store);

    void addRelationship(int precedingStmt, int followingStmt) override;

    void clear() override;

    // Custom methods
    /**
     * @brief Adds a "Follows" relationship between a preceding statement and a following statement.
     * @param stmtNum The statement number of the preceding statement.
     * @param followingStmt The statement number of the following statement.
     */
    void addFollows(int stmtNum, int followingStmt);
};
// ai-gen end
