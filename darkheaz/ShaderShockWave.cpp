
#include "ShaderShockWave.h"
#include "../include/EngineBuffers.h"
#include "../include/Render/Transforms.h"
#include "../include/ComponentsManager.h"

ShaderShockWave::ShaderShockWave(float size, float speed)
{
    this->size = size;
    this->waveSpeed = speed;
}

void ShaderShockWave::onUpdate(Vertex3D position)
{
    Shader::update();

    Point2D focalPoint = Transforms::WorldToPoint(position, ComponentsManager::get()->getComponentCamera()->getCamera());

    const float edgecut = fract(this->executionTime * waveSpeed) * (float) this->w;

    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {
            Point2D uv(x, y);
            Point2D modifiedUV = uv - focalPoint;
            float res = smoothstep( edgecut - size , edgecut + size,modifiedUV.getLength());

            const float invertedRes = 1.0f - res;

            uv = uv + modifiedUV.getScaled(res * invertedRes);

            if (Tools::isPixelInWindow(uv.x, uv.y)) {
                this->videoBuffer[y * this->w + x] = EngineBuffers::getInstance()->getVideoBuffer(uv.x, uv.y);
            }
        }
    }

    auto screenBuffer = EngineBuffers::getInstance()->videoBuffer;
    auto currentBuffer = this->videoBuffer;

    for (int i = 0; i < bufferSize ; i++, ++screenBuffer, ++currentBuffer) {
        *screenBuffer = *currentBuffer;
    }
}

float ShaderShockWave::fract(float x)
{
    return x - floor(x);
}

float ShaderShockWave::smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}