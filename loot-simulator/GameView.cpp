//---------------------------------------------------------------
//
// GameView.cpp
//

#include "GameView.h"

#include "GameController.h"
#include "GameEvents.h"

#include <algorithm>
#include <iostream>

namespace LootSimulator {

//===============================================================


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

	e.GetLootDroppedEvent().subscribe([this](const LootSession& lootSession)
	{
		int32_t monsterTotal = std::accumulate(std::cbegin(lootSession.monsterCounts),
			std::cend(lootSession.monsterCounts), 0,
			[](int value, const std::pair<MonsterType, int32_t>& counts)
		{
			return value + counts.second;
		});

		std::cout << "\n\nYou just finished slaying " << monsterTotal << " monster(s)!\n";
		std::cout << "Here is all the loot that dropped!\n\n";

		PrintLootSummary(lootSession, monsterTotal);
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

	uint32_t optionNum = 1;
	uint32_t numOptionsCategories = static_cast<uint32_t>(OptionCategory::NUM_OPTION_CATEGORIES);
	for (uint32_t i = 0; i < numOptionsCategories; ++i)
	{
		OptionCategory optionCategory = static_cast<OptionCategory>(i);
		switch (optionCategory)
		{
		case OptionCategory::SLAY_MONSTER:
		{
			const std::set<Monster>& monsters = m_controller->GetMonsters();
			for (const auto& monster : monsters)
			{
				 std::cout << optionNum++ << ". " << monster.name << "\n";
			}
		}
		break;
		case OptionCategory::SLAY_RANDOM:
			std::cout << optionNum++ << ". " << "Random" << "\n";
			break;
		case OptionCategory::QUIT:
			std::cout << optionNum++ << ". " << "Quit" << "\n";
			break;
		default:
			std::cout << "Unsupported option category. optionCategory=" << i;
			break;
		}
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

void GameView::PrintTreasureItem(const std::pair<TreasureType, int>& itemSummary, int32_t totalMonsterCount)
{
	int32_t totalItemCount = itemSummary.second;
	float percentOfTotal = static_cast<float>(totalItemCount) / static_cast<float>(totalMonsterCount) * 100;

	std::cout << "\tLoot: " << m_controller->GetTreasureName(itemSummary.first) << "\n";
	std::cout << "\tCount: " << itemSummary.second << "(" << percentOfTotal << "%)";
	std::cout << "\n\n";
}

void GameView::PrintTreasureCollection(const TreasureMap& treasureMap, int32_t totalMonsterCount)
{
	for (const std::pair<TreasureType, int32_t>& item : treasureMap)
	{
		PrintTreasureItem(item, totalMonsterCount);
	}
}

void GameView::PrintLootSummary(const LootSession& lootSession, int32_t totalMonsterCount)
{
	for (MonsterType type : lootSession.monsters)
	{
		std::cout << "Monster: " << m_controller->GetMonsterName(type) << "\n";
		std::cout << "Count: " << lootSession.monsterCounts.at(type) << "\n";

		PrintTreasureCollection(lootSession.lootMap.at(type), totalMonsterCount);
		std::cout << "\n\n---------------------------------------------------------\n\n";
	}
}

//===============================================================

} // namespace LootSimulator
