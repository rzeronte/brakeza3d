
#include "../../include/Objects/Decal.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Objects/SpriteDirectional3D.h"
#include "../../include/ComponentsManager.h"

Decal::Decal() : sprite(new Sprite3D()) {
    setDecal(true);
}

void Decal::setupCube(float sizeX, float sizeY, float sizeZ) {
    this->w = sizeX;
    this->h = sizeY;

    cube = new Cube3D(sizeX, sizeY, sizeZ);
    cube->setDecal(true);
}

float Decal::getTCoord(Vertex3D Q) const {
    return (T * (Q - P) / w) + 0.5f;
}

float Decal::getSCoord(Vertex3D Q) const {
    return B * (Q - P) / h + 0.5f;
}

void Decal::setupFromAxis() {
    sprite->setPosition(getPosition());

    P = getPosition();
    N = AxisForward();
    T = AxisUp();
    B = AxisRight();
}

void Decal::setDimensions(float w, float h) {
    this->w = w;
    this->h = h;
}

void Decal::getTriangles(std::vector<Triangle *> &triangles, Camera3D *camera) {
    this->modelTriangles.clear();

    setupFromAxis();

    int alreadyDecal = 0;
    int clipped = 0;
    int out = 0;

    for (auto & triangle : triangles) {
        if (!triangle->bspTriangle) continue;

        if (triangle->parent->isDecal()) {
            alreadyDecal++;
            continue;
        }

        auto *spriteDirectional = dynamic_cast<SpriteDirectional3D *> (triangle->parent);
        auto *sprite = dynamic_cast<Sprite3D *> (triangle->parent);

        // Decals ignoran sprites
        if (spriteDirectional != nullptr || sprite != nullptr) {
            continue;
        }

        triangle->updateNormal();
        if (triangle->getNormal().getNormalize() * this->N.getNormalize() >= 0) {
            continue;
        }

        if (triangle->testForClipping(cube->planes, 0, 5)) {
            triangle->clipping(
                    camera->frustum,
                    cube->planes,
                    0,
                    5,
                    this,
                    this->modelTriangles,
                    false
            );
            clipped++;
            continue;
        }

        if (!cube->isPointInside(triangle->Ao) &&
            !cube->isPointInside(triangle->Bo) &&
            !cube->isPointInside(triangle->Co)
                ) {
            out++;
            continue;
        }

        Triangle *t = triangle;
        t->parent = this;
        t->bspTriangle = false;
        t->clipped = true;
        t->A = Transforms::objectToLocal(t->Ao, this);
        t->B = Transforms::objectToLocal(t->Bo, this);
        t->C = Transforms::objectToLocal(t->Co, this);

        modelTriangles.emplace_back(t);
    }

    // Fix separation for avoid Z-fighting
    float OffsetSeparation = 0.25;
    for (auto & modelTriangle : modelTriangles) {
        modelTriangle->A = modelTriangle->A - N.getScaled(OffsetSeparation);
        modelTriangle->B = modelTriangle->B - N.getScaled(OffsetSeparation);
        modelTriangle->C = modelTriangle->C - N.getScaled(OffsetSeparation);
    }
}

Sprite3D *Decal::getSprite() const {
    return sprite;
}

void Decal::setSprite(Sprite3D *sprite) {
    Decal::sprite = sprite;
}

void Decal::onUpdate() {
    if (EngineSetup::get()->DRAW_DECAL_WIREFRAMES) {
        this->cube->sendTrianglesToFrame(&ComponentsManager::get()->getComponentRender()->getFrameTriangles());
    }
}
