//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <vector>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../Misc/Color.h"

class TextWriter {
private:
    SDL_Renderer *renderer;
    TTF_Font *font;
    float alpha;
public:
    TextWriter(SDL_Renderer *renderer, TTF_Font *font);

    static TextWriter* create(const std::string& fontFile);

    virtual ~TextWriter();

    void writeTextTTFAutoSize(int x, int y, const char *text, const Color &c, float sizeRatio) const;

    void writeTextTTF(int x, int y, int w, int h, const char *text, const Color &c) const;

    void writeTextTTFMiddleScreen(const char *text, const Color &c, float sizeRatio) const;

    void writeTTFCenterHorizontal(int y, const char *text, const Color &c, float sizeRatio) const;

    [[nodiscard]] float getAlpha() const;

    void setAlpha(float alpha);

    void setFont(TTF_Font *font);
};


#endif //BRAKEDA3D_TEXTWRITER_H
