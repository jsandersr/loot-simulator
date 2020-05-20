//---------------------------------------------------------------
//
// Application.cpp
//

#include "GameController.h"

int main()
{
	LootSimulator::GameController gc;
	gc.Initialize();
	gc.Run();

	return 0;
}
