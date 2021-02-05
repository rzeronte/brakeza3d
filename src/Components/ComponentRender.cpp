//
// Created by darkhead on 8/1/20.
//

#include "../../headers/Components/ComponentRender.h"
#include "../../headers/Components/ComponentWindow.h"
#include "../../headers/ComponentsManager.h"
#include "../../headers/Misc/Parallells.h"
#include "../../headers/Brakeza3D.h"

ComponentRender::ComponentRender()
{
}

void ComponentRender::onStart()
{
    std::cout << "ComponentRender onStart" << std::endl;
    initTiles();
}

void ComponentRender::preUpdate()
{
}

void ComponentRender::onUpdate()
{
    this->onUpdateBSP();
    this->onUpdateSceneObjects();

    this->hiddenSurfaceRemoval();
    this->drawVisibleTriangles();

    if (SETUP->BULLET_DEBUG_MODE) {
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->debugDrawWorld();
    }

    if (SETUP->RENDER_OBJECTS_AXIS) {
        this->drawSceneObjectsAxis();
    }

    if (SETUP->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis( ComponentsManager::get()->getComponentCamera()->getCamera() );
    }
}

void ComponentRender::postUpdate() {

}

void ComponentRender::onEnd()
{
}

void ComponentRender::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

std::vector<Triangle *> &ComponentRender::getFrameTriangles()
{
    return frameTriangles;
}

std::vector<Triangle *> &ComponentRender::getVisibleTriangles()
{
    return visibleTriangles;
}

std::vector<LightPoint3D *> &ComponentRender::getLightPoints()
{
    return lightpoints;
}

void ComponentRender::onUpdateBSP()
{
    /*ComponentsManager::get()->getComponentBSP()->getBSPCollider()->drawHullAABB(
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getWorldModel(),
            0
    );*/

    if (!SETUP->RENDER_BSP_TRIANGLES) return;

    if (ComponentsManager::get()->getComponentBSP()->getBSP()->isLoaded() ) {

        ComponentsManager::get()->getComponentBSP()->getBSP()->DrawVisibleLeaf(
                ComponentsManager::get()->getComponentCamera()->getCamera()
        );


        if (SETUP->DRAW_BSP_HULLS) {
            ComponentsManager::get()->getComponentBSP()->getBSP()->DrawHulls(ComponentsManager::get()->getComponentCamera()->getCamera() );
        }
    }
}

void ComponentRender::onUpdateSceneObjects()
{
    if (!SETUP->EXECUTE_GAMEOBJECTS_ONUPDATE) return;

    std::vector<Object3D *>::iterator it;

    for ( it = getSceneObjects()->begin(); it != getSceneObjects()->end(); ) {
        Object3D *object = *(it);

        // Check for delete
        if ( object->isRemoved() ) {
            getSceneObjects()->erase(it);
            continue;
        } else {
            it++;
        }

        if ( !object->isEnabled() ) {
            continue;
        }

        object->onUpdate();
    }
}

void ComponentRender::hiddenSurfaceRemovalTriangle(Triangle *t)
{
    Camera3D *cam = ComponentsManager::get()->getComponentCamera()->getCamera();

    t->updateObjectSpace();
    t->updateNormal();

    // back face culling (needs objectSpace)
    if (t->isBackFaceCulling( &cam->getPosition() )) {
        return;
    }

    // Clipping (needs objectSpace)
    if (t->testForClipping( cam->frustum->planes, SETUP->LEFT_PLANE, SETUP->BOTTOM_PLANE )) {
        if (SETUP->ENABLE_CLIPPING) {
            t->clipping(
                    cam,
                    cam->frustum->planes,
                    SETUP->LEFT_PLANE,
                    SETUP->BOTTOM_PLANE,
                    t->parent,
                    clippedTriangles,
                    t->isBSP
            );
        }

        return;
    }

    // Frustum Culling (needs objectSpace)
    if (!cam->frustum->isPointInFrustum(t->Ao) &&
        !cam->frustum->isPointInFrustum(t->Bo) &&
        !cam->frustum->isPointInFrustum(t->Co)
    ) {
        return;
    }

    // Triangle precached data
    // Estas operaciones las hacemos después de descartar triángulos
    // para optimización en el rasterizador por software
    t->updateCameraSpace( cam );
    t->updateNDCSpace( cam );
    t->updateScreenSpace();
    t->updateBoundingBox();
    t->updateFullArea();
    t->updateUVCache();

    t->drawed = false;
    visibleTriangles.emplace_back( t );
}

