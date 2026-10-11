#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOGLUIBatch.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Profiler.h"

namespace {
    constexpr int FLOATS_PER_VERTEX = 9;
}

ShaderOGLUIBatch::ShaderOGLUIBatch()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "UIBatch.vs",
        Config::get()->SHADERS_FOLDER + "UIBatch.fs",
        false
    )
{
}

void ShaderOGLUIBatch::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vboCapacityBytes = 64 * 1024;
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vboCapacityBytes, nullptr, GL_STREAM_DRAW);
    const GLsizei stride = FLOATS_PER_VERTEX * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Textura blanca 1x1 para los rectángulos lisos (color del vértice * 1 = color del rect)
    const unsigned char white[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &whiteTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    vertices.reserve(FLOATS_PER_VERTEX * 6 * 1024);
}

void ShaderOGLUIBatch::LoadUniforms()
{
    projectionUniform = glGetUniformLocation(programID, "projection");
    for (int i = 0; i < MAX_TEXTURES; i++) {
        const std::string name = "images[" + std::to_string(i) + "]";
        textureUniforms[i] = glGetUniformLocation(programID, name.c_str());
    }
}

void ShaderOGLUIBatch::Destroy()
{
    // Sin recursos ligados al tamaño de la ventana: el VAO/VBO y la textura blanca sobreviven a un resize
}

float ShaderOGLUIBatch::beginEntry(const Key& key)
{
    // RectAny va con la mezcla de la tanda abierta (o con la de imágenes si no hay ninguna)
    const bool rect = key.blend == Blend::Rect || (key.blend == Blend::RectAny && !vertices.empty() && rectBlend);
    if (!vertices.empty() && (key.fbo != fbo || key.rw != rw || key.rh != rh || rect != rectBlend)) {
        flush();
    }
    if (vertices.empty()) {
        fbo = key.fbo; rw = key.rw; rh = key.rh;
        rectBlend = rect;
    }

    int slot = -1;
    for (int i = 0; i < numTextures; i++) {
        if (textures[i] == key.texture) { slot = i; break; }
    }
    if (slot < 0) {
        if (numTextures == MAX_TEXTURES) {
            flush();
            fbo = key.fbo; rw = key.rw; rh = key.rh;
            rectBlend = rect;
        }
        slot = numTextures;
        textures[numTextures++] = key.texture;
    }
    // Estado GL de salida: un RectAny deja el de un rect (renderRect)
    lastBlend = key.blend == Blend::RectAny ? Blend::Rect : key.blend;
    return (float)slot;
}

void ShaderOGLUIBatch::appendTriangles(const Key& key, const float* xyuv, int numVerts,
                                       float ox, float oy, float sx, float sy, const Color& c)
{
    if (numVerts <= 0 || key.texture == 0) return;
    const float slot = beginEntry(key);

    const size_t base = vertices.size();
    vertices.resize(base + (size_t)numVerts * FLOATS_PER_VERTEX);
    float* out = vertices.data() + base;
    for (int i = 0; i < numVerts; i++) {
        const float* v = xyuv + i * 4;
        out[0] = v[0] * sx + ox;
        out[1] = v[1] * sy + oy;
        out[2] = v[2];
        out[3] = v[3];
        out[4] = c.r;
        out[5] = c.g;
        out[6] = c.b;
        out[7] = c.a;
        out[8] = slot;
        out += FLOATS_PER_VERTEX;
    }
}

void ShaderOGLUIBatch::appendQuad(const Key& key, float x, float y, float w, float h,
                                  float u0, float v0, float u1, float v1, const Color& c)
{
    // Ancho/alto negativos se dibujan igual que antes (quad volteado, mismo orden de vértices)
    if (w == 0.0f || h == 0.0f) return;
    // Mismos dos triángulos (y orden) que el quad de ShaderOGLImage: (0,1)(1,0)(0,0) (0,1)(1,1)(1,0)
    const float x1 = x + w, y1 = y + h;
    const float q[6 * 4] = {
        x,  y1, u0, v1,
        x1, y,  u1, v0,
        x,  y,  u0, v0,
        x,  y1, u0, v1,
        x1, y1, u1, v1,
        x1, y,  u1, v0,
    };
    appendTriangles(key, q, 6, 0.0f, 0.0f, 1.0f, 1.0f, c);
}

void ShaderOGLUIBatch::flush()
{
    if (vertices.empty()) return;

    auto* render = Components::get()->Render();
    const int numVerts = (int)(vertices.size() / FLOATS_PER_VERTEX);
    const GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);

    render->ChangeOpenGLFramebuffer(fbo);
    ComponentRender::ScopedTargetViewport vp(rw, rh);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    if (rectBlend) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                      // = renderRect
    } else {
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // = texto/imagen
    }
    render->ChangeOpenGLProgram(programID);

    const glm::mat4 projection = glm::ortho(0.0f, (float)rw, (float)rh, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv((GLint)projectionUniform, 1, GL_FALSE, &projection[0][0]);
    // Unidades sin textura en esta tanda: también apuntan a la primera (ningún sampler sin textura)
    for (int i = 0; i < MAX_TEXTURES; i++) {
        if (textureUniforms[i] < 0) continue;
        setTextureUniform((GLuint)textureUniforms[i], textures[i < numTextures ? i : 0], i);
    }
    glActiveTexture(GL_TEXTURE0);   // el resto del motor da por hecho la unidad 0 activa

    // Búfer "huérfano" en cada vaciado (glBufferData con nullptr): el driver da memoria nueva en vez
    // de esperar a que la GPU termine con el contenido anterior (con glBufferSubData sobre un VBO
    // compartido cada texto de la UI esperaba al anterior)
    const size_t bytes = vertices.size() * sizeof(float);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (bytes > vboCapacityBytes) {
        while (vboCapacityBytes < bytes) vboCapacityBytes *= 2;
    }
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vboCapacityBytes, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)bytes, vertices.data());
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    Profiler::get()->incrementDrawCall(GL_TRIANGLES, numVerts, 1, "UIBatch");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Estado GL de salida = el que dejaba el dibujo inmediato equivalente a la ÚLTIMA entrada:
    //   texto  (writeTextAtlasToFB)          -> blend encendido, FBO sin tocar
    //   imagen (ShaderOGLImage::renderTexture) -> blend como estaba, FBO 0
    //   rect   (ShaderOGLRect::renderRect)    -> blend apagado, FBO 0
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    if (lastBlend == Blend::Rect) {
        glDisable(GL_BLEND);
        render->ChangeOpenGLFramebuffer(0);
    } else if (lastBlend == Blend::Image) {
        if (!blendWasEnabled) glDisable(GL_BLEND);
        render->ChangeOpenGLFramebuffer(0);
    }

    vertices.clear();
    numTextures = 0;
}
