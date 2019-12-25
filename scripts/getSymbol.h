#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

char getSymbol(SDL_Event e, bool shiftPressed) {
	string key = (string)SDL_GetKeyName(SDL_GetKeyFromScancode(e.key.keysym.scancode));
	if (key.find("Keypad ") < strlen(key.c_str())) {
		key.erase(0, 7);
	}
	char ch;
	if (e.key.keysym.sym == SDLK_SPACE) {
		return ' ';
	}
	if (strlen(key.c_str()) == 1) {
		ch = key.c_str()[0];
		if (shiftPressed) {
			return ch;
		}
		else {
			ch = tolower(ch);
			return ch;
		}
	}
	else {
		return '^';
	}
}