#include "../../include/Render/GlyphAtlas.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"

GlyphAtlas::GlyphAtlas()
{
}

GlyphAtlas::~GlyphAtlas()
{
    if (atlasTexture != 0) {
        glDeleteTextures(1, &atlasTexture);
    }
    delete atlas;
}

bool GlyphAtlas::build(TTF_Font* font, int atlasSize)
{
    LOG_MESSAGE("[GlyphAtlas] Building glyph atlas (size: %dx%d)...", atlasSize, atlasSize);

    atlas = new TextureAtlas(atlasSize, atlasSize);

    lineHeight = TTF_FontHeight(font);
    ascent = TTF_FontAscent(font);
    LOG_MESSAGE("[GlyphAtlas] Font metrics: lineHeight=%d, ascent=%d", lineHeight, ascent);

    int packed = 0, failed = 0;
    for (int c = 32; c <= 126; c++) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* glyphSurf = TTF_RenderGlyph_Blended(font, (Uint16)c, white);
        if (!glyphSurf) {
            LOG_WARNING("[GlyphAtlas] TTF_RenderGlyph_Blended failed for char %d ('%c')", c, (char)c);
            failed++;
            continue;
        }

        SDL_Surface* formatSurf = glyphSurf;
#ifdef _WIN32
        formatSurf = SDL_ConvertSurfaceFormat(glyphSurf, SDL_PIXELFORMAT_BGRA32, 0);
#endif

        auto* renderer = Components::get()->Window()->getRenderer();
        SDL_Texture* sdlTex = SDL_CreateTextureFromSurface(renderer, formatSurf);
        if (!sdlTex) {
            LOG_WARNING("[GlyphAtlas] SDL_CreateTextureFromSurface failed for char %d ('%c')", c, (char)c);
            if (formatSurf != glyphSurf) SDL_FreeSurface(formatSurf);
            SDL_FreeSurface(glyphSurf);
            failed++;
            continue;
        }

        auto* img = new Image(formatSurf, sdlTex);
        std::string name = "glyph_" + std::to_string(c);

        if (atlas->AddToAtlas(img, name)) {
            TextureAtlasImageInfo info = atlas->getAtlasTextureInfoForName(name);

            GlyphInfo& gi = glyphs[c];
            gi.u = info.x / (float)atlasSize;
            gi.vTop = info.y / (float)atlasSize;
            gi.uw = info.width / (float)atlasSize;
            gi.vh = info.height / (float)atlasSize;
            gi.width = (int)info.width;
            gi.height = (int)info.height;

            int minx, maxx, miny, maxy, adv;
            TTF_GlyphMetrics(font, (Uint16)c, &minx, &maxx, &miny, &maxy, &adv);
            gi.bearingX = minx;
            gi.bearingY = maxy;
            gi.minY     = miny;
            gi.advance = adv;
            gi.valid = true;
            packed++;
        }

        if (formatSurf != glyphSurf) SDL_FreeSurface(formatSurf);
        SDL_FreeSurface(glyphSurf);
    }

    atlas->uploadAtlasToGL();
    atlasTexture = atlas->getAtlasGLTexture();

    built = true;
    LOG_MESSAGE("[GlyphAtlas] Done: %d glyphs packed, %d failed (GL texture ID: %u)", packed, failed, atlasTexture);
    return true;
}

const GlyphInfo& GlyphAtlas::getGlyphInfo(char c) const
{
    if (c < 32 || c > 126) {
        static GlyphInfo invalid;
        return invalid;
    }
    return glyphs[(int)c];
}