void ComponentRender::hiddenSurfaceRemoval()
{
    for (int i = 0 ; i < clippedTriangles.size(); i++) {
        delete clippedTriangles[i];
    }
    clippedTriangles.clear();

    visibleTriangles.clear();
    for (int i = 0; i < frameTriangles.size() ; i++) {
        this->hiddenSurfaceRemovalTriangle( frameTriangles[i] );
    }

    visibleTriangles.insert(
            visibleTriangles.end(),
            std::make_move_iterator(clippedTriangles.begin()),
            std::make_move_iterator(clippedTriangles.end())
    );

    if (SETUP->DEBUG_RENDER_INFO) {
        Logging::getInstance()->Log("[DEBUG_RENDER_INFO] frameTriangles: " + std::to_string(frameTriangles.size()) + ", numVisibleTriangles: " + std::to_string(visibleTriangles.size()));
    }

    frameTriangles.clear();
}

void ComponentRender::drawVisibleTriangles()
{
    if (SETUP->BASED_TILE_RENDER) {

        this->handleTrianglesToTiles(visibleTriangles);
        this->drawTilesTriangles(&visibleTriangles);

        if (SETUP->DRAW_TILES_GRID) {
            this->drawTilesGrid();
        }

        return;
    }

    this->drawTriangles(visibleTriangles);
}

void ComponentRender::handleTrianglesToTiles(std::vector<Triangle*> &visibleTriangles)
{
    for (int i = 0; i < this->numTiles; i++) {
        this->tiles[i].triangleIds.clear();
    }

    for (int i = 0; i < visibleTriangles.size(); i++) {
        int tileStartX = std::max((float) (visibleTriangles[i]->minX / this->sizeTileWidth), 0.0f);
        int tileEndX   = std::min((float) (visibleTriangles[i]->maxX / this->sizeTileWidth), (float) this->tilesWidth - 1);

        int tileStartY = std::max((float) (visibleTriangles[i]->minY / this->sizeTileHeight), 0.0f);
        int tileEndY   = std::min((float) (visibleTriangles[i]->maxY / this->sizeTileHeight), (float) this->tilesHeight - 1);

        for (int y = tileStartY; y <= tileEndY; y++) {
            for (int x = tileStartX; x <= tileEndX; x++) {
                this->tiles[ y * tilesWidth + x ].triangleIds.emplace_back( i );
            }
        }
    }
}

void ComponentRender::drawTilesGrid()
{
    for (int j = 0 ; j < (this->tilesWidth*this->tilesHeight) ; j++) {
        Tile *t = &this->tiles[j];
        Uint32 color = Color::white();

        if (t->triangleIds.size() > 0) {
            color = Color::red();
        }

        Line2D top = Line2D(t->start_x, t->start_y, t->start_x+sizeTileWidth, t->start_y);
        Line2D left = Line2D(t->start_x, t->start_y, t->start_x, t->start_y+sizeTileHeight);
        Drawable::drawLine2D(top, color);
        Drawable::drawLine2D(left, color);
    }
}

void ComponentRender::drawTriangles(std::vector<Triangle*> &visibleTriangles)
{
    std::vector<Triangle *>::iterator it;
    for ( it = visibleTriangles.begin(); it != visibleTriangles.end(); it++) {
        Triangle *triangle = *(it);
        processTriangle( triangle );
    }
}

