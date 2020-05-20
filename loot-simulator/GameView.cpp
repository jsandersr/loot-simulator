//---------------------------------------------------------------
//
// GameView.cpp
//

#include "GameView.h"

#include "GameController.h"
#include "GameEvents.h"

#include <iostream>

namespace LootSimulator {

//===============================================================


static std::string ItemTypeToString(TreasureItem itemType)
{
	switch (itemType)
	{
	case LootSimulator::TREASURE_NONE:
		return "Nothing";
	case LootSimulator::TREASURE_GOLD_PILE:
		return "pile of gold";
	case LootSimulator::TREASURE_RUSTY_SWORD:
		return "Rusty Sword";
	case LootSimulator::TREASURE_SHARP_SWORD:
		"Sharp Sword";
	case LootSimulator::TREASURE_GODLY_SWORD:
		return "Godly Sword";
	case LootSimulator::TREASURE_SMALL_SHIELD:
		return "Small Shield";
	case LootSimulator::TREASURE_HEATER_SHIELD:
		return "Heater Sheidl";
	case LootSimulator::TREASURE_KITE_SHIELD:
		return "Kite Shield";
	case LootSimulator::TREASURE_REGENERATION_RING:
		return "Regeneration Ring";
	case LootSimulator::TREASURE_CURSED_RING:
		return "Cursed Ring";

		// fallthrough
	case LootSimulator::NUM_TREASURE_ITEMS:
	default:
		return std::string();
	}
}

static std::string MonsterTypeToString(MonsterType monsterType)
{
	switch (monsterType)
	{
	case LootSimulator::GOBLIN:
		return "Goblin";
	case LootSimulator::SKELETON:
		return "Skeleton";
	case LootSimulator::DRAGON:
		return "Dragon";
	case LootSimulator::NUM_MONSTER_TYPES:
	default:
		return std::string();
	}
}

static void PrintTreasureItem(const std::pair<TreasureItem, int>& itemSummary)
{
	std::cout << "\tLoot: " << ItemTypeToString(itemSummary.first) << "\n";
	std::cout << "\tCount: " << itemSummary.second;
	std::cout << "\n\n";
}

static void PrintTreasureCollection(const TreasureMap& treasureMap)
{
	for (const std::pair<TreasureItem, int32_t>& item : treasureMap)
	{
		PrintTreasureItem(item);
	}
}

static void PrintLootSummary(const LootMap& monsterSummary)
{
	for (const std::pair<MonsterType, TreasureMap>& monsterToTreasureMap : monsterSummary)
	{
		std::cout << "Monster: " << MonsterTypeToString(monsterToTreasureMap.first) << "\n";
		PrintTreasureCollection(monsterToTreasureMap.second);
		std::cout << "\n\n---------------------------------------------------------\n\n";
	}
}

GameView::GameView(GameController* gameController)
	: m_controller(gameController)
{

}

void GameView::Initialize()
{
	auto& e = m_controller->GetGameEvents();

	e.GetLoadingCompleteEvent().subscribe([]()
	{
		std::cout << "Loading Content Data Complete...\n\n";
	});

	e.GetGameErrorEvent().subscribe([](const std::string& errorMsg)
	{
		std::cout << "Game error occurred. errorMessage=";
		std::cout << "\n\t";
		std::cout << errorMsg;
	});

	e.GetMonsterSlainEvent().subscribe([](const std::string& monsterName)
	{
		std::cout << "You've slain a " << monsterName << "!";
	});

	e.GetLootDroppedEvent().subscribe([this](const LootMap& lootMap, int count)
	{
		std::cout << "\n\nYou just finished slaying " << count << " monster(s)!\n";
		std::cout << "Here is all the loot that dropped!\n\n";

		PrintLootSummary(lootMap);
	});
}

void GameView::PrintInvalidInputMessage()
{
	std::cout << "Invalid input. Must be in format"
		" <selection number (1-4)> <count number (1-99999)>"
		"\neg. 1 100"
		"\n\n";
}

void GameView::PrintGamePrompt()
{
	std::cout << "Select enemy to slay and collect loot!"
		"\n"
		"If you'd like to slay multiple, specify how many"
		" and a loot distribution table will show when we're done"
		" with all the carnage."
		"\n\n";

	const std::vector<std::string>& options = m_controller->GetOptionsStrings();
	for (const auto& option : options)
	{
		std::cout << option;
		std::cout << "\n";
	}
	std::cout << "\n";
}

void GameView::PrintMovePrompt()
{
	std::cout << "\n";
	std::cout << "<selection count>: ";
}

void GameView::PrintBackToMenuPrompt()
{
	std::cout << "\n";
	std::cout << "Press any key to go back to menu!";
}

//===============================================================

} // namespace LootSimulator
