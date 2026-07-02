#define GL_GLEXT_PROTOTYPES

#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/Render/TextWriter.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"

TextWriter::TextWriter(SDL_Renderer *renderer, TTF_Font *font)
:
    renderer(renderer),
    font(font),
    alpha(1.0f)
{
}

void TextWriter::WriteTextTTF(int x, int y, int w, int h, const char *text, const Color &c)
{
    auto surfaceTTF = TTF_RenderUTF8_Blended(font, text, c.toSDL());
#ifdef _WIN32
    auto surfaceTTFGoodFormat = SDL_ConvertSurfaceFormat(surfaceTTF, SDL_PIXELFORMAT_BGRA32, 0);
    GLuint texID = Image::MakeOGLImage(surfaceTTFGoodFormat);
#else
    GLuint texID = Image::MakeOGLImage(surfaceTTF);
#endif

    int targetW, targetH;
    GLuint targetFBO;
    if (activeTextCache != nullptr) {
        targetW   = activeTextCache->w;
        targetH   = activeTextCache->h;
        targetFBO = activeTextCache->fbo;
    } else {
        auto *win = Components::get()->Window();
        const int rw = win->getWidthRender();
        const int rh = win->getHeightRender();
        const float rx = (float)rw / (float)win->getWidth();
        const float ry = (float)rh / (float)win->getHeight();
        targetW   = rw;
        targetH   = rh;
        targetFBO = win->getUIFramebuffer();
        x = (int)std::round(x * rx);
        y = (int)std::round(y * ry);
        w = (int)std::round(w * rx);
        h = (int)std::round(h * ry);
    }

    glViewport(0, 0, targetW, targetH);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        texID,
        x, y,
        w, h,
        targetW, targetH,
        alpha,
        false,
        targetFBO
    );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDeleteTextures(1, &texID);
    SDL_FreeSurface(surfaceTTF);
#ifdef _WIN32
    SDL_FreeSurface(surfaceTTFGoodFormat);
#endif
}

void TextWriter::beginTextCache(const std::string &name, int w, int h)
{
    auto &tc = textCaches[name];
    if (tc.fbo == 0 || tc.w != w || tc.h != h) {
        if (tc.fbo != 0) {
            glDeleteFramebuffers(1, &tc.fbo);
            glDeleteTextures(1, &tc.texture);
        }
        glGenTextures(1, &tc.texture);
        glBindTexture(GL_TEXTURE_2D, tc.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &tc.fbo);
        Components::get()->Render()->ChangeOpenGLFramebuffer(tc.fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tc.texture, 0);
        tc.w = w;
        tc.h = h;
    } else {
        Components::get()->Render()->ChangeOpenGLFramebuffer(tc.fbo);
    }

    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    activeTextCache = &tc;
}

void TextWriter::endTextCache()
{
    activeTextCache = nullptr;
    auto *win = Components::get()->Window();
    Components::get()->Render()->ChangeOpenGLFramebuffer(win->getUIFramebuffer());
    glViewport(0, 0, win->getWidthRender(), win->getHeightRender());
}

void TextWriter::drawTextCache(const std::string &name, int x, int y)
{
    auto it = textCaches.find(name);
    if (it == textCaches.end() || it->second.fbo == 0) return;
    const TextCache &tc = it->second;

    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();

    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        tc.texture,
        (int)(x * rx), (int)(y * ry),
        (int)(tc.w * rx), (int)(tc.h * ry),
        rw, rh,
        1.0f,
        true,   // FBO textures need Y-flip: ortho(0,W,H,0) stores content inverted vs SDL textures
        win->getUIFramebuffer()
    );
}

void TextWriter::WriteTextTTFAutoSize(int x, int y, const char *text, const Color &c, float sizeRatio)
{
    int w = 0, h = 0;
    TTF_SizeUTF8(font, text, &w, &h);

    w = (int)(w * sizeRatio);
    h = (int)(h * sizeRatio);

    WriteTextTTF(x, y, w, h, text, c);
}

