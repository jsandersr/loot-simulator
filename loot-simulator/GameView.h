//---------------------------------------------------------------
//
// GameView.h
//

#pragma once

#include "GameTypes.h"

#include <utility>

namespace LootSimulator {

//===============================================================
class GameController;
class GameView {
public:
	GameView(GameController* gameController);

	void Initialize();

	void PrintInvalidInputMessage();
	void PrintGamePrompt();
	void PrintMovePrompt();
	void PrintBackToMenuPrompt();

private: 
	void PrintTreasureItem(const std::pair<TreasureType, int>& itemSummary, int32_t totalMonsterCount);
	void PrintTreasureCollection(const TreasureMap& treasureMap, int32_t totalMonsterCount);
	void PrintLootSummary(const LootSession& lootSessions, int32_t totalMonsterCount);

private:
	GameController* m_controller = nullptr;
};

//===============================================================

}