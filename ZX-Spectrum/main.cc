#include <vector>
#include <cstdint>
#include <random>
#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "Window.h"


int main(int, char **)
{

	SDL_Init(SDL_INIT_EVERYTHING);

	try {
		Window win;

		win.main();
	} catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "An unknown exception has occured!" << std::endl;
		return 2;
	}

	return 0;
}
