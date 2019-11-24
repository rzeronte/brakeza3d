#ifndef BRAKEDA3D_PARALLELLS_H
#define BRAKEDA3D_PARALLELLS_H

#include "../Brakeza3D.h"
#include "../Game/Game.h"

void *ParalellProcessPixel(Triangle *t, int bufferIndex, int x, int y, float w0, float w1, float w2, float z, float texu, float texv, float lightu, float lightv)
{
    Brakeza3D::get()->processPixel(
            t,
            bufferIndex,
            x, y,
            w0, w1, w2,
            z,
            texu, texv,
            lightu, lightv
    );
}

void *ParallellInitBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles)
{
    const char *paletteFilename = "palette.lmp";
    Logging::getInstance()->Log("Loading BSP Quake map: " + std::string(bspFilename));

    EngineSetup::getInstance()->BULLET_STEP_SIMULATION = true;

    Brakeza3D::get()->getBSP()->Initialize(bspFilename, paletteFilename, frameTriangles);
    Game::get()->player->respawn();
    Game::get()->player->respawnNPCS();

    EngineSetup::getInstance()->DRAW_WEAPON = true;
    EngineSetup::getInstance()->DRAW_HUD    = true;
    EngineSetup::getInstance()->LOADING     = false;
}


#endif //BRAKEDA3D_PARALLELLS_H
