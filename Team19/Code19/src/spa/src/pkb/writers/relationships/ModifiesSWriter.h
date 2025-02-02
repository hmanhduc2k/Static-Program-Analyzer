#pragma once

#include <memory>

#include "pkb/stores/relationships/IRelationshipWriter.h"
#include "pkb/stores/relationships/types/ModifiesSStore.h"

// ai-gen start(copilot, 2, e)
// prompt: used copilot
class ModifiesSWriter : public IRelationshipWriter<int , std::string> {
private:
    std::shared_ptr<ModifiesSStore> modifiesSStore;

public:
    explicit ModifiesSWriter(std::shared_ptr<ModifiesSStore> store);

    void addRelationship(int stmtNum, std::string var) override;

    void clear() override;

    // Custom methods
    /**
     * @brief Adds a "Modifies" relationship between a statement and a variable.
     * @param stmtNum The statement number of the statement that modifies the variable.
     * @param var The variable that is modified by the statement.
     */
    void addModifiesS(int stmtNum, std::string var);
};
// ai-gen end
