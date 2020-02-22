//
// Created by darkhead on 8/1/20.
//

#include "../../headers/Components/ComponentRender.h"
#include "../../headers/Components/ComponentWindow.h"
#include "../../headers/Objects/Decal.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/ComponentsManager.h"
#include "../../headers/Misc/Parallells.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Objects/Mesh3DAnimated.h"

ComponentRender::ComponentRender()
{
}

void ComponentRender::onStart()
{
    std::cout << "ComponentRender onStart" << std::endl;
    initTiles();
    initOpenCL();
}

void ComponentRender::preUpdate()
{
}

void ComponentRender::onUpdate()
{
    //this->getBSPTriangles();
    this->getObjectsTriangles();
    this->hiddenSurfaceRemoval();

    if (SETUP->BASED_TILE_RENDER) {
        this->handleTrianglesToTiles(visibleTriangles);

        this->drawTilesTriangles(&visibleTriangles);

        if (SETUP->DRAW_TILES_GRID) {
            this->drawTilesGrid();
        }
    } else {
        if (!SETUP->RASTERIZER_OPENCL) {
            this->drawFrameTriangles(visibleTriangles);
        } else {
            this->handleOpenCLTriangles();
        }
    }

    if (SETUP->BULLET_DEBUG_MODE) {
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->debugDrawWorld();
    }

    if (SETUP->RENDER_OBJECTS_AXIS) {
        this->drawSceneObjectsAxis();
    }

    if (SETUP->DRAW_FRUSTUM) {
        Drawable::drawFrustum(ComponentsManager::get()->getComponentCamera()->getCamera()->frustum, ComponentsManager::get()->getComponentCamera()->getCamera(), true, true, true);
    }

    if (SETUP->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis(ComponentsManager::get()->getComponentCamera()->getCamera() );
    }

    auto *mesh = dynamic_cast<Mesh3DAnimated*> ( Brakeza3D::get()->getObjectByLabel("collada") );
    if (mesh != NULL) {

        EngineSetup::getInstance()->TESTING+=Brakeza3D::get()->getDeltaTime();
        float RunningTime = EngineSetup::getInstance()->TESTING;

        if (RunningTime >= mesh->scene->mAnimations[0]->mDuration) EngineSetup::getInstance()->TESTING = 0.1;

        // Update Transforms (one per bone)
        std::vector<aiMatrix4x4> Transforms;
        mesh->BoneTransform(RunningTime, Transforms);
        for (uint i = 0 ; i < Transforms.size() ; i++) {
            std::cout << "-> Data Transform for Bone ID: " << i << std::endl;
            aiVector3t<float> scaling;
            aiVector3t<float> position;
            aiQuaterniont<float> rotation;

            Transforms[i].Decompose(scaling, rotation, position);
            std::cout << "Scaling: " << scaling.x << ", " << scaling.y << ", " << scaling.z << std::endl;
            std::cout << "Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
        }

        // Apply bone transforms and create triangle
        mesh->modelTriangles.clear();
        for (unsigned int k = 0 ; k < mesh->scene->mMeshes[0]->mNumFaces ; k++) {
            const aiFace& Face = mesh->scene->mMeshes[0]->mFaces[k];
            Vertex3D V1 = mesh->vertices.at(Face.mIndices[0]);
            Vertex3D V2 = mesh->vertices.at(Face.mIndices[1]);
            Vertex3D V3 = mesh->vertices.at(Face.mIndices[2]);

            mesh->updateForBone(V1, Face.mIndices[0], Transforms);
            mesh->updateForBone(V2, Face.mIndices[1], Transforms);
            mesh->updateForBone(V3, Face.mIndices[2], Transforms);

            //std::cout << "Assimp: Triangle: " << scene->mMeshes[i]->mNumBones << " " << Face.mIndices[0] << " " << Face.mIndices[1] << " " << Face.mIndices[2] << std::endl;

            mesh->modelTriangles.emplace_back( new Triangle(V1, V2, V3, mesh) );
        }
    }
}

void ComponentRender::postUpdate() {

}

void ComponentRender::onEnd()
{
}

void ComponentRender::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

std::vector<Triangle *> &ComponentRender::getFrameTriangles() {
    return frameTriangles;
}

