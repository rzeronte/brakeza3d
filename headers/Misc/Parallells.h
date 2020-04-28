#ifndef BRAKEDA3D_PARALLELLS_H
#define BRAKEDA3D_PARALLELLS_H


void *ParallellInitBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles)
{
    //Logging::getInstance()->Log("Loading BSP Quake map: " + std::string(bspFilename));

    EngineSetup::getInstance()->BULLET_STEP_SIMULATION = true;

    //Brakeza3D::get()->getBSP()->Initialize(bspFilename, "palette.lmp", frameTriangles);

    //Game::get()->player->respawn();
    //Game::get()->player->respawnNPCS();

    EngineSetup::getInstance()->DRAW_WEAPON = true;
    EngineSetup::getInstance()->DRAW_HUD    = true;
    EngineSetup::getInstance()->LOADING     = false;

}

void ParallellDrawTileTriangles(int i, std::vector<Triangle*> *visibleTriangles)
{
    ComponentsManager::get()->getComponentRender()->drawTileTriangles(i, *visibleTriangles);
}

void Object3DOnUpdate(Object3D *o)
{
    o->onUpdate();
}

#endif //BRAKEDA3D_PARALLELLS_H
