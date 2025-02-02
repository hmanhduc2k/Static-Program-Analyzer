#pragma once

#include <memory>

#include "pkb/stores/patterns/control/IControlPatternWriter.h"
#include "pkb/stores/patterns/control/WhilePatternStore.h"

// ai-gen start(copilot, 2, e)
// prompt: used copilot
class WhilePatternWriter: public IControlPatternWriter {
private:
	std::shared_ptr<WhilePatternStore> whilePatternStore;
public:
	WhilePatternWriter(std::shared_ptr<WhilePatternStore> store);

	/**
	 * @brief Adds an "While" control variable to the store.
	 * @param statementNumber The statement number of the while statement.
	 * @param whileControlVariable The control variable of the while statement.
	 */
	void addWhileControlVariable(int statementNumber, const std::string& whileControlVariable);

	// Clears all while patterns from the store.
	void clear() override;

	void addControlPattern(int statementNumber, const std::string& controlPattern) override;
};
// ai-gen end
