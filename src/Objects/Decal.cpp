
#include "../../headers/Objects/Decal.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Objects/SpriteDirectional3D.h"

Decal::Decal() : sprite(new Sprite3D())
{
    setDecal(true);
}

void Decal::setupCube(float sizeX, float sizeY, float sizeZ)
{
    this->w = sizeX;
    this->h = sizeY;

    cube = new Cube3D(sizeX, sizeY, sizeZ);
    cube->setDecal(true);
}

float Decal::getTCoord(Vertex3D Q)
{
    return (float) ( (T * (Q-P) / w ) + 0.5f );
}

float Decal::getSCoord(Vertex3D Q)
{
    return (float) (B * (Q-P) / h ) + 0.5f;
}

void Decal::setupFromAxis()
{
    sprite->setPosition(*getPosition());

    P = *getPosition();
    N = AxisForward();
    T = AxisUp();
    B = AxisRight();
}

void Decal::setDimensions(float w, float h)
{
    this->w = w;
    this->h = h;
}

void Decal::getTriangles(std::vector<Triangle*> &triangles, Camera3D *camera)
{
    this->modelTriangles.clear();

    setupFromAxis();

    int alreadyDecal = 0;
    int clipped = 0;
    int out = 0;

    for (int i = 0; i < triangles.size() ; i++ ) {
        if (triangles[i]->parent->isDecal())  {
            alreadyDecal++;
            continue;
        }

        SpriteDirectional3D *spriteDirectional = dynamic_cast<SpriteDirectional3D*> (triangles[i]->parent);
        Sprite3D *sprite = dynamic_cast<Sprite3D*> (triangles[i]->parent);

        // Decals ignoran sprites
        if (spriteDirectional != NULL || sprite != NULL) {
            continue;
        }

        triangles[i]->updateNormal();
        if (triangles[i]->getNormal().getNormalize() * this->N.getNormalize() >= 0) {
            continue;
        }

        if (triangles[i]->testForClipping(
                cube->planes,
                0,
                5
        )) {
            triangles[i]->clipping(
                    camera,
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

        if (!cube->isPointInside(triangles[i]->Ao) &&
            !cube->isPointInside(triangles[i]->Bo) &&
            !cube->isPointInside(triangles[i]->Co)
        ) {
            out++;
            continue;
        }

        Triangle *t = triangles[i];
        t->parent = this;
        t->isBSP = false;
        t->clipped = true;
        t->A = Transforms::objectToLocal(t->Ao, this);
        t->B = Transforms::objectToLocal(t->Bo, this);
        t->C = Transforms::objectToLocal(t->Co, this);

        modelTriangles.emplace_back( t );
    }

    // Fix separation for avoid Z-fighting
    float OffsetSeparation = 0.25;
    for (int i = 0; i < modelTriangles.size() ; i++) {
        modelTriangles[i]->A = modelTriangles[i]->A - N.getScaled(OffsetSeparation);
        modelTriangles[i]->B = modelTriangles[i]->B - N.getScaled(OffsetSeparation);
        modelTriangles[i]->C = modelTriangles[i]->C - N.getScaled(OffsetSeparation);
    }
}

Sprite3D *Decal::getSprite() const {
    return sprite;
}

void Decal::setSprite(Sprite3D *sprite) {
    Decal::sprite = sprite;
}