std::vector<Triangle *> &ComponentRender::getVisibleTriangles() {
    return visibleTriangles;
}

void ComponentRender::getBSPTriangles()
{
    if (ComponentsManager::get()->getComponentBSP()->getBSP()->isLoaded() ) {
        ComponentsManager::get()->getComponentBSP()->getBSP()->DrawVisibleLeaf(ComponentsManager::get()->getComponentCamera()->getCamera() );

        if (SETUP->DRAW_BSP_HULLS) {
            ComponentsManager::get()->getComponentBSP()->getBSP()->DrawHulls(ComponentsManager::get()->getComponentCamera()->getCamera() );
        }
    }
}

void ComponentRender::getObjectsTriangles()
{
    std::vector<Object3D *>::iterator it;
    for ( it = getSceneObjects()->begin(); it != getSceneObjects()->end(); ) {
        Object3D *object = *(it);

        // Check for delete
        if (object->isRemoved()) {
            getSceneObjects()->erase(it);
            continue;
        } else {
            it++;
        }

        if (!object->isEnabled()) {
            continue;
        }

        // Sprite Directional 3D
        auto *oSpriteDirectional = dynamic_cast<SpriteDirectional3D*> (object);
        if (oSpriteDirectional != nullptr) {
            if (!ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->isPointInFrustum(*oSpriteDirectional->getPosition())) {
                continue;
            }

            oSpriteDirectional->updateTrianglesCoordinates( ComponentsManager::get()->getComponentCamera()->getCamera() );
            Drawable::drawBillboard(oSpriteDirectional->getBillboard(), &this->frameTriangles);

            if (SETUP->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(ComponentsManager::get()->getComponentWindow()->renderer, ComponentsManager::get()->getComponentCamera()->getCamera(), ComponentsManager::get()->getComponentWindow()->fontDefault, *oSpriteDirectional->getPosition(), SETUP->TEXT_3D_COLOR, oSpriteDirectional->getLabel());
            }
            continue;
        }

        // Sprite 3D
        auto *oSprite = dynamic_cast<Sprite3D*> (object);
        if (oSprite != nullptr) {
            oSprite->updateTrianglesCoordinatesAndTexture( ComponentsManager::get()->getComponentCamera()->getCamera() );
            Drawable::drawBillboard(oSprite->getBillboard(), &frameTriangles);

            if (SETUP->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(ComponentsManager::get()->getComponentWindow()->renderer, ComponentsManager::get()->getComponentCamera()->getCamera(), ComponentsManager::get()->getComponentWindow()->fontDefault, *oSprite->getPosition(), SETUP->TEXT_3D_COLOR, oSprite->getLabel());
            }
            continue;
        }

        auto *oBillboardBody = dynamic_cast<BillboardBody*> (object);
        if (oBillboardBody != nullptr) {
            oBillboardBody->updateTrianglesCoordinatesAndTexture( ComponentsManager::get()->getComponentCamera()->getCamera() );
            Drawable::drawBillboard(oBillboardBody, &frameTriangles);
            continue;
        }

        auto *oMesh = dynamic_cast<Mesh3D*> (object);
        if (oMesh != nullptr) {
            oMesh->draw( &this->frameTriangles) ;
            if (SETUP->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(ComponentsManager::get()->getComponentWindow()->renderer, ComponentsManager::get()->getComponentCamera()->getCamera(), ComponentsManager::get()->getComponentWindow()->fontDefault, *oMesh->getPosition(), SETUP->TEXT_3D_COLOR, oMesh->getLabel());
            }

            if (SETUP->DRAW_DECAL_WIREFRAMES) {
                Decal *oDecal = dynamic_cast<Decal*> (oMesh);
                if (oDecal != NULL) {
                    oDecal->cube->draw( &this->frameTriangles );
                }
            }
            continue;
        }
    }
}

