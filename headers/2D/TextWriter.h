//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <vector>

#define CONCHARS_WIDTH 128
#define CONCHARS_HEIGHT 128

#define CONCHARS_CHARACTER_W 8
#define CONCHARS_CHARACTER_H 8

class TextWriter {

public:
    TextWriter(SDL_Renderer *renderer, const char *concharsFile);

    void writeText(int x, int y, const char *text, bool bold);

    void putCharacter(int ascii, int x, int y);

private:
    SDL_Renderer *renderer;
    SDL_Surface *sprite;
    std::vector<SDL_Surface *> characterSurfaces;


};


#endif //BRAKEDA3D_TEXTWRITER_H