void TextWriter::WriteTextTTFMiddleScreen(const char *text, const Color &c, float sizeRatio)
{
    int textWidth, textHeight;
    TTF_SizeUTF8(font, text, &textWidth, &textHeight);

    textWidth *= (int) sizeRatio;
    textHeight *= (int) sizeRatio;

    const int totalW = Config::get()->screenWidth;
    const int totalH = Config::get()->screenHeight;

    int xPosition = (totalW / 2) - textWidth / 2;
    int yPosition = totalH / 2;

    WriteTextTTFAutoSize(xPosition, yPosition, text, c, sizeRatio);
}

void TextWriter::WriteTTFCenterHorizontal(int y, const char *text, const Color &c, float sizeRatio)
{
    int w, h;
    TTF_SizeUTF8(font, text, &w, &h);

    w = (int)(w * sizeRatio);
    h = (int)(h * sizeRatio);

    int totalW = Config::get()->screenWidth;

    int xPosition = (totalW / 2) - w / 2;

    WriteTextTTFAutoSize(xPosition, y, text, c, sizeRatio);
}

float TextWriter::getAlpha() const {
    return alpha;
}

void TextWriter::setAlpha(float alpha) {
    TextWriter::alpha = alpha;
}

void TextWriter::setFont(TTF_Font *font) {
    TextWriter::font = font;
}

void TextWriter::buildGlyphAtlas(int atlasSize)
{
    if (glyphAtlas != nullptr) {
        delete glyphAtlas;
    }
    glyphAtlas = new GlyphAtlas();
    if (!glyphAtlas->build(font, atlasSize)) {
        Logging::Warning("[TextWriter] GlyphAtlas build failed");
    }
    glyphVertexCache.clear();
}

TextWriter::~TextWriter()
{
    destroyGlyphVBO();
}