void ComponentRender::hiddenSurfaceRemoval()
{
    visibleTriangles.clear();

    Camera3D *cam = ComponentsManager::get()->getComponentCamera()->getCamera();

    for (int i = 0; i < frameTriangles.size() ; i++) {

        frameTriangles[i]->updateObjectSpace();
        frameTriangles[i]->updateNormal();

        // back face culling (needs objectSpace)
        if (frameTriangles[i]->isBackFaceCulling( cam->getPosition() )) {
            continue;
        }

        // Clipping (needs objectSpace)
        if (frameTriangles[i]->testForClipping( cam->frustum->planes,SETUP->LEFT_PLANE,SETUP->BOTTOM_PLANE )) {
            frameTriangles[i]->clipping(
                    cam,
                    cam->frustum->planes,
                    SETUP->LEFT_PLANE,
                    SETUP->BOTTOM_PLANE,
                    frameTriangles[i]->parent,
                    visibleTriangles,
                    frameTriangles[i]->isBSP
            );
            continue;
        }

        // Frustum Culling (needs objectSpace)
        if (!cam->frustum->isPointInFrustum(frameTriangles[i]->Ao) &&
            !cam->frustum->isPointInFrustum(frameTriangles[i]->Bo) &&
            !cam->frustum->isPointInFrustum(frameTriangles[i]->Co)
        ) {
            continue;
        }

        // Triangle precached data
        // Estas operaciones las hacemos después de descartar triángulos
        // para optimización en el rasterizador por software
        frameTriangles[i]->updateCameraSpace( cam );
        frameTriangles[i]->updateNDCSpace( cam );
        frameTriangles[i]->updateScreenSpace();
        frameTriangles[i]->updateBoundingBox();
        frameTriangles[i]->updateFullArea();
        frameTriangles[i]->updateUVCache();

        if (SETUP->RASTERIZER_OPENCL) {
            BUFFERS->addOCLTriangle(frameTriangles[i]->getOpenCL());
        }

        if (this->frameTriangles[i]->fullArea < SETUP->MIN_TRIANGLE_AREA) {
            continue;
        }

        this->frameTriangles[i]->drawed = false;
        this->visibleTriangles.emplace_back(frameTriangles[i]);
    }

    if (SETUP->DEBUG_RENDER_INFO) {
        Logging::getInstance()->Log("[DEBUG_RENDER_INFO] frameTriangles: " + std::to_string(frameTriangles.size()) + ", numVisibleTriangles: " + std::to_string(visibleTriangles.size()));
    }

    frameTriangles.clear();
}

void ComponentRender::handleTrianglesToTiles(std::vector<Triangle*> &visibleTriangles)
{
    for (int i = 0; i < this->numTiles; i++) {
        this->tiles[i].numTriangles = 0;
    }

    for (int i = 0; i < visibleTriangles.size(); i++) {
        int tileStartX = std::max((float) (visibleTriangles[i]->minX / this->sizeTileWidth), 0.0f);
        int tileEndX = std::min((float) (visibleTriangles[i]->maxX / this->sizeTileWidth), (float) this->tilesWidth - 1);

        int tileStartY = std::max((float) (visibleTriangles[i]->minY / this->sizeTileHeight), 0.0f);
        int tileEndY = std::min((float) (visibleTriangles[i]->maxY / this->sizeTileHeight), (float) this->tilesHeight - 1);

        for (int y = tileStartY; y <= tileEndY; y++) {
            for (int x = tileStartX; x <= tileEndX; x++) {
                int tileOffset = y * tilesWidth + x;
                this->tiles[tileOffset].triangleIds[this->tiles[tileOffset].numTriangles] = i;
                this->tiles[tileOffset].numTriangles++;
            }
        }
    }
}

void ComponentRender::handleOpenCLTriangles()
{
    int numTriangles = BUFFERS->numOCLTriangles;

    opencl_buffer_triangles = clCreateBuffer(contextCPU, CL_MEM_READ_ONLY, numTriangles * sizeof(OCLTriangle), NULL, &ret);
    clEnqueueWriteBuffer(command_queue_rasterizer, opencl_buffer_triangles, CL_TRUE, 0, numTriangles * sizeof(OCLTriangle), BUFFERS->OCLTrianglesBuffer, 0, NULL, NULL);

    clSetKernelArg(processAllTriangles, 0, sizeof(cl_mem), (void *)&opencl_buffer_triangles);
    clSetKernelArg(processAllTriangles, 1, sizeof(int), &SETUP->screenWidth);
    clSetKernelArg(processAllTriangles, 2, sizeof(cl_mem), (void *)&opencl_buffer_video);
    clSetKernelArg(processAllTriangles, 3, sizeof(cl_mem), (void *)&opencl_buffer_depth);

    size_t global_item_size = numTriangles; // Process the entire lists
    size_t local_item_size = 1;          // Divide work items into groups of 64

    ret = clEnqueueNDRangeKernel(command_queue_rasterizer, processAllTriangles, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        Logging::getInstance()->getInstance()->Log( "Error processAllTriangles: " + std::to_string(ret) );

        char buffer[1024];
        clGetProgramBuildInfo(programCPU, device_cpu_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->getInstance()->Log( buffer );
        }
    }
    clFinish(command_queue_rasterizer);
    BUFFERS->numOCLTriangles = 0;
}

