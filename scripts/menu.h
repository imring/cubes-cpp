#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SFML/Network.hpp>
#include "font.h"
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include "cube.h"
#include "getSymbol.h"

using sf::IpAddress;
using namespace std;

string symbolsAllowedForName = "1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm *-+`.";
string symbolsAllowedForAddress = "1234567890.";
string symbolsAllowedForPort = "1234567890";

namespace menu {
 
    void draw_player_in_menu(SDL_Renderer *ren, vector<Player> &players, TTF_Font *nameFont, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        short int step = (1366 - 40) / players.size();
        for (int player = 0; player < players.size(); player++) {
            players[player].update_colors();
            players[player].draw_in_menu(ren, nameFont, 20 + (player) * step + step / 2 - 20, 80, SCREEN_DIFF);
        }
    }

    void draw_player_in_menu_client(SDL_Renderer *ren, vector<Player> &players, TTF_Font *nameFont, float SCREEN_DIFF[2]) {
        short int step = (1366 - 40) / players.size();
        for (int player = 0; player < players.size(); player++) {
            players[player].draw_in_menu(ren, nameFont, 20 + (player) * step + step / 2 - 20, 80, SCREEN_DIFF);
        }
    }

    void draw_options(SDL_Renderer *ren, vector<string> options, vector<string> optionsAddOns, int currentOption, Font *font, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        for (int option = 0; option < options.size(); option++) {
            if (option != currentOption) {
                SDL_Color textColor = {120, 120, 120};
                font->render(ren, options[option] + optionsAddOns[option], 50, SCREEN_SIZE[1] - SCREEN_DIFF[1]*10 - SCREEN_DIFF[1] * options.size()*50 + SCREEN_DIFF[1] * option*50, &textColor);
            }
            else {
                font->render(ren, options[option] + optionsAddOns[option], 50, SCREEN_SIZE[1] - SCREEN_DIFF[1]*10 - SCREEN_DIFF[1] * options.size()*50 + SCREEN_DIFF[1] * option*50, NULL);
            }
        }
    }

    void draw_ip_and_port(SDL_Renderer *ren, string ipAddress, string port, Font *font, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        font->render(ren, "ADDRESS: " + ipAddress, 50, SCREEN_SIZE[1] - SCREEN_DIFF[1] * 250, NULL);
        font->render(ren, "PORT: " + port, 50, SCREEN_SIZE[1] - SCREEN_DIFF[1] * 200, NULL);
    }

