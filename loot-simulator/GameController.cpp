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

	static const int32_t s_maxInputs = 2;

	// Selection is a single number
	static const int32_t s_selectionInputMaxSize = 1;

	// Min and max selectable option number.
	static const std::pair<int32_t, int32_t> s_validSelectionRange = { 0, 5 };

	static const int s_countInputMaxSize = 5;

	// Min and Max number of monsters to slay.
	static const std::pair<int32_t, int32_t> s_validCountRange = { 0, 99999 };

	static const int32_t s_defaultNumMonsters = 1;

	static const int32_t s_randomOption = 4;
	static const int32_t s_quitOption = 5;

	static const Option s_options[5]{ SLAY_GOBLIN , SLAY_SKELETON , SLAY_DRAGON , SLAY_RANDOM, QUIT };

	static MonsterType ToMonsterType(Option option)
	{
		switch (option)
		{
		case SLAY_GOBLIN:
			return GOBLIN;
		case SLAY_SKELETON:
			return SKELETON;
		case SLAY_DRAGON:
			return DRAGON;
		case SLAY_RANDOM:
		case QUIT:
		default:
			std::cout << "Invalid monter type";
			return NUM_MONSTER_TYPES;
		}
	}

GameController::GameController()
	: m_game(std::make_unique<Game>())
	, m_view(std::make_unique<GameView>(this))
{
	m_view->Initialize();
}

GameController::~GameController()
{

}

void GameController::Run()
{
	m_game->LoadData();

	bool done = false;
	while (!done)
	{
		UserSelection selection = GetMoveInput();
		Option option = selection.first;
		int32_t count = selection.second;

		if (selection.first == QUIT)
		{
			done = true;
		}
		else
		{
			std::optional<MonsterType> type;
			if (option != SLAY_RANDOM)
			{
				type = ToMonsterType(option);
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
	UpdateOptionsStrings();
}

GameEvents& GameController::GetGameEvents()
{
	return m_game->GetGameEvents();
}

void GameController::UpdateOptionsStrings()
{
	m_optionsStrings.clear();

	for (auto option : s_options)
	{
		switch (option)
		{
		case SLAY_GOBLIN:
			m_optionsStrings.push_back("1. Goblin");
			break;
		case SLAY_SKELETON:
			m_optionsStrings.push_back("2. Skeleton");
			break;
		case SLAY_DRAGON:
			m_optionsStrings.push_back("3. Dragon");
			break;
		case SLAY_RANDOM:
			m_optionsStrings.push_back("4. Random");
			break;
		case QUIT:
			m_optionsStrings.push_back("5. Quit");
			break;
		default:
			break;
		}
	}
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
	return { s_options[selection - 1] , count };
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
	if (firstToken.size() > s_countInputMaxSize)
	{
		return false;
	}

	int selectionNum = std::stoi(firstToken);
	bool isInputValid = selectionNum >= s_validSelectionRange.first
		&& selectionNum <= s_validSelectionRange.second;

	// Second param is optional.
	if (tokens.size() == 2)
	{
		const std::string& secondToken = tokens[1];
		if (secondToken.size() > s_countInputMaxSize)
		{
			return false;
		}

		int32_t numToSlay = std::stoi(secondToken);
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

//===============================================================

} // namespace Client