void ComponentRender::drawTilesGrid()
{
    for (int j = 0 ; j < (this->tilesWidth*this->tilesHeight) ; j++) {
        Tile *t = &this->tiles[j];
        Uint32 color = Color::white();

        if (t->numTriangles > 0) {
            color = Color::red();
        }

        Line2D top = Line2D(t->start_x, t->start_y, t->start_x+sizeTileWidth, t->start_y);
        Line2D left = Line2D(t->start_x, t->start_y, t->start_x, t->start_y+sizeTileHeight);
        Drawable::drawLine2D(top, color);
        Drawable::drawLine2D(left, color);
    }
}

void ComponentRender::drawFrameTriangles(std::vector<Triangle*> &visibleTriangles)
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
        Drawable::drawVertex(t->Co, CC, Color::red());
        Drawable::drawVertex(t->Bo, CC, Color::green());
        Drawable::drawVertex(t->Co, CC, Color::blue());
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

    Fragment *fragment = new Fragment();

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
                    fragment->texU   = (fragment->alpha * (t->tex_u1_Ac_z) + fragment->theta * (t->tex_u2_Bc_z) + fragment->gamma * (t->tex_u3_Cc_z) ) * fragment->affineUV;
                    fragment->texV   = (fragment->alpha * (t->tex_v1_Ac_z) + fragment->theta * (t->tex_v2_Bc_z) + fragment->gamma * (t->tex_v3_Cc_z) ) * fragment->affineUV;

                    fragment->lightU = (fragment->alpha * (t->light_u1_Ac_z) + fragment->theta * (t->light_u2_Bc_z) + fragment->gamma * (t->light_u3_Cc_z) ) * fragment->affineUV;
                    fragment->lightV = (fragment->alpha * (t->light_v1_Ac_z) + fragment->theta * (t->light_v2_Bc_z) + fragment->gamma * (t->light_v3_Cc_z) ) * fragment->affineUV;
                    
                    this->processPixel(
                            t,
                            bufferIndex,
                            x, y, fragment
                    );
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

void ComponentRender::processPixel(Triangle *t, int bufferIndex, const int x, const int y, Fragment *fragment)
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

        if ( t->parent->isDecal() ) {
            if ((fragment->texU < 0 || fragment->texU > 1) || (fragment->texV < 0 || fragment->texV > 1) ) {
                return;
            }
        }

        t->processPixelTexture(pixelColor, fragment->texU, fragment->texV );

        Uint8 red, green, blue, alpha;
        SDL_GetRGBA(pixelColor, t->texture->getSurface(t->lod)->format, &red, &green, &blue, &alpha);

        if (alpha == 0) {
            return;
        } else {
            pixelColor = Tools::alphaBlend( BUFFERS->getVideoBuffer( x, y ), pixelColor, alpha );
        }

        if (!t->parent->isDecal() && t->getLightmap()->isLightMapped() && SETUP->ENABLE_LIGHTMAPPING) {
            t->processPixelLightmap(pixelColor, fragment->lightU, fragment->lightV);
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
        if ( getSceneObjects()->operator[](i)->isEnabled() ) {
            Drawable::drawObject3DAxis(getSceneObjects()->operator[](i), ComponentsManager::get()->getComponentCamera()->getCamera(), true, true, true);
        }
    }
}