    int host_menu(SDL_Renderer *ren, string *playerName, Font *font, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        vector<string> options = {"NAME: ", "CREATE HOST", "BACK"};
        vector<string> optionsAddOns = {*playerName, "", ""};

        int fTime = 1000 / 60, fDelta = 0;
        long int fStart, fEnd;

        bool run = true;
        SDL_Event e;
        int currentOption = 0;
        bool shiftPressed = false;

        const auto modkeys = KMOD_CAPS | KMOD_RSHIFT | KMOD_LSHIFT;

        while(run) {
            fStart = SDL_GetTicks();
            if (SDL_GetModState()&modkeys) {
                shiftPressed = true;
            }
            else {
                shiftPressed = false;
            }
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                        return -1;
                    }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP) {
                        currentOption--;
                        if (currentOption == -1) {
                            currentOption = 2;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_DOWN) {
                        currentOption++;
                        if (currentOption == 3) {
                            currentOption = 0;
                        }
                    }
                    if (currentOption == 0) {
                        char symbol = getSymbol(e, shiftPressed);
                        if (symbol != '^' && symbolsAllowedForName.find(symbol) < strlen(symbolsAllowedForName.c_str())) {
                            *playerName = *playerName + symbol;
                        }
                        if (e.key.keysym.sym == SDLK_BACKSPACE) {
                            if (playerName->size() > 0) {playerName->resize(playerName->size() - 1);}
                        }
                    }
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        if (currentOption == 1 && playerName->size() > 0 && playerName->size() <= 12) {
                            return 1;
                        }
                        if (currentOption == 2) {
                            return 0;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        return 0;
                    }
                }
            }

            optionsAddOns[0] = *playerName;

            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);

            draw_options(ren, options, optionsAddOns, currentOption, font, SCREEN_SIZE, SCREEN_DIFF);

            SDL_RenderPresent(ren);

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }

        }

        return 0;
    }

    int client_menu(SDL_Renderer *ren, string *ADDRESS, string *PORT, string *NAME, Font *optionFont, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        bool run = true;
        SDL_Event e;
        int currentOption = 0;
        vector<string> options = {"ADDRESS: ", "PORT: ", "NAME: ", "CONNECT", "BACK"};
        vector<string> optionsAddOns = {*ADDRESS, *PORT, *NAME, "", ""};

        int fTime = 1000 / 60, fDelta = 0;
        long int fStart, fEnd;

        bool shiftPressed = false;
        const auto modkeys = KMOD_CAPS | KMOD_RSHIFT | KMOD_LSHIFT;

        
        while (run) {
            fStart = SDL_GetTicks();
            if (SDL_GetModState()&modkeys) {
                shiftPressed = true;
            }
            else {
                shiftPressed = false;
            }

            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    return -1;
                }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP) {
                        currentOption--;
                        if (currentOption == -1) {
                            currentOption = 4  ;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_DOWN) {
                        currentOption++;
                        if (currentOption == 5) {
                            currentOption = 0;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        if (currentOption == 3 && strlen(NAME->c_str()) <= 12) {
                            return 1;
                        }
                        if (currentOption == 4) {
                            return 2;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        return 2;
                    }
                    if (currentOption == 0) {
                        char symbol = getSymbol(e, shiftPressed);
                        if (symbol != '^' && symbolsAllowedForAddress.find(symbol) < strlen(symbolsAllowedForAddress.c_str())) {
                            *ADDRESS = *ADDRESS + symbol;
                        }
                        if (e.key.keysym.sym == SDLK_BACKSPACE) {
                            if (ADDRESS->size() > 0) {ADDRESS->resize(ADDRESS->size() - 1);}
                        }
                        optionsAddOns[0] = *ADDRESS;
                    }
                    if (currentOption == 1) {
                        char symbol = getSymbol(e, shiftPressed);
                        if (symbol != '^' && symbolsAllowedForPort.find(symbol) < strlen(symbolsAllowedForPort.c_str())) {
                            *PORT = *PORT + symbol;
                        }
                        if (e.key.keysym.sym == SDLK_BACKSPACE) {
                            if (PORT->size() > 0) {PORT->resize(PORT->size() - 1);}
                        }
                        optionsAddOns[1] = *PORT;
                    }
                    if (currentOption == 2) {
                        char symbol = getSymbol(e, shiftPressed);
                        if (symbol != '^' && symbolsAllowedForName.find(symbol) < strlen(symbolsAllowedForName.c_str())) {
                            *NAME = *NAME + symbol;
                        }
                        if (e.key.keysym.sym == SDLK_BACKSPACE) {
                            if (NAME->size() > 0) {NAME->resize(NAME->size() - 1);}
                        }
                        optionsAddOns[2] = *NAME;
                    }
                }
            }

            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);
            draw_options(ren, options, optionsAddOns, currentOption, optionFont, SCREEN_SIZE, SCREEN_DIFF);

            SDL_RenderPresent(ren);

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }

        }

        return 0;
    }

    int main_menu(SDL_Renderer *ren, string *ADDRESS, string *PORT, string *NAME, Font *optionFont, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        bool run = true;
        SDL_Event e;
        int currentOption = 0;
        const vector<string> options = {"CREATE HOST", "CONNECT", "EXIT"};
        const vector<string> optionsAddOns = {"", "", ""};

        int fTime = 1000 / 60, fDelta = 0;
        long int fStart, fEnd;

        while (run) {
            fStart = SDL_GetTicks();

            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    return -1;
                }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP) {
                        currentOption--;
                        if (currentOption == -1) {
                            currentOption = 2;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_DOWN) {
                        currentOption++;
                        if (currentOption == 3) {
                            currentOption = 0;
                        }
                    }
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        if (currentOption == 0) {
                            int hMenuResult = host_menu(ren, NAME, optionFont, SCREEN_SIZE, SCREEN_DIFF);
                            if (hMenuResult == 1) {
                                return 1;
                            }
                            if (hMenuResult == -1) {
                                return -1;
                            }
                        }
                        if (currentOption == 1) {
                            int cMenuResult = client_menu(ren, ADDRESS, PORT, NAME, optionFont, SCREEN_SIZE, SCREEN_DIFF);
                            if (cMenuResult == -1) {
                                return -1;
                            }
                            if (cMenuResult == 1) {
                                return 2;
                            }
                        }
                        if (currentOption == 2) {
                            return -1;
                        }
                    }
                }
            }

            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);
            draw_options(ren, options, optionsAddOns, currentOption, optionFont, SCREEN_SIZE, SCREEN_DIFF);

            SDL_RenderPresent(ren);

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }
        }
        return 0;
    }

    void drawPause(SDL_Renderer *ren, int pauseIptionNum, Font *font, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        vector<string> options {"CONTINUE", "EXIT"};
        vector<string> optionsAddOns = {"", ""};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 200);
        SDL_Rect blackRect = {0, 0, SCREEN_SIZE[0], SCREEN_SIZE[1]};
        SDL_RenderFillRect(ren, &blackRect);
        draw_options(ren, options, optionsAddOns, pauseIptionNum, font, SCREEN_SIZE, SCREEN_DIFF);
    }

    void drawTitle(SDL_Renderer *ren, Font *titleFont, int SCREEN_SIZE[2], float SCREEN_DIFF[2]) {
        SDL_Event e;
        int sizeChange = 50;
        bool run = true;
        int textSize[2];
        titleFont->getSize("LLEMOON", &textSize[0], &textSize[1]);
        int fTime = 1000 / 60, fDelta = 0;
        long int fStart, fEnd;
        int frameCount = 0;
        int alpha = 255;
        SDL_Texture *titleTextue = NULL;


        SDL_Rect titleRect = {
                SCREEN_SIZE[0]/2 - textSize[0]/2,
                SCREEN_SIZE[1]/2 - textSize[1]/2,
                textSize[0],
                textSize[1],
            };
        while(run) {
            fStart = SDL_GetTicks();
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_KEYDOWN or e.type == SDL_MOUSEBUTTONDOWN) {
                    run = false;
                }
            }

            titleTextue = titleFont->getTextureForTitle(ren, "LLEMOON");
            if (frameCount >= 240) {
                alpha -= 5;
                SDL_SetTextureAlphaMod(titleTextue, alpha);
            }

            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, titleTextue, NULL, &titleRect);

            SDL_RenderPresent(ren);

            frameCount++;
            if (alpha <= 0) {
                run = false;
            }

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }
        }

    }

}

#endif