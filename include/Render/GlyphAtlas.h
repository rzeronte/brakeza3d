#ifndef BRAKEDA3D_GLYPHATLAS_H
#define BRAKEDA3D_GLYPHATLAS_H

#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <SDL2/SDL_ttf.h>
#include "../Misc/TextureAtlas.h"

struct GlyphInfo {
    float u = 0, vTop = 0;
    float uw = 0, vh = 0;
    int advance = 0;
    int bearingX = 0, bearingY = 0;
    int minY = 0;       // miny from TTF_GlyphMetrics (negative = descender below baseline)
    int width = 0, height = 0;
    bool valid = false;
};

class GlyphAtlas {
    TextureAtlas* atlas = nullptr;
    GLuint atlasTexture = 0;
    GlyphInfo glyphs[128];
    int lineHeight = 0;
    int ascent = 0;
    bool built = false;

public:
    GlyphAtlas();
    ~GlyphAtlas();

    bool build(TTF_Font* font, int atlasSize = 512);
    [[nodiscard]] bool isBuilt() const { return built; }
    [[nodiscard]] GLuint getAtlasTexture() const { return atlasTexture; }
    [[nodiscard]] const GlyphInfo& getGlyphInfo(char c) const;
    [[nodiscard]] int getLineHeight() const { return lineHeight; }
    [[nodiscard]] int getAscent() const { return ascent; }
};

#endif
