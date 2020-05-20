//---------------------------------------------------------------
//
// GameController.h
//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace LootSimulator {

//===============================================================

enum Option
{
	SLAY_GOBLIN = 0,
	SLAY_SKELETON,
	SLAY_DRAGON,
	SLAY_RANDOM,
	QUIT
};
using UserSelection = std::pair<Option, int32_t>;

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
	const std::vector<std::string>& GetOptionsStrings() { return m_optionsStrings; }

private:
	void UpdateOptionsStrings();
	UserSelection GetMoveInput();
	std::vector<std::string> TokenizeString(const std::string& input);
	bool IsValidInput(const std::vector<std::string>& tokens);
	void WaitForInput();

private:
	std::unique_ptr<Game> m_game;
	std::unique_ptr<GameView> m_view;
	std::vector<std::string> m_optionsStrings;
};

//===============================================================

} // namespace Client
