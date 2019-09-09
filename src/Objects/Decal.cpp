
#include "../../headers/Objects/Decal.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"

Decal::Decal() {
    sprite = new Sprite3D();
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
    return (T * (Q-P) / w ) + 0.5;
}

float Decal::getSCoord(Vertex3D Q)
{
    return (B * (Q-P) / h ) + 0.5;
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

void Decal::getTriangles(Triangle *soupTriangles, int numSoupTriangles, Camera3D *camera)
{
    this->numTriangles = 0;

    setupFromAxis();

    int alreadyDecal = 0;
    int clipped = 0;
    int out = 0;

    for (int i = 0; i < numSoupTriangles ; i++ ) {
        if (soupTriangles[i].parent->isDecal())  {
            alreadyDecal++;
            continue;
        }

        SpriteDirectional3D *spriteDirectional = dynamic_cast<SpriteDirectional3D*> (soupTriangles[i].parent);
        Sprite3D *sprite = dynamic_cast<Sprite3D*> (soupTriangles[i].parent);

        // Decals ignoran sprites
        if (spriteDirectional != NULL || sprite != NULL) {
            continue;
        }

        soupTriangles[i].updateNormal();
        if (soupTriangles[i].getNormal().getNormalize() * this->N.getNormalize() >= 0) {
            continue;
        }

        if (soupTriangles[i].testForClipping(
                cube->planes,
                0,
                5
        )) {
            soupTriangles[i].clipping(
                    camera,
                    cube->planes,
                    0,
                    5,
                    this,
                    this->modelTriangles,
                    this->numTriangles,
                    false
            );
            clipped++;
            continue;
        }

        if (!cube->isPointInside(soupTriangles[i].Ao) &&
            !cube->isPointInside(soupTriangles[i].Bo) &&
            !cube->isPointInside(soupTriangles[i].Co)
        ) {
            out++;
            continue;
        }

        modelTriangles[numTriangles] = soupTriangles[i];
        modelTriangles[numTriangles].parent = this;
        modelTriangles[numTriangles].isBSP = false;
        modelTriangles[numTriangles].clipped = true;
        modelTriangles[numTriangles].A = Transforms::objectToLocal(modelTriangles[numTriangles].Ao, this);
        modelTriangles[numTriangles].B = Transforms::objectToLocal(modelTriangles[numTriangles].Bo, this);
        modelTriangles[numTriangles].C = Transforms::objectToLocal(modelTriangles[numTriangles].Co, this);
        numTriangles++;
    }

    // Fix separation for avoid Z-fightingba
    float OffsetSeparation = 0.25;
    for (int i = 0; i < numTriangles ; i++) {
        modelTriangles[i].A = modelTriangles[i].A - N.getScaled(OffsetSeparation);
        modelTriangles[i].B = modelTriangles[i].B - N.getScaled(OffsetSeparation);
        modelTriangles[i].C = modelTriangles[i].C - N.getScaled(OffsetSeparation);
    }
}

Sprite3D *Decal::getSprite() const {
    return sprite;
}

void Decal::setSprite(Sprite3D *sprite) {
    Decal::sprite = sprite;
}
