#pragma once

#include "pkb/stores/entities/IEntityReader.h"
#include "pkb/stores/entities/types/VariableStore.h"

// ai-gen start(copilot, 2, e)
// prompt: used copilot
class VariableReader: public IEntityReader<std::string> {
private:
    std::shared_ptr<VariableStore> variableStore;
public:
    explicit VariableReader(std::shared_ptr<VariableStore> vs) {
        variableStore = std::move(vs);
    }

    bool isEmpty() const override {
        return variableStore->isEmpty();
    }

    std::unordered_set<std::string> getAllEntities() const override {
        return getAllVariables();
    }

    bool contains(std::string variable) const override {
      return hasVariable(variable);
    }

    // Custom methods
    /**
     * @brief Gets all variable names.
     * @return unordered_set<string>: The set of all variable names.
     */
    std::unordered_set<std::string> getAllVariables() const {
      return variableStore->getAllEntities();
    }

    /**
     * @brief Checks if a specific variable exists.
     * @param variable The variable name to check.
     * @return true if the variable exists, false otherwise.
     */
    bool hasVariable(std::string variable) const {
      return variableStore->contains(std::move(variable));
    }
};
// ai-gen end
