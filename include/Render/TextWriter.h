//
// Created by darkhead on 5/2/21.
//

#ifndef BRAKEDA3D_TEXTWRITER_H
#define BRAKEDA3D_TEXTWRITER_H

#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>
#include <unordered_map>
#include "Color.h"
#include "GlyphAtlas.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>

class TextWriter {
    SDL_Renderer *renderer;
    TTF_Font *font;
    float alpha;

    struct TextCache {
        GLuint fbo     = 0;
        GLuint texture = 0;
        int    w       = 0;
        int    h       = 0;
    };
    std::map<std::string, TextCache> textCaches;
    TextCache* activeTextCache = nullptr;

    GlyphAtlas* glyphAtlas = nullptr;
    GLuint glyphVAO = 0;
    GLuint glyphVBO = 0;
    int glyphVBOAllocated = 0;

    void ensureGlyphVBO(int numVerts);
    void destroyGlyphVBO();

public:
    TextWriter(SDL_Renderer *renderer, TTF_Font *font);
    ~TextWriter();

    void WriteTextTTFAutoSize(int x, int y, const char *text, const Color &c, float sizeRatio);
    void WriteTextTTF(int x, int y, int w, int h, const char *text, const Color &c);
    void WriteTextTTFMiddleScreen(const char *text, const Color &c, float sizeRatio);
    void WriteTTFCenterHorizontal(int y, const char *text, const Color &c, float sizeRatio);
    void setAlpha(float alpha);
    void setFont(TTF_Font *font);
    [[nodiscard]] float getAlpha() const;

    void beginTextCache(const std::string &name, int w, int h);
    void endTextCache();
    void drawTextCache(const std::string &name, int x, int y);

    void setGlyphAtlas(GlyphAtlas* ga) { glyphAtlas = ga; }
    [[nodiscard]] GlyphAtlas* getGlyphAtlas() const { return glyphAtlas; }
    void buildGlyphAtlas(int atlasSize = 512);
    void writeTextAtlas(int x, int y, const char *text, const Color &c, float scale);
    void writeTextAtlasCache(int x, int y, const char *text, const Color &c, float scale);
    void writeTextAtlasMiddleScreen(const char *text, const Color &c, float scale);
    void writeTextAtlasCenterHorizontal(int y, const char *text, const Color &c, float scale);

    void flushTextBatch();

private:
    int measureTextWidthAtlas(const char *text, float scale);
    struct BatchedText {
        int x, y;
        std::string text;
        Color color;
        float scale;
    };
    std::vector<BatchedText> batchQueue;

    void drawTextAtlasImmediate(int x, int y, const char *text, float scale, float rx, float ry);

    // Vertices at origin (0,0), no rx/ry applied — keyed by "text@scale"
    std::unordered_map<std::string, std::vector<float>> glyphVertexCache;
    const std::vector<float>& getOrBuildGlyphVerts(const char *text, float scale);
};


#endif //BRAKEDA3D_TEXTWRITER_H