void ComponentRender::processTriangle(Triangle *t)
{
    // degradate
    if (t->getTexture() != NULL && SETUP->TRIANGLE_MODE_TEXTURIZED) {
        triangleRasterizer(t);
    }

    // texture
    if (SETUP->TRIANGLE_MODE_COLOR_SOLID) {
        triangleRasterizer(t);
    }

    // wireframe
    if (SETUP->TRIANGLE_MODE_WIREFRAME || (t->parent->isDecal() && SETUP->DRAW_DECAL_WIREFRAMES)) {
        drawWireframe(t);
    }

    // Pixels
    if (SETUP->TRIANGLE_MODE_PIXELS ) {
        Camera3D *CC = ComponentsManager::get()->getComponentCamera()->getCamera();
        Drawable::drawVertex(t->Co, CC, Color::green());
        Drawable::drawVertex(t->Bo, CC, Color::green());
        Drawable::drawVertex(t->Co, CC, Color::green());
    }

    t->drawed = true;
}

void ComponentRender::triangleRasterizer(Triangle *t)
{
    // LOD determination
    t->lod = t->processLOD();

    // Triangle setup
    int A01 = (int) (-t->As.y + t->Bs.y);
    int A12 = (int) (-t->Bs.y + t->Cs.y);
    int A20 = (int) (-t->Cs.y + t->As.y);

    int B01 = (int) (-t->Bs.x + t->As.x);
    int B12 = (int) (-t->Cs.x + t->Bs.x);
    int B20 = (int) (-t->As.x + t->Cs.x);

    Point2D startP(t->minX, t->minY);
    int w0_row = Maths::orient2d(t->Bs, t->Cs, startP);
    int w1_row = Maths::orient2d(t->Cs, t->As, startP);
    int w2_row = Maths::orient2d(t->As, t->Bs, startP);

    auto *fragment = new Fragment();
    bool bilinear = EngineSetup::getInstance()->TEXTURES_BILINEAR_INTERPOLATION;

    for (int y = t->minY ; y < t->maxY ; y++) {
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = t->minX ; x < t->maxX ; x++) {

            if ((w0 | w1 | w2) > 0) {
                fragment->alpha = (int) w0 * t->reciprocalFullArea;
                fragment->theta = (int) w1 * t->reciprocalFullArea;
                fragment->gamma = 1 - fragment->alpha - fragment->theta;

                fragment->depth = (fragment->alpha * t->An.z) + (fragment->theta * t->Bn.z) + (fragment->gamma * t->Cn.z);

                int bufferIndex = ( y * SETUP->screenWidth ) + x;

                if (t->parent->isDecal()) {
                    fragment->depth-=1;
                }

                if ( fragment->depth < BUFFERS->getDepthBuffer( bufferIndex )) {
                    fragment->affineUV = 1 / (fragment->alpha * (t->persp_correct_Az) + fragment->theta * (t->persp_correct_Bz) + fragment->gamma * (t->persp_correct_Cz) );
                    fragment->texU = (fragment->alpha * (t->tex_u1_Ac_z) + fragment->theta * (t->tex_u2_Bc_z) + fragment->gamma * (t->tex_u3_Cc_z) ) * fragment->affineUV;
                    fragment->texV = (fragment->alpha * (t->tex_v1_Ac_z) + fragment->theta * (t->tex_v2_Bc_z) + fragment->gamma * (t->tex_v3_Cc_z) ) * fragment->affineUV;

                    if (t->getLightmap() != NULL) {
                        fragment->lightU = (fragment->alpha * (t->light_u1_Ac_z) + fragment->theta * (t->light_u2_Bc_z) + fragment->gamma * (t->light_u3_Cc_z) ) * fragment->affineUV;
                        fragment->lightV = (fragment->alpha * (t->light_v1_Ac_z) + fragment->theta * (t->light_v2_Bc_z) + fragment->gamma * (t->light_v3_Cc_z) ) * fragment->affineUV;
                    }

                    this->processPixel( t, bufferIndex, x, y, fragment, bilinear);
                }
            }

            // edge function increments
            w0 += A12;
            w1 += A20;
            w2 += A01;
        }

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }

    delete fragment;
}

