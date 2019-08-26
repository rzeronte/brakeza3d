
#include "Decal.h"
#include "../headers/Render/Logging.h"
#include "../headers/Render/Transforms.h"

Decal::Decal(Vertex3D position) {
    frustum = new Frustum();
    texture = new Texture();

    setDecal(true);
    setPosition(position);

    setupFromAxis();
}

float Decal::getSCoord(Vertex3D Q)
{
    return (T * (Q-P) / w ) + 0.5;
}

float Decal::getTCoord(Vertex3D Q)
{
    return (B * (Q-P) / h ) + 0.5;
}

void Decal::setupFromAxis()
{
    frustum->position = *getPosition();

    P = frustum->position;
    N = this->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    T = this->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    B = this->getRotation().getTranspose() * EngineSetup::getInstance()->right;
}

void Decal::setDimensions(float w, float h)
{
    this->w = w;
    this->h = h;
}

void Decal::updateFrustum()
{
    frustum->position = *getPosition();

    frustum->updateCenters();
    frustum->updatePoints();
    frustum->updatePlanes();
}

void Decal::getTriangles(Triangle *soupTriangles, int numSoupTriangles, Camera3D *camera)
{
    this->numTriangles = 0;

    setupFromAxis();
    updateFrustum();

    for (int i = 0; i < numSoupTriangles ; i++ ) {
        if (soupTriangles[i].parent->isDecal())  {
            continue;
        }

        SpriteDirectional3D *spriteDirectional = dynamic_cast<SpriteDirectional3D*> (soupTriangles[i].parent);
        Sprite3D *sprite = dynamic_cast<Sprite3D*> (soupTriangles[i].parent);

        if (spriteDirectional != NULL || sprite != NULL) {
            continue;
        }

        if (soupTriangles[i].isBackFaceCulling(&this->N)) {
            continue;
        }

        if (soupTriangles[i].testForClipping(
                frustum->planes,
                EngineSetup::getInstance()->NEAR_PLANE,
                EngineSetup::getInstance()->BOTTOM_PLANE
        )) {
            soupTriangles[i].clipping(
                    camera,
                    frustum->planes,
                    EngineSetup::getInstance()->NEAR_PLANE,
                    EngineSetup::getInstance()->BOTTOM_PLANE,
                    this,
                    this->modelTriangles,
                    this->numTriangles,
                    false
            );
            continue;
        }

        if (!frustum->isPointInFrustum(soupTriangles[i].Ao) &&
            !frustum->isPointInFrustum(soupTriangles[i].Bo) &&
            !frustum->isPointInFrustum(soupTriangles[i].Co)
        ) {
            continue;
        }

        modelTriangles[numTriangles] = soupTriangles[i];
        modelTriangles[numTriangles].parent = this;
        modelTriangles[numTriangles].isBSP = false;
        modelTriangles[numTriangles].is_clipped = false;
        modelTriangles[numTriangles].A = Transforms::objectToLocal(modelTriangles[numTriangles].Ao, this);
        modelTriangles[numTriangles].B = Transforms::objectToLocal(modelTriangles[numTriangles].Bo, this);
        modelTriangles[numTriangles].C = Transforms::objectToLocal(modelTriangles[numTriangles].Co, this);
        numTriangles++;
    }

    // Fix separation for avoid Z-fightingba
    float OffsetSeparation = 0.1;
    for (int i = 0; i < numTriangles ; i++) {
        modelTriangles[i].A = modelTriangles[i].A - N.getScaled(OffsetSeparation);
        modelTriangles[i].B = modelTriangles[i].B - N.getScaled(OffsetSeparation);
        modelTriangles[i].C = modelTriangles[i].C - N.getScaled(OffsetSeparation);
    }
}