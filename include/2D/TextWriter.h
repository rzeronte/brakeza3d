//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <vector>
#include <SDL2/SDL_ttf.h>
#include <SDL_opengl.h>
#include "../Misc/Color.h"


class TextWriter {
private:
    SDL_Renderer *renderer;

    TTF_Font *font;
    float alpha;
public:
    TextWriter(SDL_Renderer *renderer, TTF_Font *font);

    virtual ~TextWriter();

    void writeTextTTFAutoSize(int x, int y, const char *text, Color c, float sizeRatio);

    void writeTextTTF(int x, int y, int w, int h, const char *text, Color c);

    void writeTextTTFMiddleScreen(const char *text, Color c, float sizeRatio);

    void writeTTFCenterHorizontal(int y, const char *text, Color c, float sizeRatio);

    [[nodiscard]] float getAlpha() const;

    void setAlpha(float alpha);

    void setFont(TTF_Font *font);

    TTF_Font *getFont() const;
};


#endif //BRAKEDA3D_TEXTWRITER_H
