#ifndef CUBE_H
#define CUBE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <vector>
#include "bullet.h"
#include <SFML/Network.hpp>
#include <string>


using sf::IpAddress;
using namespace std;

class Player {
    public:
        int color[3] = {0, 0, 0};
        SDL_Rect rect;
        int port = 0;
        IpAddress address;
        int tryedToConn = 0;
        bool dead = false;
        string name;
        int triedToConn = 0;
        bool rPressed = false;
        bool enterPressed = false;


        Player(int x, int y, int port, IpAddress address, string name) {
            this->rect = {x, y, 40, 40};

            int color_index = rand()%9;
            this->next_color[0] = COLORS[color_index][0];
            this->next_color[1] = COLORS[color_index][1];
            this->next_color[2] = COLORS[color_index][2];

            color_index = rand()%9;
            this->color[0] = COLORS[color_index][0];
            this->color[1] = COLORS[color_index][1];
            this->color[2] = COLORS[color_index][2];
            this->port = port;
            this->address = address;
            this->name = name;
        }

        void togleReady() {
            if (this->rPressed) {this->rPressed = false;}
            else {this->rPressed = true;}
        }

        void update(int xbutton, int ybutton, vector<Bullet> *bullets) {
            if (this->shot_timer > 0) {
                this->shot_timer --;
            }
            update_colors();
            if (xbutton == 1) {
                xspeed -= (-this->weight * 9.8 * 0.02 + this->acceleration) / this->weight;
                if (this->xspeed < -this->max_speed) {
                    this->xspeed = -this->max_speed;
                }
            }
            else if (xbutton == 2) {
                xspeed += (-this->weight * 9.8 * 0.02 + this->acceleration) / this->weight;
                if (this->xspeed > this->max_speed) {
                    this->xspeed = this->max_speed;
                }
            }
            else if (xbutton == 0) {
                if (this->xspeed < 0) {
                    this->xspeed += (this->weight * 9.8 * 0.02) / this->weight;
                }
                if (this->xspeed > 0) {
                    this->xspeed -= (this->weight * 9.8 * 0.02) / this->weight;
                }
                if (abs(this->xspeed) < 0.5) {
                    this->xspeed = 0;
                }
            }
            if (ybutton == 1) {
                this->yspeed -= (-this->weight * 9.8 * 0.02 + this->acceleration) / this->weight;
                if (this->yspeed < -this->max_speed) {
                    this->yspeed = -this->max_speed;
                }
            }

            else if (ybutton == 2) {
                this->yspeed += (-this->weight * 9.8 * 0.02 + this->acceleration) / this->weight;
                if (this->yspeed > this->max_speed) {
                    this->yspeed = this->max_speed;
                }
            }
            else if (ybutton == 0) {
                if (this->yspeed < 0) {
                    this->yspeed += (this->weight * 9.8 * 0.02) / this->weight;
                }
                if (this->yspeed > 0) {
                    this->yspeed -= (this->weight * 9.8 * 0.02) / this->weight;
                }
                if (abs(this->yspeed) < 0.5) {
                    this->yspeed = 0;
                }
            }

            this->collide_wall();

            this->rect.x += round(this->xspeed);
            this->rect.y += round(this->yspeed);

            SDL_Rect tempRect;
            for (auto &bullet: *bullets) {
                if (SDL_IntersectRect(&this->rect, &bullet.rect, &tempRect) == SDL_TRUE && this->port != bullet.ownerPort) {
                    this->die();
                    bullet.die();
                }
            }
        }

        void update_colors() {
            if (abs(this->next_color[0] - this->color[0]) < 5 && abs(this->next_color[1] - this->color[1]) < 5 && \
            abs(this->next_color[2] - this->color[2]) < 5){
                int color_index = rand()%9;
                this->next_color[0] = COLORS[color_index][0];
                this->next_color[1] = COLORS[color_index][1];
                this->next_color[2] = COLORS[color_index][2];
            }

            for (int i = 0; i < 3; i++) {
                if (this->next_color[i] < this->color[i]) {
                    this->color[i] -= 4;
                    if (this->color[i] < 0) {
                        this->color[i] = 0;
                    }
                }
                else if (this->next_color[i] > this->color[i]) {
                    this->color[i] += 4;
                    if (this->color[i] > 255) {
                        this->color[i] = 255;
                    }
                }
            }   
        }

        void collide_wall() {
            if (this->rect.x + this->rect.w >= 640) {
                this->xspeed = -abs(this->xspeed);
            }
            if (this->rect.x <= 0) {
                this->xspeed = abs(this->xspeed);
            }

            if (this->rect.y + this->rect.h >= 480) {
                this->yspeed = -abs(this->yspeed);
            }
            if (this->rect.y <= 0) {
                this->yspeed = abs(this->yspeed);
            }
        }

