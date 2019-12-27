#ifndef MESSAGES_H
#define MESSAGES_H

#include <vector>
#include <string>
#include "font.h"
#include <SDL2/SDL.h>

using namespace std;

class Messages {
	public:
		vector<string> messages;
		vector<int> lifeTimes;
		Messages() {
			this->messages = {};
			this->lifeTimes = {};
		}

		void append(string message) {
			this->messages.push_back(message);
			this->lifeTimes.push_back(300);
		}

		void update() {
			for (int i = 0; i < this->messages.size(); i++) {
				this->lifeTimes[i]--;
				if (this->lifeTimes[i] == 0) {
					this->messages.erase(this->messages.begin());
					this->lifeTimes.erase(this->lifeTimes.begin());
				}
			}
		}

		void draw(SDL_Renderer *ren, Font *font, int SCRREEN_SIZE[2], float SCRREEN_DIFF[2], bool update = true) {
			int textSize[2] = {0, 0};
			int forLen = 5;
			if (update) {this->update();}
			if (this->messages.size() <= 5) {forLen = this->messages.size();}
			for (int i = 0; i < forLen; i++) {	
				font->getSize((string)messages[i], &textSize[0], &textSize[1]);
				SDL_Color textColor = {255, 255, 255};
				font->render(ren, messages[i], SCRREEN_SIZE[0] - SCRREEN_DIFF[0]*10 - SCRREEN_DIFF[0]*(textSize[0]), SCRREEN_DIFF[1]*10 + SCRREEN_DIFF[1]*(i*(textSize[1]) + 5), &textColor);
			}
		}
};

#endif