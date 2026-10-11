//
// Dibujado de UI por lotes: acumula quads (posición, UV, color por vértice, textura) y los dibuja con
// un solo glDrawArrays mientras compartan destino (FBO + tamaño) y modo de mezcla. Hasta MAX_TEXTURES
// texturas distintas por tanda (una unidad de textura cada una; el vértice lleva su índice): cambiar
// de textura ya no vacía el lote, solo la (MAX_TEXTURES+1)-ésima distinta. Lo usan los textos de los
// widgets (TextWriter) y, desde 2026-10-10, sus imágenes, marcos 9-slice y rectángulos
// (ComponentRender, solo mientras UIManager pinta un widget: ver ComponentRender::UIDeferScope).
// Cualquier dibujo inmediato de la UI vacía antes el lote (ComponentRender::flushUIBatch) para
// conservar el orden de pintado.
//

#ifndef BRAKEZA3D_SHADEROGLUIBATCH_H
#define BRAKEZA3D_SHADEROGLUIBATCH_H

#define GL_GLEXT_PROTOTYPES

#include <vector>
#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"

class ShaderOGLUIBatch : public ShaderBaseOpenGL
{
public:
    static constexpr int MAX_TEXTURES = 8;   // = sampler "images[8]" de UIBatch.fs

    // Text e Image usan la misma mezcla (la de writeTextAtlasToFB / ShaderOGLImage::renderTexture) y
    // comparten tanda; solo cambia el estado GL que se deja al vaciar (ver flush). Rect es la mezcla de
    // ShaderOGLRect::renderRect (también el alfa con SRC_ALPHA): tanda aparte.
    // RectAny: rect con alfa 0 o 1 -- las dos mezclas dan el mismo píxel (1·1 = 1, 0·0 = 0), así que
    // se une a la tanda abierta, sea de imágenes o de rects, sin cortarla (2026-10-10).
    enum class Blend { Text = 0, Image = 1, Rect = 2, RectAny = 3 };

    struct Key {
        GLuint fbo = 0;
        int    rw = 0, rh = 0;       // tamaño del destino (viewport y proyección)
        GLuint texture = 0;          // no forma parte de la tanda: se asigna a una unidad libre
        Blend  blend = Blend::Text;
    };

    ShaderOGLUIBatch();

    void PrepareMainThread() override;
    void LoadUniforms() override;
    void Destroy() override;

    // Añade triángulos con vértices (x, y, u, v) en px "de ventana": se pasan a px del destino con
    // pos * (sx, sy) + (ox, oy). Si el destino o la mezcla no coinciden con los del lote pendiente, o
    // ya no caben más texturas, lo vacía antes.
    void appendTriangles(const Key& key, const float* xyuv, int numVerts,
                         float ox, float oy, float sx, float sy, const Color& c);
    // Quad alineado a ejes en px del DESTINO (ya escalados), UV (u0,v0) arriba-izquierda y (u1,v1)
    // abajo-derecha: mismo resultado que ShaderOGLImage::renderTexture / renderTextureRegion.
    void appendQuad(const Key& key, float x, float y, float w, float h,
                    float u0, float v0, float u1, float v1, const Color& c);
    void flush();
    [[nodiscard]] bool hasPending() const { return !vertices.empty(); }
    // Textura blanca 1x1: los rectángulos lisos se pintan con ella (color = color del vértice)
    [[nodiscard]] GLuint getWhiteTexture() const { return whiteTexture; }

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint whiteTexture = 0;
    size_t vboCapacityBytes = 0;
    GLuint projectionUniform = 0;
    GLint  textureUniforms[MAX_TEXTURES] = {};

    // Lote pendiente
    GLuint fbo = 0;
    int    rw = 0, rh = 0;
    bool   rectBlend = false;          // Blend::Rect
    Blend  lastBlend = Blend::Text;    // tipo de la última entrada: decide el estado GL al vaciar
    GLuint textures[MAX_TEXTURES] = {};
    int    numTextures = 0;
    std::vector<float> vertices;       // 9 floats por vértice: x, y, u, v, r, g, b, a, índice de textura

    // Prepara el lote para una entrada nueva y devuelve el índice de su textura
    float beginEntry(const Key& key);
};

#endif //BRAKEZA3D_SHADEROGLUIBATCH_H