void ComponentRender::processPixel(Triangle *t, int bufferIndex, const int x, const int y, Fragment *fragment, bool bilinear)
{
    Uint32 pixelColor(NULL);

    if (SETUP->TRIANGLE_MODE_COLOR_SOLID) {
        pixelColor = (Uint32) Tools::createRGB(fragment->alpha * 255, fragment->theta * 255, fragment->gamma * 255);
    }

    // Texture
    if (SETUP->TRIANGLE_MODE_TEXTURIZED && t->getTexture() != NULL) {
        if (t->getTexture()->liquid && SETUP->TRIANGLE_TEXTURES_ANIMATED ) {
            float cache1 = fragment->texU / SETUP->LAVA_CLOSENESS;
            float cache2 = fragment->texV / SETUP->LAVA_CLOSENESS;
            fragment->texU = (cache1 + SETUP->LAVA_INTENSITY * sin(SETUP->LAVA_SPEED * Brakeza3D::get()->executionTime + cache2) ) * SETUP->LAVA_SCALE;
            fragment->texV = (cache2 + SETUP->LAVA_INTENSITY * sin(SETUP->LAVA_SPEED * Brakeza3D::get()->executionTime + cache1) ) * SETUP->LAVA_SCALE;
        }

        /*if ( t->parent->isDecal() ) {
            if ((fragment->texU < 0 || fragment->texU > 1) || (fragment->texV < 0 || fragment->texV > 1) ) {
                return;
            }
        }*/

        t->processPixelTexture(pixelColor, fragment->texU, fragment->texV, bilinear);

        Uint8 red, green, blue, alpha;
        SDL_GetRGBA(pixelColor, t->texture->getSurface(t->lod)->format, &red, &green, &blue, &alpha);

        if (alpha == 0) {
            return;
        } else {
            pixelColor = Tools::alphaBlend( BUFFERS->getVideoBuffer( x, y ), pixelColor, alpha );
        }

        if (!t->parent->isDecal() && t->getLightmap() != NULL && !t->getTexture()->liquid && SETUP->ENABLE_LIGHTMAPPING) {
            t->processPixelLightmap(pixelColor, fragment->lightU, fragment->lightV, red, green, blue, alpha);
        }
    }

    if (SETUP->ENABLE_FOG) {
        float nZ = Maths::normalizeToRange(fragment->depth, 0, SETUP->FOG_DISTANCE);

        if (nZ >= 1) {
            pixelColor = SETUP->FOG_COLOR;
        } else {
            pixelColor = Tools::mixColor(pixelColor, SETUP->FOG_COLOR, nZ * SETUP->FOG_INTENSITY);
        }
    }
    if (EngineSetup::getInstance()->ENABLE_LIGHTS) {
        Vertex3D D;

        if (this->lightpoints.size() > 0) {
            // Coordenadas del punto que estamos procesando en el mundo (object space)
            float x3d = fragment->alpha * t->Ao.x + fragment->theta * t->Bo.x + fragment->gamma * t->Co.x;
            float y3d = fragment->alpha * t->Ao.y + fragment->theta * t->Bo.y + fragment->gamma * t->Co.y;
            float z3d = fragment->alpha * t->Ao.z + fragment->theta * t->Bo.z + fragment->gamma * t->Co.z;

            D = Vertex3D( x3d, y3d, z3d ); // Object space

            for (int i = 0; i < this->lightpoints.size(); i++) {
                if (!this->lightpoints[i]->isEnabled()) {
                    continue;
                }

                // Color light apply
                pixelColor = this->lightpoints[i]->mixColor(pixelColor, D);

                /*if (EngineSetup::getInstance()->ENABLE_SHADOW_CASTING) {
                    Mesh3D *isMesh = dynamic_cast<Mesh3D*> (parent);

                    if (isMesh != NULL && isMesh->isShadowCaster()) {
                        // Shadow test
                        Vertex3D Dl = Transforms::cameraSpace(D, this->lightPoints[i]->cam);
                        Dl = Transforms::NDCSpace(Dl, this->lightPoints[i]->cam);
                        const Point2D DP = Transforms::screenSpace(Dl, this->lightPoints[i]->cam);

                        if (Tools::isPixelInWindow(DP.x, DP.y)) {
                            float buffer_shadowmapping_z = this->lightPoints[i]->getShadowMappingBuffer(DP.x, DP.y);
                            if ( Dl.z > buffer_shadowmapping_z) {
                                pixelColor = Color::red();
                            }
                        }
                    }
                }*/
            }
        }
    }

    BUFFERS->setDepthBuffer(bufferIndex, fragment->depth);
    BUFFERS->setVideoBuffer(bufferIndex, pixelColor);
}

