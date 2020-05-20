//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "GameEvents.h"
#include "Log.h"

#include <fstream>
#include <random>

namespace LootSimulator {

//===============================================================

NLOHMANN_JSON_SERIALIZE_ENUM(MonsterType,
{
	{ GOBLIN,   "goblin"   },
	{ SKELETON, "skeleton" },
	{ DRAGON,   "dragon"   }
});

//TODO: This could be generated via a script,
// It only needs to exist in the same namespace as the enum
NLOHMANN_JSON_SERIALIZE_ENUM(TreasureItem,
{
	{ TREASURE_NONE,              "none"             },
	{ TREASURE_GOLD_PILE,         "goldPile"         },
	{ TREASURE_RUSTY_SWORD,       "rustySword"       },
	{ TREASURE_SHARP_SWORD,       "sharpSword"       },
	{ TREASURE_GODLY_SWORD,       "godlySword"       },
	{ TREASURE_SMALL_SHIELD,      "smallShield"      },
	{ TREASURE_HEATER_SHIELD,     "heaterShield"     },
	{ TREASURE_KITE_SHIELD,       "kiteShield"       },
	{ TREASURE_REGENERATION_RING, "regenerationRing" },
	{ TREASURE_CURSED_RING,       "cursedRing"       }
});

static std::string GetRelativePath(const std::string& path)
{
	std::string relativePath = "../";
	relativePath.append(path);
	return relativePath;
}

//---------------------------------------------------------------

static std::random_device s_rd;
static std::mt19937 s_mt(s_rd());
static float GetRandomFloat(float lowerBound, float upperBound)
{
	std::uniform_real_distribution<> dist(lowerBound, upperBound);
	return static_cast<float>(dist(s_mt));
}

static int32_t GetRandomInt(int32_t lowerBound, int32_t upperBound)
{
	std::uniform_int_distribution<> dist(lowerBound, upperBound);
	return static_cast<int>(dist(s_mt));
}

//---------------------------------------------------------------

Treasure LootTable::Roll() const
{
	// Roulette selection.
	float weightTotal = 0.0f;
	for (auto t : treasures)
	{
		weightTotal += t.dropRate;
	}

	float randomNumber = GetRandomFloat(0.0f, weightTotal);
	for (auto t : treasures)
	{

		if (randomNumber < t.dropRate)
		{
			return t;
		}

		randomNumber -= t.dropRate;
	}

	// Shouldn't ever get here.
	return {};
}

//---------------------------------------------------------------


// if we have mutual exclusive tables
// if drop rate doesn't add up to 100
// roll to try and hit the mutual exclusive one
	// if not, then roll each of the other tables.


void Monster::RerollLoot()
{
	lootDrops.clear();

	// Drop rates imply mutual exclusivity.
	// Split tables that have drop rates from tables that drop no matter what.
	auto it = std::partition(std::begin(tables), std::end(tables),
		[](const LootTable& table)
	{
		return table.dropRate != 1.0f;
	});

	uint32_t numExclusive = std::distance(std::begin(tables), it);

	// We're going to pick loot from either any exclusive table, OR the remaining tables.
	float exclusiveTableDropRate = 0.0f;
	std::vector<float> dropRates;
	for (uint32_t i = 0; i < numExclusive; ++i)
	{
		float dropRate = tables[i].dropRate;
		dropRates.push_back(dropRate);
		exclusiveTableDropRate += dropRate;
	}
	dropRates.push_back(1.0f - exclusiveTableDropRate);

	float randomNumber = GetRandomFloat(0.0f, 1.0f);

	size_t tableIndex = 0;
	for (; tableIndex < dropRates.size(); ++tableIndex)
	{
		float dropRate = dropRates[tableIndex];
		if (randomNumber < dropRate)
		{
			break;
		}
		randomNumber -= dropRate;
	}

	// If we picked an exclusive table, we're done! Otherwise,
	// we'll pick from the other tables.
	if (tableIndex < numExclusive)
	{
		lootDrops.push_back(tables[tableIndex].Roll());
		return;
	}

	// Tables which have a 100% drop rate will all roll a loot piece.
	for (size_t i = numExclusive; i < tables.size(); ++i)
	{
		lootDrops.push_back(tables[i].Roll());
	}
}

//---------------------------------------------------------------

static const std::string s_monsterData = "../resources/monsters.json";
static const std::string s_test = "../resources/loot-tables/variety-tier-1.json";

Game::Game()
	: m_events(std::make_unique<GameEvents>())
{
}

void Game::LoadData()
{
	// All of our data is defined here.
	std::ifstream fileStream(s_monsterData);
	if (!fileStream.is_open())
	{
		LOG_DEBUG("Could not open file. file=" + s_monsterData);
		return;
	}

	using json = nlohmann::json;
	json monsterJsonData;
	fileStream >> monsterJsonData;

	uint32_t numMonsterTypes = monsterJsonData["numMonsterTypes"];
	m_monstersTemplates.reserve(numMonsterTypes);

	for (const auto& monsterJson: monsterJsonData["monsters"])
	{
		m_monstersTemplates.emplace_back(monsterJson.get<Monster>());
	}

	m_isDataLoaded = true;
	m_events->GetLoadingCompleteEvent().notify();
}

void Game::SlayMonster(std::optional<MonsterType> type)
{
	m_droppedLootMap.clear();
	if (!m_isDataLoaded)
	{
		LOG_DEBUG("Attempted to say monster with no data loaded.");
		return;
	}
	Monster& m = type != std::nullopt
		? GetMonsterForType(type.value())
		: GetRandomMonster();

	m.RerollLoot();

	std::vector<std::string> lootNames;
	std::transform(std::cbegin(m.lootDrops), std::cend(m.lootDrops),
		std::back_inserter(lootNames),
		[](const Treasure& t)
	{
		return t.name;
	});

	if (!lootNames.empty())
	{
		// Begin populating the results.
		TreasureMap treasures = m_droppedLootMap[m.type];
		for (auto treasure : m.lootDrops)
		{
			// Insert into the map if it doesn't exist and increment its count.
			treasures[treasure.type]++;
		}

		m_droppedLootMap[m.type] = treasures;
		m_events->GetLootDroppedEvent().notify(m_droppedLootMap, 1);
	}

}

void Game::SlayBatchOfMonsters(int32_t count, std::optional<MonsterType> type)
{
	m_droppedLootMap.clear();

	// We're going to build a large pile of loot and report the results.
	// This is simply so the console doesn't scroll forever on large numbers
	// of monster slayings requested.

	bool shouldRerollType = !type.has_value();
	Monster m = !shouldRerollType
		? GetMonsterForType(type.value())
		: GetRandomMonster();

	int remaining = count;
	while (remaining--)
	{
		// If we weren't given a type then every monster will be random.
		if (shouldRerollType)
		{
			m = GetRandomMonster();
		}

		m.RerollLoot();

		// Begin populating the results.
		TreasureMap treasures = m_droppedLootMap[m.type];
		for (auto treasure : m.lootDrops)
		{
			// Insert into the map if it doesn't exist and increment its count.
			treasures[treasure.type]++;
		}

		m_droppedLootMap[m.type] = treasures;
	}

	m_events->GetLootDroppedEvent().notify(m_droppedLootMap, count);

	return;
}

Monster Game::GetRandomMonster()
{
	int32_t randomNumber = GetRandomInt(0, NUM_MONSTER_TYPES - 1);
	return GetMonsterForType(static_cast<MonsterType>(randomNumber));
}

Monster Game::GetMonsterForType(MonsterType type)
{
	auto it = std::find_if(std::begin(m_monstersTemplates), std::end(m_monstersTemplates),
		[type](const Monster& monster)
	{
		return monster.type == type;
	});

	return *it;
}


//---------------------------------------------------------------
// nlohmann::json helpers.

void to_json(json& j, const Monster& m)
{
	j = json {
		{ "name", m.name },
		{ "type", m.type },
		{ "lootData", m.tables }
	};
}

void from_json(const json& j, Monster& m)
{
	j.at("name").get_to(m.name);
	j.at("type").get_to(m.type);
	j.at("tables").get_to(m.tables);
}

void to_json(json& j, const LootTable& table)
{
	j = json {
		{ "dropRate", table.dropRate },
		{ "path", table.path }
	};
}

void from_json(const json& j, LootTable& table)
{
	// Set some optional data.
	// Tables are not required to have a drop rate and default to 100%.
	if (j.find("dropRate") != j.end())
	{
		j.at("dropRate").get_to(table.dropRate);
	}

	// Find and populate treasures.
	j.at("path").get_to(table.path);

	std::vector<Treasure> treasures;
	std::ifstream fileStream(GetRelativePath(table.path));
	if (!fileStream.is_open())
	{
		LOG_DEBUG("Could not open treasure data file. file=" + table.path);
		return;
	}

	json treasureJsonData;
	fileStream >> treasureJsonData;
	treasures.reserve(treasures.size() + treasureJsonData["numItems"]);

	for (const auto& treasureData : treasureJsonData["items"])
	{
		treasures.emplace_back(treasureData.get<Treasure>());
	}

	table.treasures = std::move(treasures);

}

void to_json(json& j, const Treasure& treasure)
{
	j = json {
		{ "type", treasure.type },
		{ "name", treasure.name },
		{ "dropRate", treasure.dropRate }
	};
}

void from_json(const json& j, Treasure& treasure)
{
	j.at("type").get_to(treasure.type);
	j.at("name").get_to(treasure.name);
	j.at("dropRate").get_to(treasure.dropRate);
}

//===============================================================

} // namespace LootSimulator
