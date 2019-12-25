#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>
#include "cube.h"
#include <cmath>
#include <math.h>
#include "draw.h"

using namespace std;


int COLORS[9][3] = {
    {255, 255, 0},
    {51, 51, 255},
    {255, 0, 0},
    {0, 255, 0},
    {255, 0, 127},
    {255, 255, 255},
    {255, 128, 0},
    {0, 255, 255},
    {144, 238, 144}
};

class Bullet {
    public:
        int color[3] = {255, 255, 255};
        SDL_Rect rect;
        unsigned short ownerPort;
        Bullet(int x, int y, int xspeed, int yspeed, unsigned short ownerPort) {
            this->ownerPort = ownerPort;
            this->rect.x = x;
            this->rect.y = y;
            this->rect.w = this->size;
            this->rect.h = this->size;

            this->xspeed = xspeed;
            this->yspeed = yspeed;

            int color_index = rand()%9;
            this->next_color[0] = COLORS[color_index][0];
            this->next_color[1] = COLORS[color_index][1];
            this->next_color[2] = COLORS[color_index][2];

            color_index = rand()%9;
            this->color[0] = COLORS[color_index][0];
            this->color[1] = COLORS[color_index][1];
            this->color[2] = COLORS[color_index][2];
        }

        void draw(SDL_Renderer *ren) {
            SDL_SetRenderDrawColor(ren, this->color[0], this->color[1], this->color[2], 255);
            /*SDL_RenderFillRect(ren, &this->rect);*/
            SDL_RenderFillCircle(ren, this->rect.x + (this->rect.w / 2), this->rect.y + (this->rect.h / 2), this->rect.w / 2);
        }

        bool update() {
            if (this->is_died) { return false; }

            this->rect.x += this->xspeed;
            this->rect.y += this->yspeed;
            this->update_colors();
            if (this->rect.x > 640 || this->rect.x < -this->size || this->rect.y > 480 || this->rect.y < -10) {
                return false;
            }
            return true;
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

        void die() {
            this->is_died = true;
        }

    private:
    int xspeed, yspeed;
    int size = 10;
    int next_color[3] = {255, 255, 255};
    bool is_died = false;
};

#endif