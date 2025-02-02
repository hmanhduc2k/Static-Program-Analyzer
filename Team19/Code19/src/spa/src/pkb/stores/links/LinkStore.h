#pragma once

#include <unordered_set>
#include <unordered_map>

#include "ILinkReader.h"
#include "ILinkWriter.h"

// ai-gen start(copilot, 1, e)
// prompt: used copilot
/**
 * @brief A store that manages a collection of entities of a certain type.
 *
 * @tparam LinkerType The type of the entity linking.
 * @tparam LinkedType The type of the entity being linked.
 */
template <typename LinkerType, typename LinkedType>
class LinkStore : public ILinkReader<LinkerType, LinkedType>, public ILinkWriter<LinkerType, LinkedType> {
private:
    std::unordered_map<LinkerType, LinkedType> links;
    std::unordered_map<LinkedType, std::unordered_set<LinkerType>> reverseLinks;
    std::unordered_set<LinkedType> allLinked;
public:
    // ILinkWriter implementation
    void addLink(LinkerType linkerEntity, LinkedType linkedEntity) override {
        links[linkerEntity] = linkedEntity;
        reverseLinks[linkedEntity].insert(linkerEntity);
        allLinked.insert(linkedEntity);
    }

    void clear() override {
        links.clear();
        reverseLinks.clear();
    }

    // ILinkReader implementation
    bool isEmpty() override {
        return links.empty();
    }

    bool hasLink(LinkerType linkerEntity, LinkedType linkedEntity) override {
        return links.find(linkerEntity) != links.end() && links.at(linkerEntity) == linkedEntity;
    }

    std::unordered_set<LinkerType> getLinker(LinkedType entity) override {
        if (reverseLinks.find(entity) == reverseLinks.end()) {
            return std::unordered_set<LinkerType>();
        }
        return reverseLinks.at(entity);
    }

    LinkedType getLinked(LinkerType entity) override {
        return links.at(entity);
    }

    std::unordered_set<LinkedType> getAllLinked() override {
        return allLinked;
    }
};
// ai-gen end
