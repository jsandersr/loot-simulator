//---------------------------------------------------------------
//
// GameController.h
//

#pragma once

#include "GameTypes.h"

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace LootSimulator {

//===============================================================

enum class OptionCategory : uint32_t
{
	SLAY_MONSTER = 0,
	SLAY_RANDOM,
	QUIT,
	NUM_OPTION_CATEGORIES
};

// Selection, Count
using UserSelection = std::pair<int32_t, int32_t>;

class Game;
class GameEvents;
class GameView;
class GameController
{
public:
	GameController();
	~GameController();

	void Run();
	void Initialize();
	GameEvents& GetGameEvents();

	const std::string& GetMonsterName(MonsterType type);
	const std::string& GetTreasureName(TreasureType type);
	const std::set<Monster>& GetMonsters();;

private:
	UserSelection GetMoveInput();
	std::vector<std::string> TokenizeString(const std::string& input);
	bool IsValidInput(const std::vector<std::string>& tokens);
	void WaitForInput();
	OptionCategory GetOptionCategoryForSelection(int32_t selection);

private:
	std::unique_ptr<Game> m_game;
	std::unique_ptr<GameView> m_view;
	std::pair<int32_t, int32_t> m_optionSelectionRange;
};

//===============================================================

} // namespace Client
