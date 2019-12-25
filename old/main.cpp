#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "cube.h"
#include "bullet.h"
#include <vector>

using namespace std;

SDL_Window *window = NULL;
SDL_Renderer *ren = NULL;

void init() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    window = SDL_CreateWindow("CUBES", 0, 0, 640, 480, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void quit() {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);

    ren = NULL;
    window = NULL;

    SDL_Quit();
}

void get_buttons(int *xbutton, int *ybutton) {
    
}

int main() {
    init();
    int frame_time = 1000 / 60, frame_delta = 0;
    Player player(50, 50, 0, 0);
    SDL_Event e;
    bool run = true;
    short int xbutton, ybutton;
    bool rpressed = false, lpressed = false, upressed = false, dpressed = false, lmousepressed = false;

    long int frame_start, frame_end;

    vector<Bullet> bullets;

    int mouse_pos[2] = {0, 0};
    while (run) {
        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_w) {
                    upressed = true;
                }
                if (e.key.keysym.sym == SDLK_s) {
                    dpressed = true;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    rpressed = true;
                } 
                if (e.key.keysym.sym == SDLK_a) {
                    lpressed = true;
                }
            }
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_w) {
                    upressed = false;
                }
                if (e.key.keysym.sym == SDLK_s) {
                    dpressed = false;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    rpressed = false;
                } 
                if (e.key.keysym.sym == SDLK_a) {
                    lpressed = false;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    lmousepressed = true;
                }
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    lmousepressed = false;
                }
            }

        }
        if (lmousepressed) {
            SDL_GetMouseState(&mouse_pos[0], &mouse_pos[1]);
            player.shoot(&bullets, mouse_pos);
        }
        if (lpressed) {
            xbutton = 1;
        }
        if (rpressed) {
            xbutton = 2;
        }
        if ((rpressed && lpressed) || (!rpressed && !lpressed)) {
            xbutton = 0;
        }
        if (upressed) {
            ybutton = 1;
        }
        if (dpressed) {
            ybutton = 2;
        }
        if ((upressed && dpressed) || (!upressed && !dpressed)) {
            ybutton = 0;
        }

        SDL_SetRenderDrawColor(ren, 15, 15, 15, 255);
        SDL_RenderClear(ren);

        player.update(xbutton, ybutton);
        player.draw(ren);
        for (int i = 0; i < bullets.size(); i++) {
            if (!bullets[i].update()) {
                bullets.erase(bullets.begin() + i);
            }
            else {
                bullets[i].draw(ren);
            }
        }

        SDL_RenderPresent(ren);

        frame_end = SDL_GetTicks();
        frame_delta = (frame_end - frame_start);
        if (frame_delta < frame_time) {
            SDL_Delay(frame_time - frame_delta);
        }

    }   

    quit();
    return 0;
}