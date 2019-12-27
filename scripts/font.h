#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>
#include <cmath>
#include <iostream>
#include "bullet.h"


class Font {
    private:
        TTF_Font *font;
        int color[3];
        int nextColor[3];


    public:
        Font(std::string fileName, int fontSize) {
            this->font = TTF_OpenFont(fileName.c_str(), fontSize);
            if (this->font == NULL) {
                std::cout << "Font error: " << TTF_GetError() << std::endl;
            }

            int colorIndex = rand()%9;
            this->color[0] = COLORS[colorIndex][0];
            this->color[1] = COLORS[colorIndex][1];
            this->color[2] = COLORS[colorIndex][2];

            colorIndex = rand()%9;
            this->nextColor[0] = COLORS[colorIndex][0];
            this->nextColor[1] = COLORS[colorIndex][1];
            this->nextColor[2] = COLORS[colorIndex][2];
        }

        void update_colors() {
            if (abs(this->nextColor[0] - this->color[0]) < 5 && abs(this->nextColor[1] - this->color[1]) < 5 && \
            abs(this->nextColor[2] - this->color[2]) < 5){
                int color_index = rand()%9;
                this->nextColor[0] = COLORS[color_index][0];
                this->nextColor[1] = COLORS[color_index][1];
                this->nextColor[2] = COLORS[color_index][2];
            }

            for (int i = 0; i < 3; i++) {
                if (this->nextColor[i] < this->color[i]) {
                    this->color[i] -= 4;
                    if (this->color[i] < 0) {
                        this->color[i] = 0;
                    }
                }
                else if (this->nextColor[i] > this->color[i]) {
                    this->color[i] += 4;
                    if (this->color[i] > 255) {
                        this->color[i] = 255;
                    }
                }
            }   
        }

        void render(SDL_Renderer *ren, std::string text, int x, int y, SDL_Color *argTextColor = NULL) {
            SDL_Color textColor;
            if (argTextColor != NULL) {
                textColor = *argTextColor;
            }
            else {
		        this->update_colors();
		        textColor = {(Uint8)this->color[0], (Uint8)this->color[1], (Uint8)this->color[2]};
            }
            SDL_Surface *tempSurf = TTF_RenderText_Blended(this->font, text.c_str(), textColor);
            if (tempSurf == NULL) {
                std::cout << "Can't create surface from font: " << TTF_GetError() << std::endl;
            }
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, tempSurf);
            if (textTexture == NULL) {
                std::cout << "Can't create texture from surface: " << SDL_GetError() << std::endl;
            }
            SDL_Rect textRect = {x, y, tempSurf->w, tempSurf->h};
            SDL_RenderCopy(ren, textTexture, NULL, &textRect);

            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(tempSurf);

            textTexture = NULL;
            tempSurf = NULL;
        }

        void getSize(string text, int *width, int *height) {
            SDL_Color textColor = {0, 0, 0};
            SDL_Surface *tempSurf = TTF_RenderText_Blended(this->font, text.c_str(), textColor);
            *width = tempSurf->w;
            *height = tempSurf->h;
            SDL_FreeSurface(tempSurf);
            tempSurf = NULL;
        }
};

#endif
