/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
    GlyphInfo glyphs[256];   // ASCII imprimible (32-126) + Latin-1 (160-255: á é í ó ú ñ ü ¿ ¡ ...)
    int lineHeight = 0;
    int ascent = 0;
    bool built = false;

public:
    GlyphAtlas();
    ~GlyphAtlas();

    bool build(TTF_Font* font, int atlasSize = 512);
    [[nodiscard]] bool isBuilt() const { return built; }
    [[nodiscard]] GLuint getAtlasTexture() const { return atlasTexture; }
    [[nodiscard]] const GlyphInfo& getGlyphInfo(unsigned int codepoint) const;
    [[nodiscard]] const GlyphInfo& getGlyphInfo(char c) const { return getGlyphInfo((unsigned int)(unsigned char)c); }
    [[nodiscard]] int getLineHeight() const { return lineHeight; }
    [[nodiscard]] int getAscent() const { return ascent; }
};

// Siguiente carácter de un texto UTF-8 (los textos llegan así desde Lua/JSON: "ó" = 2 bytes) y
// avanza p. Cubre hasta U+07FF (Latin-1 incluido); secuencias más largas o mal formadas -> '?'.
inline unsigned int GlyphNextCodepoint(const char*& p)
{
    const auto b0 = (unsigned char)*p++;
    if (b0 < 0x80) return b0;
    if ((b0 & 0xE0) == 0xC0 && (((unsigned char)*p) & 0xC0) == 0x80) {
        const auto b1 = (unsigned char)*p++;
        return ((b0 & 0x1Fu) << 6) | (b1 & 0x3Fu);
    }
    // Secuencia de 3-4 bytes (fuera del atlas) o byte suelto: saltar continuaciones
    while ((((unsigned char)*p) & 0xC0) == 0x80) p++;
    return '?';
}

#endif