void ComponentRender::drawTilesTriangles(std::vector<Triangle*> *visibleTriangles)
{
    std::vector<std::thread> threads;
    for (int i = 0 ; i < numTiles ; i++) {
        if (!tiles[i].draw) continue;

        if (SETUP->BASED_TILE_RENDER_THREADED) {
            threads.emplace_back( std::thread(ParallellDrawTileTriangles, i, visibleTriangles) );
        } else {
            this->drawTileTriangles(i, *visibleTriangles);
        }
    }

    if (SETUP->BASED_TILE_RENDER_THREADED) {
        for (std::thread & th : threads) {
            if (th.joinable()) {
                th.join();
            }
        }
    }
}

void ComponentRender::drawSceneObjectsAxis()
{
    for (int i = 0; i < getSceneObjects()->size(); i++) {
        if (
            getSceneObjects()->operator[](i)->isEnabled() &&
                (
                    !getSceneObjects()->operator[](i)->getLabel().compare( "test1") ||
                    !getSceneObjects()->operator[](i)->getLabel().compare( "test2")
                )
        ) {
            Drawable::drawObject3DAxis(getSceneObjects()->operator[](i), ComponentsManager::get()->getComponentCamera()->getCamera(), true, true, true);
        }
    }
}

void ComponentRender::initTiles()
{
    if (SETUP->screenWidth % this->sizeTileWidth != 0) {
        printf("Bad sizetileWidth\r\n");
        exit(-1);
    }
    if (SETUP->screenHeight % this->sizeTileHeight != 0) {
        printf("Bad sizeTileHeight\r\n");
        exit(-1);
    }

    // Tiles Raster setup
    this->tilesWidth  = SETUP->screenWidth / this->sizeTileWidth;
    this->tilesHeight = SETUP->screenHeight / this->sizeTileHeight;
    this->numTiles = tilesWidth * tilesHeight;
    this->tilePixelsBufferSize = this->sizeTileWidth*this->sizeTileHeight;

    Logging::getInstance()->Log("Tiles: ("+std::to_string(tilesWidth)+"x"+std::to_string(tilesHeight)+"), Size: ("+std::to_string(sizeTileWidth)+"x"+std::to_string(sizeTileHeight)+") - bufferTileSize: " + std::to_string(sizeTileWidth*sizeTileHeight));

    for (int y = 0 ; y < SETUP->screenHeight; y+=this->sizeTileHeight) {
        for (int x = 0 ; x < SETUP->screenWidth; x+=this->sizeTileWidth) {

            Tile t;

            t.draw    = true;
            t.id_x    = (x/this->sizeTileWidth);
            t.id_y    = (y/this->sizeTileHeight);
            t.id      = t.id_y * this->tilesWidth + t.id_x;
            t.start_x = x;
            t.start_y = y;

            this->tiles.emplace_back(t);
            // Load up the vector with MyClass objects


            Logging::getInstance()->Log("Tiles: (id:" + std::to_string(t.id) + "), (offset_x: " + std::to_string(x)+", offset_y: " + std::to_string(y) + ")");
        }
    }

    // Create local buffers and openCL buffer pointers
    for (int i = 0; i < numTiles; i++) {

        this->tiles[i].buffer = new unsigned int[tilePixelsBufferSize];
        for (int j = 0; j < tilePixelsBufferSize ; j++) {
            this->tiles[i].buffer[j] = Color::red();
        }

        this->tiles[i].bufferDepth = new float[tilePixelsBufferSize];
        for (int j = 0; j < tilePixelsBufferSize ; j++) {
            this->tiles[i].bufferDepth[j] = 0;
        }
    }
}

