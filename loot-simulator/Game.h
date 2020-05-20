//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "GameTypes.h"
#include "GameEvents.h"
#include "nlohmann/json/json.hpp"

#include <memory>
#include <optional>
#include <set>
#include <vector>

namespace LootSimulator {

//===============================================================

class GameEvents;
class Game {
public:
	Game();

	// Populates all of our monsters from the data files.
	bool LoadData();

	// Slay a single monster. If type is not set, we'll pick random types.
	void SlayMonster(std::optional<MonsterType> type);

	// Slay many monsters. If type is not set, we'll pick random types.
	void SlayBatchOfMonsters(int32_t count, std::optional<MonsterType> type);

	GameEvents& GetGameEvents() { return *m_events.get(); };
	const std::string& GetMonsterName(MonsterType type);
	const std::string& GetTreasureName(TreasureType type);
	const std::set<Monster>& GetMonsters() { return m_monsterData; }

private:
	Monster CreateRandomMonster();
	Monster CreateMonsterForType(MonsterType type);

private:
	// Events for us to fire when interesting things happen.
	std::unique_ptr<GameEvents> m_events;

	// One of each monster goes here, populated from data.
	std::set<Monster> m_monsterData;

	// Aggregate of all treasure items.
	std::set<Treasure> m_treasureData;

	// Used to track loot history.
	LootMap m_droppedLootMap;

	bool m_isDataLoaded = false;
};

// nlohmann::json helpers.

using nlohmann::json;
void to_json(json& j, const Monster& m);
void from_json(const json& j, Monster& m);

void to_json(json& j, const LootTable& m);
void from_json(const json& j, LootTable& m);

void to_json(json& j, const Treasure& m);
void from_json(const json& j, Treasure& m);

//===============================================================

} // namespace LootSimulator
