//---------------------------------------------------------------
//
// GameView.h
//

#pragma once

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
	GameController* m_controller = nullptr;
};

//===============================================================

}