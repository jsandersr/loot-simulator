//---------------------------------------------------------------
//
// GameTypes.h
//

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace LootSimulator {

enum MonsterType {
	GOBLIN = 0,
	SKELETON,
	DRAGON,
	NUM_MONSTER_TYPES
};

enum TreasureItem {
	TREASURE_NONE = -1,
	TREASURE_GOLD_PILE = 0,
	TREASURE_RUSTY_SWORD,
	TREASURE_SHARP_SWORD,
	TREASURE_GODLY_SWORD,
	TREASURE_SMALL_SHIELD,
	TREASURE_HEATER_SHIELD,
	TREASURE_KITE_SHIELD,
	TREASURE_REGENERATION_RING,
	TREASURE_CURSED_RING,
	NUM_TREASURE_ITEMS
};

// treasure item, count
using TreasureMap = std::unordered_map<TreasureItem, int32_t>;

// monster to treasure dropped.
using LootMap = std::unordered_map <MonsterType, TreasureMap>;

struct Treasure
{
	//--------------------------
	// Model data
	// Indicates the type of item.
	TreasureItem type = TREASURE_NONE;

	// Display name for this item.
	std::string name;

	// Indicates how likely this item is to drop. 0.0f - 1.0f.
	float dropRate = 0.0f;
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
};

} // namespace LootSimulator