void ComponentRender::initOpenCL()
{
    platform_id   = NULL;
    device_cpu_id = NULL;

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms) ;
    if (ret != CL_SUCCESS) {
        printf("Unable to get platform_id");
    }

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_cpu_id, &ret_num_devices);
    if (ret != CL_SUCCESS) {
        printf("Unable to get device_cpu_id");
    }

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_gpu_id, &ret_num_devices);
    if (ret != CL_SUCCESS) {
        printf("Unable to get device_gpu_id");
    }

    cl_context_properties properties[3];
    properties[0] = CL_CONTEXT_PLATFORM;
    properties[1] = (cl_context_properties) platform_id;
    properties[2] = 0;

    // Create an OpenCL context
    contextCPU = clCreateContext(properties, 1, &device_cpu_id, NULL, NULL, &ret);
    contextGPU = clCreateContext(properties, 1, &device_gpu_id, NULL, NULL, &ret);

    command_queue_rasterizer = clCreateCommandQueue(contextCPU, device_cpu_id, NULL, &ret);

    // Load the kernel source code into the array source_str
    size_t source_size;
    char *source_str = Tools::readFile("../opencl.cl", source_size);

    // Create a program from the kernel source
    programCPU = clCreateProgramWithSource(contextCPU, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    programGPU = clCreateProgramWithSource(contextGPU, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    clBuildProgram(programCPU, 1, &device_cpu_id, NULL, NULL, NULL);
    clBuildProgram(programGPU, 1, &device_gpu_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    processAllTriangles = clCreateKernel(programCPU, "rasterizerFrameTrianglesKernel", &ret);

    opencl_buffer_depth = clCreateBuffer(contextCPU, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, BUFFERS->sizeBuffers * sizeof(float), BUFFERS->depthBuffer, &ret);
    opencl_buffer_video = clCreateBuffer(contextCPU, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, BUFFERS->sizeBuffers * sizeof(unsigned int), BUFFERS->videoBuffer, &ret);

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
    for (int j = 0 ; j < this->tiles[i].numTriangles ; j++) {
        int triangleId = this->tiles[i].triangleIds[j];
        Tile *t = &this->tiles[i];
        Triangle *tr = visibleTriangles[triangleId];

        this->softwareRasterizerForTile(
                tr,
                t->start_x,
                t->start_y,
                t->start_x+sizeTileWidth,
                t->start_y+sizeTileHeight
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

    Fragment *fragment = new Fragment();

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

                const int bufferIndex = ( y * SETUP->screenWidth ) + x;

                if ( fragment->depth < BUFFERS->getDepthBuffer( bufferIndex )) {
                    fragment->affineUV = 1 / ((fragment->alpha * t->persp_correct_Az) + (fragment->theta * t->persp_correct_Bz) + (fragment->gamma * t->persp_correct_Cz) );
                    fragment->texU   = ((fragment->alpha * t->tex_u1_Ac_z) + (fragment->theta * t->tex_u2_Bc_z) + (fragment->gamma * t->tex_u3_Cc_z) ) * fragment->affineUV;
                    fragment->texV   = ((fragment->alpha * t->tex_v1_Ac_z) + (fragment->theta * t->tex_v2_Bc_z) + (fragment->gamma * t->tex_v3_Cc_z) ) * fragment->affineUV;

                    fragment->lightU = ((fragment->alpha * t->light_u1_Ac_z) + (fragment->theta * t->light_u2_Bc_z) + (fragment->gamma * t->light_u3_Cc_z) ) * fragment->affineUV;
                    fragment->lightV = ((fragment->alpha * t->light_v1_Ac_z) + (fragment->theta * t->light_v2_Bc_z) + (fragment->gamma * t->light_v3_Cc_z) ) * fragment->affineUV;

                    if (! ((x < minTileX || x > maxTileX) || (y < minTileY || y > maxTileY )) ) {
                        processPixel( t, bufferIndex, x, y, fragment);
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
    Drawable::drawLine2D(Line2D(t->As.x, t->As.y, t->Bs.x, t->Bs.y), Color::red());
    Drawable::drawLine2D(Line2D(t->Bs.x, t->Bs.y, t->Cs.x, t->Cs.y), Color::green());
    Drawable::drawLine2D(Line2D(t->Cs.x, t->Cs.y, t->As.x, t->As.y), Color::blue());
}
