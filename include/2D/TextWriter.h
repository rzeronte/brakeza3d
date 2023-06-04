//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <vector>
#include <SDL_ttf.h>
#include "../Misc/Color.h"


class TextWriter {
private:
    SDL_Renderer *renderer;

    TTF_Font *font;
    float alpha;
public:
    TextWriter(SDL_Renderer *renderer, TTF_Font *font);

    virtual ~TextWriter();

    int convertPositionYAspect(int value);

    void writeTextTTFAutoSize(int x, int y, const char *text, Color c, float sizeRatio);

    void writeTextTTF(int x, int y, int w, int h, const char *text, Color c);

    void writeTextTTFMiddleScreen(const char *text, Color c, float sizeRatio);

    void writeTTFCenterHorizontal(int y, const char *text, Color c, float sizeRatio);

    int convertPositionXAspect(int value);

    [[nodiscard]] float getAlpha() const;

    void setAlpha(float alpha);
};


#endif //BRAKEDA3D_TEXTWRITER_H