void TextWriter::ensureGlyphVBO(int numVerts)
{
    if (glyphVAO == 0) {
        glGenVertexArrays(1, &glyphVAO);
        glGenBuffers(1, &glyphVBO);
    }
    int neededBytes = numVerts * 4 * (int)sizeof(float);
    if (neededBytes > glyphVBOAllocated) {
        glBindBuffer(GL_ARRAY_BUFFER, glyphVBO);
        glBufferData(GL_ARRAY_BUFFER, neededBytes, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glyphVBOAllocated = neededBytes;
    }
}

void TextWriter::destroyGlyphVBO()
{
    if (glyphVAO != 0) {
        glDeleteVertexArrays(1, &glyphVAO);
        glDeleteBuffers(1, &glyphVBO);
        glyphVAO = 0;
        glyphVBO = 0;
        glyphVBOAllocated = 0;
    }
}

void TextWriter::writeTextAtlas(int x, int y, const char *text, const Color &c, float scale)
{
    if (!glyphAtlas || !glyphAtlas->isBuilt()) return;

    int len = (int)strlen(text);
    if (len == 0) return;

    int numVerts = len * 6;
    int numFloats = numVerts * 4;
    std::vector<float> vertices(numFloats);

    float baseX = (float)x;
    float baseY = (float)y;
    float cursorX = 0;
    int idx = 0;

    for (int i = 0; i < len; i++) {
        char ch = text[i];

        if (ch == ' ') {
            cursorX += glyphAtlas->getGlyphInfo(' ').advance * scale;
            continue;
        }

        const GlyphInfo& g = glyphAtlas->getGlyphInfo(ch);
        if (!g.valid) {
            cursorX += glyphAtlas->getGlyphInfo(' ').advance * scale;
            continue;
        }

        float gx = baseX + cursorX + g.bearingX * scale;
        float gw = g.width * scale;
        float gh = (float)g.height * scale;
        float gy = baseY;

        float u0 = g.u;
        float v0 = g.vTop + g.vh;
        float u1 = g.u + g.uw;
        float v1 = g.vTop;

        vertices[idx++] = gx;      vertices[idx++] = gy + gh; vertices[idx++] = u0; vertices[idx++] = v0;
        vertices[idx++] = gx + gw; vertices[idx++] = gy;      vertices[idx++] = u1; vertices[idx++] = v1;
        vertices[idx++] = gx;      vertices[idx++] = gy;       vertices[idx++] = u0; vertices[idx++] = v1;

        vertices[idx++] = gx;      vertices[idx++] = gy + gh; vertices[idx++] = u0; vertices[idx++] = v0;
        vertices[idx++] = gx + gw; vertices[idx++] = gy + gh; vertices[idx++] = u1; vertices[idx++] = v0;
        vertices[idx++] = gx + gw; vertices[idx++] = gy;      vertices[idx++] = u1; vertices[idx++] = v1;

        cursorX += g.advance * scale;
    }

    if (idx == 0) return;

    auto *win = Components::get()->Window();
    int targetW, targetH;
    GLuint targetFBO;

    if (activeTextCache != nullptr) {
        targetW   = activeTextCache->w;
        targetH   = activeTextCache->h;
        targetFBO = activeTextCache->fbo;
    } else {
        const int rw = win->getWidthRender();
        const int rh = win->getHeightRender();
        const float rx = (float)rw / (float)win->getWidth();
        const float ry = (float)rh / (float)win->getHeight();
        targetW   = rw;
        targetH   = rh;
        targetFBO = win->getUIFramebuffer();
        for (int j = 0; j < numFloats; j += 4) {
            vertices[j]     *= rx;
            vertices[j + 1] *= ry;
        }
    }

    auto* shader = Components::get()->Render()->getShaders()->shaderOGLImage;

    Components::get()->Render()->ChangeOpenGLFramebuffer(targetFBO);
    glDisable(GL_DEPTH_TEST);
    Components::get()->Render()->ChangeOpenGLProgram(shader->getProgramID());

    glm::mat4 projection = glm::ortho(0.0f, (float)targetW, (float)targetH, 0.0f, -1.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    shader->setMat4("projection", projection);
    shader->setMat4("model", model);
    shader->setFloat("alpha", alpha);
    shader->setInt("inverse", 0);
    shader->setTexture("image", glyphAtlas->getAtlasTexture(), 0);

    ensureGlyphVBO(numVerts);
    glBindBuffer(GL_ARRAY_BUFFER, glyphVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, idx * (int)sizeof(float), vertices.data());

    glBindVertexArray(glyphVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * (int)sizeof(float), nullptr);

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    glDrawArrays(GL_TRIANGLES, 0, idx / 4);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void TextWriter::writeTextAtlasCache(int x, int y, const char *text, const Color &c, float scale)
{
    if (!glyphAtlas || !glyphAtlas->isBuilt()) return;
    if (!text || text[0] == '\0') return;

    batchQueue.push_back({x, y, text, c, scale});
}

int TextWriter::measureTextWidthAtlas(const char *text, float scale)
{
    if (!glyphAtlas || !glyphAtlas->isBuilt()) return 0;

    int len = (int)strlen(text);
    float cursorX = 0;
    float maxRight = 0;

    for (int i = 0; i < len; i++) {
        char ch = text[i];

        if (ch == ' ') {
            const GlyphInfo& space = glyphAtlas->getGlyphInfo(' ');
            cursorX += space.advance * scale;
            float right = cursorX;
            if (right > maxRight) maxRight = right;
            continue;
        }

        const GlyphInfo& g = glyphAtlas->getGlyphInfo(ch);
        if (!g.valid) {
            cursorX += glyphAtlas->getGlyphInfo(' ').advance * scale;
            continue;
        }

        float right = cursorX + (g.bearingX + g.width) * scale;
        if (right > maxRight) maxRight = right;

        cursorX += g.advance * scale;
    }

    return (int)maxRight;
}

void TextWriter::writeTextAtlasMiddleScreen(const char *text, const Color &c, float scale)
{
    if (!glyphAtlas || !glyphAtlas->isBuilt()) return;

    int textWidth = measureTextWidthAtlas(text, scale);

    auto *win = Components::get()->Window();
    const int totalW = win->getWidth();
    const int totalH = win->getHeight();

    int xPosition = (totalW / 2) - textWidth / 2;
    int yPosition = totalH / 2;

    writeTextAtlas(xPosition, yPosition, text, c, scale);
}

void TextWriter::writeTextAtlasCenterHorizontal(int y, const char *text, const Color &c, float scale)
{
    if (!glyphAtlas || !glyphAtlas->isBuilt()) return;

    int textWidth = measureTextWidthAtlas(text, scale);

    auto *win = Components::get()->Window();
    const int totalW = win->getWidth();

    int xPosition = (totalW / 2) - textWidth / 2;

    writeTextAtlas(xPosition, y, text, c, scale);
}

const std::vector<float>& TextWriter::getOrBuildGlyphVerts(const char *text, float scale)
{
    std::string key = std::string(text) + "@" + std::to_string(scale);
    auto it = glyphVertexCache.find(key);
    if (it != glyphVertexCache.end()) return it->second;

    int len = (int)strlen(text);
    std::vector<float>& verts = glyphVertexCache[key];
    verts.reserve(len * 6 * 4);

    float cursorX = 0;
    for (int i = 0; i < len; i++) {
        char ch = text[i];
        if (ch == ' ') { cursorX += glyphAtlas->getGlyphInfo(' ').advance * scale; continue; }
        const GlyphInfo& g = glyphAtlas->getGlyphInfo(ch);
        if (!g.valid) { cursorX += glyphAtlas->getGlyphInfo(' ').advance * scale; continue; }

        float gx = cursorX + g.bearingX * scale;
        float gw = g.width  * scale;
        float gh = g.height * scale;
        float u0 = g.u, v0 = g.vTop + g.vh, u1 = g.u + g.uw, v1 = g.vTop;

        verts.insert(verts.end(), {gx,    gh,  u0, v0,
                                   gx+gw, 0,   u1, v1,
                                   gx,    0,   u0, v1,
                                   gx,    gh,  u0, v0,
                                   gx+gw, gh,  u1, v0,
                                   gx+gw, 0,   u1, v1});
        cursorX += g.advance * scale;
    }
    return verts;
}

// Uploads cached glyph verts (offset to x*rx, y*ry) and issues one draw call.
// Requires GL state (FBO, program, uniforms) already set up by the caller.
void TextWriter::drawTextAtlasImmediate(int x, int y, const char *text, float scale, float rx, float ry)
{
    const std::vector<float>& cached = getOrBuildGlyphVerts(text, scale);
    if (cached.empty()) return;

    int numVerts = (int)cached.size() / 4;
    std::vector<float> uploaded(cached.size());

    float baseX = (float)x * rx;
    float baseY = (float)y * ry;

    for (int i = 0; i < (int)cached.size(); i += 4) {
        uploaded[i]   = cached[i]   * rx + baseX;  // x
        uploaded[i+1] = cached[i+1] * ry + baseY;  // y
        uploaded[i+2] = cached[i+2];               // u
        uploaded[i+3] = cached[i+3];               // v
    }

    ensureGlyphVBO(numVerts);
    glBindBuffer(GL_ARRAY_BUFFER, glyphVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (int)uploaded.size() * (int)sizeof(float), uploaded.data());
    glBindVertexArray(glyphVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * (int)sizeof(float), nullptr);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TextWriter::flushTextBatch()
{
    if (batchQueue.empty() || !glyphAtlas || !glyphAtlas->isBuilt()) return;

    auto* win    = Components::get()->Window();
    auto* render = Components::get()->Render();
    auto* shader = render->getShaders()->shaderOGLImage;

    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const GLuint uiFBO = win->getUIFramebuffer();

    render->ChangeOpenGLFramebuffer(uiFBO);
    glDisable(GL_DEPTH_TEST);
    render->ChangeOpenGLProgram(shader->getProgramID());

    glm::mat4 projection = glm::ortho(0.0f, (float)rw, (float)rh, 0.0f, -1.0f, 1.0f);
    shader->setMat4("projection", projection);
    shader->setMat4("model", glm::mat4(1.0f));
    shader->setFloat("alpha", alpha);
    shader->setInt("inverse", 0);
    shader->setTexture("image", glyphAtlas->getAtlasTexture(), 0);

    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    for (const auto& bt : batchQueue) {
        shader->setVec4("tintColor", glm::vec4(bt.color.r, bt.color.g, bt.color.b, bt.color.a));
        drawTextAtlasImmediate(bt.x, bt.y, bt.text.c_str(), bt.scale, rx, ry);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    batchQueue.clear();
}
