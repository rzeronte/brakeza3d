//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <SDL2/SDL_ttf.h>
#include <string>
#include "Color.h"

class TextWriter {
    SDL_Renderer *renderer;
    TTF_Font *font;
    float alpha;
public:
    TextWriter(SDL_Renderer *renderer, TTF_Font *font);

    void WriteTextTTFAutoSize(int x, int y, const char *text, const Color &c, float sizeRatio) const;
    void WriteTextTTF(int x, int y, int w, int h, const char *text, const Color &c) const;
    void WriteTextTTFMiddleScreen(const char *text, const Color &c, float sizeRatio) const;
    void WriteTTFCenterHorizontal(int y, const char *text, const Color &c, float sizeRatio) const;
    void setAlpha(float alpha);
    void setFont(TTF_Font *font);
    [[nodiscard]] float getAlpha() const;
};


#endif //BRAKEDA3D_TEXTWRITER_H
