
#include "../../include/Objects/LightPoint3D.h"
#include "../../include/Render/Billboard.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"

LightPoint3D::LightPoint3D() {
    frustum = new Frustum();
    frustum->setParent(this);
    frustum->setup(
        getPosition(),
        this->AxisForward(),
        this->AxisUp(),
        this->AxisRight(),
        180,
        ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
        100000
    );

    frustum->consoleInfo();

    this->showDeepMapping = false;
    this->showFrustum = false;

    this->sizeBuffer = EngineSetup::get()->screenWidth * EngineSetup::get()->screenHeight;
    shadowMappingBuffer = new float[sizeBuffer];
}

void LightPoint3D::syncFrustum()
{
    frustum->setPosition( this->getPosition() );
    frustum->setRotation( this->getRotation() );

    frustum->direction = this->getRotation().getTranspose() * EngineSetup::get()->forward;
    frustum->up = this->getRotation().getTranspose() * EngineSetup::get()->up;
    frustum->right = this->getRotation().getTranspose() * EngineSetup::get()->right;

    frustum->updateFrustum();
}

void LightPoint3D::clearShadowMappingBuffer() const {
    std::fill(shadowMappingBuffer, shadowMappingBuffer + sizeBuffer, NULL);
}

float LightPoint3D::getShadowMappingBuffer(int x, int y) const {
    return shadowMappingBuffer[(y * EngineSetup::get()->screenWidth) + x];
}

void LightPoint3D::setShadowMappingBuffer(int x, int y, float value) const {
    shadowMappingBuffer[(y * EngineSetup::get()->screenWidth) + x] = value;
}

void LightPoint3D::setColor(int r, int g, int b) {
    this->color = Color(r, g, b);
}

void LightPoint3D::setColorSpecularity(int r, int g, int b) {
    LightPoint3D::specularColor = Color(r, g, b);
}

Color LightPoint3D::mixColor(Color colorTexture, Vertex3D Q, Fragment *fragment)
{
    const float distance = Maths::distanceBetweenVertices(this->getPosition(), Q);

    Vertex3D P = this->getPosition();
    Vertex3D R = this->AxisForward();

    Vector3D Light = Vector3D(Q, P);
    Vertex3D L = Light.normal();

    const float C = powf(fmaxf(R * L, 0), this->p) / (this->kc + this->kl * distance + this->kq * (distance * distance));

    Color diffuseColor;
    if (C >= 1) {
        diffuseColor = this->color;
    } else {
        diffuseColor = Color::mixColor(colorTexture, this->color, C);;
    }

    if (!EngineSetup::get()->ENABLE_LIGHTS_SPECULAR) {
        return diffuseColor;
    }

    // specular
    Vector3D Viewer = Vector3D(Q, ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Vertex3D Ls = Light.getComponent().getInverse();
    Vertex3D V = Viewer.getComponent();
    Vertex3D N = Vertex3D(fragment->normalX, fragment->normalY, fragment->normalZ);
    Vertex3D H = this->getHalfWay(Vector3D(Q, P).getComponent() + V);

    float booleanCondition = 1;
    if (N * Ls > 0) {
        booleanCondition = 0;
    }

    const float K = powf(fmaxf(N * H, 0), this->specularComponent) * booleanCondition;

    Color specularColor = Color::mixColor(colorTexture, this->specularColor, K);

    return Color::mixColor(specularColor, diffuseColor, 0.5);
}

void LightPoint3D::onUpdate() {
    Object3D::onUpdate();
}

void LightPoint3D::setPower(float pNew) {
    this->p = pNew;
}

Vertex3D LightPoint3D::getHalfWay(Vertex3D v) {
    const float module  = v.getModule();

    return Vertex3D(
        v.x / module,
        v.y / module,
        v.z / module
    );
}

void LightPoint3D::setSpecularComponent(float m)
{
    this->specularComponent = m;
}

void LightPoint3D::drawDeepMap(int pos_x, int pos_y) const {
    int width = EngineSetup::get()->screenWidth;
    int height = EngineSetup::get()->screenHeight;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto pixelColor = getShadowMappingBuffer(j, i);
            EngineBuffers::getInstance()->setVideoBuffer(j + pos_x, i + pos_y, Color(pixelColor, pixelColor, pixelColor).getColor());
        }
    }
}
