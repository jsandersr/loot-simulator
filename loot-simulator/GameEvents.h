//---------------------------------------------------------------
//
// GameEvents.h
//

#pragma once

#include "Game.h"
#include "GameTypes.h"
#include <observable.hpp>

namespace LootSimulator {

//===============================================================

class GameEvents {

public:
	GameEvents() = default;
	~GameEvents() = default;

	// Fired when all data is finished loading.
	using LoadingCompleteEvent = observable::subject<void()>;
	LoadingCompleteEvent& GetLoadingCompleteEvent() { return m_loadingCompleteEvent; }

	// Called when a monster is slain. For now just used to show the name of the defeated monster.
	using MonsterSlainEvent = observable::subject<void(const std::string& monsterName)>;
	MonsterSlainEvent& GetMonsterSlainEvent() { return m_monsterSlainEvent; }

	// Called after slaying multiple monsters. The loot data is collected and sent out through here.
	using LootDroppedEvent = observable::subject<void(const LootMap& lootBatch, int count)>;
	LootDroppedEvent& GetLootDroppedEvent() { return m_lootDroppedEvent; }

	// TODO: could add error codes instead.
	// Called on an event error.
	using GameErrorEvent = observable::subject<void(const std::string& errorMsg)>;
	GameErrorEvent& GetGameErrorEvent() { return m_gameErrorEvent; }


private:
	LoadingCompleteEvent m_loadingCompleteEvent;
	MonsterSlainEvent m_monsterSlainEvent;
	LootDroppedEvent m_lootDroppedEvent;
	GameErrorEvent m_gameErrorEvent;
};

//===============================================================

} // namespace LootSimulator
