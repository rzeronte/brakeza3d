//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <vector>
#include <SDL_ttf.h>
#include "../Misc/Color.h"

#define CONCHARS_WIDTH 256
#define CONCHARS_HEIGHT 256

#define CONCHARS_CHARACTER_W 16
#define CONCHARS_CHARACTER_H 16

class TextWriter {
private:
    SDL_Renderer *renderer;
    SDL_Surface *sprite;
    std::vector<SDL_Surface *> characterSurfaces;

    TTF_Font *font;
    float alpha;
public:
    TextWriter(SDL_Renderer *renderer, TTF_Font *font, const char *concharsFile);

    virtual ~TextWriter();

    int convertPositionYAspect(int value);

    void writeText(int x, int y, const char *text, bool bold);

    void putCharacter(int ascii, int x, int y);

    void writeCenterHorizontal(int y, const char *text, bool bold);

    void writeTextMiddleScreen(const char *text, bool bold);

    void writeTextTTFAutoSize(int x, int y, const char *text, Color c, float sizeRatio);

    void writeTextTTF(int x, int y, int w, int h, const char *text, Color c);

    void writeTextTTFMiddleScreen(const char *text, Color c, float sizeRatio);

    void writeTTFCenterHorizontal(int y, const char *text, Color c, float sizeRatio);

    int convertPositionXAspect(int value);
    [[nodiscard]] float getAlpha() const;

    void setAlpha(float alpha);

};


#endif //BRAKEDA3D_TEXTWRITER_H