        void shoot(vector<Bullet> *bullets, int mouse_pos[2]){
            if (this->shot_timer == 0) {
                float xdiff = this->rect.x + (this->rect.w / 2) - mouse_pos[0];
                float ydiff = this->rect.y + (this->rect.h / 2) - mouse_pos[1];
                int diff = round(sqrt(pow(xdiff, 2) + pow(ydiff, 2)) / 10);
                bullets->push_back(Bullet(this->rect.x + (this->rect.w / 2) - 10 / 2, this->rect.y + (this->rect.h / 2) - 10 / 2, \
                -round(xdiff / diff), -round(ydiff / diff), this->port));
                this->shot_timer = 30;
            }
        }

        void draw(SDL_Renderer *ren, TTF_Font *nameFont) {
            if (!this->dead) {
                SDL_SetRenderDrawColor(ren, this->color[0], this->color[1], this->color[2], 255);
                SDL_RenderFillRect(ren, &this->rect);
                this->draw_name(ren, nameFont, this->rect);
            }
        }

        void draw_in_menu(SDL_Renderer *ren, TTF_Font *nameFont, int x, int y) {
            SDL_SetRenderDrawColor(ren, this->color[0], this->color[1], this->color[2], 255);
            SDL_Rect rect = {x, y, 40, 40};
            SDL_RenderFillRect(ren, &rect);
            this->draw_name(ren, nameFont, rect);
            this->draw_ready_in_menu(ren, nameFont, rect);
        }

        void draw_name(SDL_Renderer *ren, TTF_Font *nameFont, SDL_Rect rect) {
            if (nameFont == NULL) {
                cout << SDL_GetError() << endl;
            }
            SDL_Color nameColor = {(Uint8)this->color[0], (Uint8)this->color[1], (Uint8)this->color[2]};

            SDL_Surface *tempSurface = TTF_RenderText_Blended(nameFont, this->name.c_str(), nameColor);
            if (tempSurface == NULL) {
                cout << "ERROR: " << SDL_GetError() << " " << TTF_GetError() << endl;
            }
            nameTexture = SDL_CreateTextureFromSurface(ren, tempSurface);
            if (nameTexture == NULL) {
                cout << "Error: " << SDL_GetError() << endl;
            }

            SDL_Rect nameRect;
            nameRect.x = rect.x + (rect.w / 2) - (tempSurface->w / 2);
            nameRect.y = rect.y - 20;
            nameRect.w = tempSurface->w; nameRect.h = tempSurface->h;
            SDL_RenderCopy(ren, nameTexture, NULL, &nameRect);
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }

        void draw_ready_in_menu(SDL_Renderer *ren, TTF_Font *nameFont, SDL_Rect rect) {
            if (nameFont == NULL) {
                cout << SDL_GetError() << endl;
            }
            SDL_Color nameColor = {(Uint8)this->color[0], (Uint8)this->color[1], (Uint8)this->color[2]};

            SDL_Surface *tempSurface;
            if (this->rPressed) {
                tempSurface = TTF_RenderText_Blended(nameFont, "READY", nameColor);
            }
            else {
                SDL_Color tempColor = {120, 120, 120};
                tempSurface = TTF_RenderText_Blended(nameFont, "NOT READY", tempColor);
            }
            
            if (tempSurface == NULL) {
                cout << "ERROR: " << SDL_GetError() << " " << TTF_GetError() << endl;
            }
            nameTexture = SDL_CreateTextureFromSurface(ren, tempSurface);
            if (nameTexture == NULL) {
                cout << "Error: " << SDL_GetError() << endl;
            }

            SDL_Rect nameRect;
            nameRect.x = rect.x + (rect.w / 2) - (tempSurface->w / 2);
            nameRect.y = rect.y + rect.h + 5;
            nameRect.w = tempSurface->w; nameRect.h = tempSurface->h;
            SDL_RenderCopy(ren, nameTexture, NULL, &nameRect);
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }

        void die() {
            this->dead = true;
            this->rect.x = -100;
            this->rect.y = -100;
        }
        
        bool is_dead() {
            return this->dead;
        }

    private:
        
        int next_color[3] = {0, 0, 0};
        int max_speed = 5;
        float xspeed = 0, yspeed = 0;
        int acceleration = 10;
        int weight = 10;
        int shot_timer = 0;

        SDL_Texture *nameTexture = NULL;
};

#endif
