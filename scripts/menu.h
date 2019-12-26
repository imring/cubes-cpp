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
 
    void draw_player_in_menu(SDL_Renderer *ren, vector<Player> &players, TTF_Font *nameFont) {
        short int step = 600 / players.size();
        for (int player = 0; player < players.size(); player++) {
            players[player].update_colors();
            players[player].draw_in_menu(ren, nameFont, 20 + (player) * step + step / 2 - 20, 80);
        }
    }

    void draw_player_in_menu_client(SDL_Renderer *ren, vector<Player> &players, TTF_Font *nameFont) {
        short int step = 600 / players.size();
        for (int player = 0; player < players.size(); player++) {
            players[player].draw_in_menu(ren, nameFont, 20 + (player) * step + step / 2 - 20, 80);
        }
    }

    void draw_options(SDL_Renderer *ren, vector<string> options, vector<string> optionsAddOns, int currentOption, Font *font) {
        for (int option = 0; option < options.size(); option++) {
            if (option != currentOption) {
                SDL_Color textColor = {120, 120, 120};
                font->render(ren, options[option] + optionsAddOns[option], 50, 470 - options.size()*50 + option*50, &textColor);
            }
            else {
                font->render(ren, options[option] + optionsAddOns[option], 50, 470 - options.size()*50 + option*50, NULL);
            }
        }
    }

    void draw_ip_and_port(SDL_Renderer *ren, string ipAddress, string port, Font *font) {
        font->render(ren, "ADDRESS: " + ipAddress, 50, 230, NULL);
        font->render(ren, "PORT: " + port, 50, 280, NULL);
    }

    int host_menu(SDL_Renderer *ren, string *playerName, Font *font) {
        vector<string> options = {"NAME: ", "CREATE HOST", "BACK"};
        vector<string> optionsAddOns = {*playerName, "", ""};

        int fTime = 1000 / 60, fDelta = 0;
        long int fStart, fEnd;

        bool run = true;
        SDL_Event e;
        int currentOption = 0;
        bool shiftPressed = false;

        while(run) {
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
                    if (e.key.keysym.sym == SDLK_LSHIFT) {
                        shiftPressed = true;
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
                if (e.type == SDL_KEYUP) {
                    if (e.key.keysym.sym == SDLK_LSHIFT) {
                        shiftPressed = false;
                    }
                }
            }

            optionsAddOns[0] = *playerName;

            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);

            draw_options(ren, options, optionsAddOns, currentOption, font);

            SDL_RenderPresent(ren);

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }

        }

        return 0;
    }

    int client_menu(SDL_Renderer *ren, string *ADDRESS, string *PORT, string *NAME, Font *optionFont) {
        bool run = true;
        SDL_Event e;
        int currentOption = 0;
        vector<string> options = {"ADDRESS: ", "PORT: ", "NAME: ", "CONNECT", "BACK"};
        vector<string> optionsAddOns = {*ADDRESS, *PORT, *NAME, "", ""};

        int fTime = 1000 / 60, fDelta = 0;
        long int fStart, fEnd;

        bool shiftPressed = false;
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
                    if (e.key.keysym.sym == SDLK_LSHIFT) {
                        shiftPressed = true;
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
                if (e.type == SDL_KEYUP) {
                    if (e.key.keysym.sym == SDLK_LSHIFT) {
                        shiftPressed = false;
                    }
                }
            }

            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);
            draw_options(ren, options, optionsAddOns, currentOption, optionFont);

            SDL_RenderPresent(ren);

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }

        }

        return 0;
    }

    int main_menu(SDL_Renderer *ren, string *ADDRESS, string *PORT, string *NAME, Font *optionFont) {
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
                            int hMenuResult = host_menu(ren, NAME, optionFont);
                            if (hMenuResult == 1) {
                                return 1;
                            }
                            if (hMenuResult == -1) {
                                return -1;
                            }
                        }
                        if (currentOption == 1) {
                            int cMenuResult = client_menu(ren, ADDRESS, PORT, NAME, optionFont);
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
            draw_options(ren, options, optionsAddOns, currentOption, optionFont);

            SDL_RenderPresent(ren);

            fEnd = SDL_GetTicks();
            fDelta = (fEnd - fStart);
            if (fDelta < fTime) {
                SDL_Delay(fTime - fDelta);
            }
        }
        return 0;
    }


}

#endif