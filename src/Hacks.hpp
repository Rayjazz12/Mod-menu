#pragma once

#include <string>
#include <vector>

/** A single row in the mod menu. */
struct HackDef {
    std::string name;
    std::string category;
    std::string id;
};

/** Category names, in sidebar order. */
std::vector<std::string> const& allCategories();

/** Every hack, in registration order. */
std::vector<HackDef> const& allHacks();

/**
 * Hacks matching a category and a search query.
 * A non-empty search queries every category, not just `category`.
 */
std::vector<HackDef> filterHacks(std::string const& category, std::string const& search);

bool isHackEnabled(std::string const& id);
void setHackEnabled(std::string const& id, bool on);

int enabledCount();
