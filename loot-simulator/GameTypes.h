//---------------------------------------------------------------
//
// GameTypes.h
//

#pragma once

#include "generated/EnumDataBindings.h"

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace LootSimulator {

// Treasure item, count.
using TreasureMap = std::unordered_map<TreasureType, int32_t>;

// Monster to count of monster.
using MonsterCountMap = std::unordered_map<MonsterType, int32_t>;

// Monster to treasure dropped.
using LootMap = std::unordered_map <MonsterType, TreasureMap>;

struct LootSession {

	// List of monster types involved in this loot session. Used to look up
	// other data.
	std::set<MonsterType> monsters;

	// Records the counts of each monster slain during this session.
	MonsterCountMap monsterCounts;

	// Records the loot that drops for each monster.
	LootMap lootMap;
};

struct Treasure
{
	//--------------------------
	// Model data
	// Indicates the type of item.
	TreasureType type = TreasureType::NONE;

	// Display name for this item.
	std::string name;

	// Indicates how likely this item is to drop. 0.0f - 1.0f.
	float dropRate = 0.0f;

	friend bool operator<(const Treasure& l, const Treasure& r)
	{
		return l.type < r.type; // keep the same order
	}
};

struct LootTable
{
	Treasure Roll() const;

	//--------------------------
	// Model data

	// Contains the paths to all of the loot tables that this is comprised of.
	std::string path;

	// Indicates how likely this table is to be pulled from. 0.0f - 1.0f. Default is 100% chance.
	float dropRate = 1.0f;

	// List of possible treasures this table can drop[.
	std::vector<Treasure> treasures;
};

// TODO: Potentially split out the data model from the logical bits - if there is time.
struct Monster
{
	// Rolls on loot for this monster.
	void RerollLoot();

	// Populated by rolling on loot.
	std::vector<Treasure> lootDrops;

	//--------------------------
	// Model data

	std::string name;
	MonsterType type;

	// List of loot tables we can choose from.
	std::vector<LootTable> tables;

	friend bool operator<(const Monster& l, const Monster& r)
	{
		return l.type < r.type; // keep the same order
	}
};

} // namespace LootSimulator