void ComponentRender::drawTileTriangles(int i, std::vector<Triangle*> &visibleTriangles)
{
    for (int j = 0 ; j < this->tiles[i].triangleIds.size() ; j++) {
        int triangleId = this->tiles[i].triangleIds[j];
        Tile *t = &this->tiles[i];

        this->softwareRasterizerForTile(
                visibleTriangles[triangleId],
                t->start_x,
                t->start_y,
                t->start_x + sizeTileWidth,
                t->start_y + sizeTileHeight
        );
    }
}

void ComponentRender::softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY)
{
    // LOD determination
    t->lod = t->processLOD();

    // Triangle setup
    int A01 = (int) (-t->As.y + t->Bs.y);
    int A12 = (int) (-t->Bs.y + t->Cs.y);
    int A20 = (int) (-t->Cs.y + t->As.y);

    int B01 = (int) (-t->Bs.x + t->As.x);
    int B12 = (int) (-t->Cs.x + t->Bs.x);
    int B20 = (int) (-t->As.x + t->Cs.x);

    Point2D startP(t->minX, t->minY);
    int w0_row = Maths::orient2d(t->Bs, t->Cs, startP);
    int w1_row = Maths::orient2d(t->Cs, t->As, startP);
    int w2_row = Maths::orient2d(t->As, t->Bs, startP);

    Fragment fragment;
    bool bilinear = EngineSetup::getInstance()->TEXTURES_BILINEAR_INTERPOLATION;

    for (int y = t->minY ; y < t->maxY ; y++) {
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = t->minX ; x < t->maxX ; x++) {

            if ((w0 | w1 | w2) > 0) {
                if (! ((x < minTileX || x > maxTileX) || (y < minTileY || y > maxTileY )) ) {

                    fragment.alpha = (int) w0 * t->reciprocalFullArea;
                    fragment.theta = (int) w1 * t->reciprocalFullArea;
                    fragment.gamma = 1 - fragment.alpha - fragment.theta;

                    fragment.depth = (fragment.alpha * t->An.z) + (fragment.theta * t->Bn.z) + (fragment.gamma * t->Cn.z);

                    const int bufferIndex = y * SETUP->screenWidth + x;
                    if ( fragment.depth < BUFFERS->getDepthBuffer( bufferIndex )) {
                        fragment.affineUV = 1 / ((fragment.alpha * t->persp_correct_Az) + (fragment.theta * t->persp_correct_Bz) + (fragment.gamma * t->persp_correct_Cz) );

                        fragment.texU = ((fragment.alpha * t->tex_u1_Ac_z) + (fragment.theta * t->tex_u2_Bc_z) + (fragment.gamma * t->tex_u3_Cc_z) ) * fragment.affineUV;
                        fragment.texV = ((fragment.alpha * t->tex_v1_Ac_z) + (fragment.theta * t->tex_v2_Bc_z) + (fragment.gamma * t->tex_v3_Cc_z) ) * fragment.affineUV;

                        if ( t->getLightmap() != nullptr ) {
                            fragment.lightU = ((fragment.alpha * t->light_u1_Ac_z) + (fragment.theta * t->light_u2_Bc_z) + (fragment.gamma * t->light_u3_Cc_z) ) * fragment.affineUV;
                            fragment.lightV = ((fragment.alpha * t->light_v1_Ac_z) + (fragment.theta * t->light_v2_Bc_z) + (fragment.gamma * t->light_v3_Cc_z) ) * fragment.affineUV;
                        }

                        processPixel( t, bufferIndex, x, y, &fragment, bilinear);
                    }
                }
            }

            // edge function increments
            w0 += A12;
            w1 += A20;
            w2 += A01;
        }

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }
}

void ComponentRender::drawWireframe(Triangle *t)
{
    Drawable::drawLine2D(Line2D(t->As.x, t->As.y, t->Bs.x, t->Bs.y), Color::green());
    Drawable::drawLine2D(Line2D(t->Bs.x, t->Bs.y, t->Cs.x, t->Cs.y), Color::green());
    Drawable::drawLine2D(Line2D(t->Cs.x, t->Cs.y, t->As.x, t->As.y), Color::green());
}