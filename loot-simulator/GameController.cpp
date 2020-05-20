//---------------------------------------------------------------
//
// GameController.cpp
//

#include "GameController.h"

#include "Game.h"
#include "GameView.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <iostream>

namespace LootSimulator {

//===============================================================

	// We're not going to consider any more than this number of digits in a selection.
	static const int32_t s_maxOptionDigits = 5;
	static const int32_t s_maxInputs = 2;

	// Min and Max number of monsters to slay.
	static const std::pair<int32_t, int32_t> s_validCountRange = { 0, 99999 };

GameController::GameController()
	: m_game(std::make_unique<Game>())
	, m_view(std::make_unique<GameView>(this))
{
}

GameController::~GameController()
{

}

void GameController::Run()
{
	if (!m_game->LoadData())
	{
		return;
	}

	Initialize();

	bool done = false;
	while (!done)
	{
		UserSelection selection = GetMoveInput();

		// We need to figure out which category we selected.
		OptionCategory optionCategory = GetOptionCategoryForSelection(selection.first);

		int32_t count = selection.second;

		if (optionCategory == OptionCategory::QUIT)
		{
			done = true;
		}
		else
		{
			std::optional<MonsterType> type;
			if (optionCategory != OptionCategory::SLAY_RANDOM)
			{

				type  = static_cast<MonsterType>(selection.first - 1);
			}

			if (count == 1)
			{
				m_game->SlayMonster(type);
			}
			else
			{
				m_game->SlayBatchOfMonsters(count, type);
			}
		}

		WaitForInput();
	}
}

void GameController::Initialize()
{
	const std::set<Monster>& monsters = m_game->GetMonsters();

	m_optionSelectionRange = { 1, monsters.size()  + static_cast<int32_t>(OptionCategory::NUM_OPTION_CATEGORIES) - 1};

	m_view->Initialize();
}

GameEvents& GameController::GetGameEvents()
{
	return m_game->GetGameEvents();
}

const std::string& GameController::GetMonsterName(MonsterType type)
{
	return m_game->GetMonsterName(type);
}

const std::string& GameController::GetTreasureName(TreasureType type)
{
	return m_game->GetTreasureName(type);
}

const std::set<LootSimulator::Monster>& GameController::GetMonsters()
{
	return m_game->GetMonsters();
}

UserSelection GameController::GetMoveInput()
{
	m_view->PrintGamePrompt();

	std::vector<std::string> inputList;
	bool isValidInput = false;
	while (!isValidInput)
	{
		m_view->PrintMovePrompt();
		std::string input;
		std::getline(std::cin, input);

		inputList = TokenizeString(input);
		isValidInput = IsValidInput(inputList);

		if (!isValidInput)
		{
			m_view->PrintInvalidInputMessage();
		}
	}

	int selection = std::stoi(inputList[0]);

	// Always default to slaying at least one monster if option not specified.
	int count = inputList.size() == 2 ? std::stoi(inputList[1]) : 1;
	return { selection , count };
}

std::vector<std::string> GameController::TokenizeString(const std::string& str)
{
	std::istringstream iss(str);
	return { std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>() };
}

bool GameController::IsValidInput(const std::vector<std::string>& tokens)
{
	// Any input other than a source and a destination is invalid.
	if (tokens.empty() || tokens.size() > s_maxInputs)
	{
		return false;
	}

	// Ensure that both inputs are numbers.
	auto allInputsAreNumbers = [](const std::string& inputStr)
	{
		return inputStr.find_first_not_of("0123456789") == std::string::npos;
	};

	if (!std::all_of(std::cbegin(tokens), std::cend(tokens), allInputsAreNumbers))
	{
		return false;
	}

	const std::string& firstToken = tokens[0];

	int selectionNum = std::stoi(firstToken.substr(0, s_maxOptionDigits));
	bool isInputValid = selectionNum >= m_optionSelectionRange.first
		&& selectionNum <= m_optionSelectionRange.second;

	// Second param is optional.
	if (tokens.size() == 2)
	{
		const std::string& secondToken = tokens[1];
		int32_t numToSlay = std::stoi(secondToken.substr(0, s_maxOptionDigits));
		isInputValid &= numToSlay >= s_validCountRange.first
			&& numToSlay <= s_validCountRange.second;
	}

	// Ensure that the numbers are within the proper range.
	return isInputValid;
}

void GameController::WaitForInput()
{
	m_view->PrintBackToMenuPrompt();
	std::cin.ignore(std::numeric_limits <std::streamsize>::max(), '\n');
}

OptionCategory GameController::GetOptionCategoryForSelection(int32_t selection)
{
	int numMonsters = m_game->GetMonsters().size();
	if (selection < numMonsters)
	{
		return OptionCategory::SLAY_MONSTER;
	}
	
	return static_cast<OptionCategory>(selection - numMonsters);
}

//===============================================================

} // namespace Client
