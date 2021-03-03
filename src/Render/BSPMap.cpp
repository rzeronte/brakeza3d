
#include <fcntl.h> // open
#include <stdio.h> // printf
#include <stdlib.h> // malloc, free
#include <sys/stat.h> // stat
#include <unistd.h> // lseek, read, close
#include <cstring>
#include "../../headers/Render/BSPMap.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Game/NPCEnemyBody.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Physics/Sprite3DBody.h"
#include "../../headers/Physics/BillboardBody.h"
#include "../../headers/Game/ItemWeaponGhost.h"
#include "../../headers/Game/ItemHealthGhost.h"
#include "../../headers/Game/ItemAmmoGhost.h"

BSPMap::BSPMap(): frameTriangles(nullptr)
{
    setDecal(false);
}

void BSPMap::init(Camera3D *cam)
{
    bsp = NULL;
    this->camera = cam;
    surfacePrimitives = NULL;
    textureObjNames = NULL;
    visibleSurfaces = NULL;

    if (surfacePrimitives) delete[] surfacePrimitives;
    if (textureObjNames) delete[] textureObjNames;
    if (visibleSurfaces) delete[] visibleSurfaces;

    this->textures  = new Texture[MAX_MAP_TEXTURES];
    this->lightmaps = new Texture[MAX_BSP_TRIANGLES];

    // La posición de un mapa ha de ser fija ya que la información cacheada está referida a esta referencia
    // La rotación x = 180, y = 90, es para ajustar los ejes con los que quake1 trabaja. (z la vertical)
    setPosition( Vertex3D(0, 0, 0) );
    setRotation( M3(180, 90, 0) );

    this->surface_triangles = new surface_triangles_t[MAX_BSP_TRIANGLES];
    this->surface_lightmaps = new lightmap_t[MAX_BSP_TRIANGLES];

    this->entities = new entity_t[MAX_BSP_ENTITIES];
    this->n_entities = 0;
    this->setLabel("BSPMap");
}

bool BSPMap::Initialize(const char *bspFilename, const char *paletteFilename, Camera3D *cam)
{
    this->init(cam);

    std::string bspFilename_str     = std::string(EngineSetup::getInstance()->MAPS_FOLDER + bspFilename);
    std::string paletteFilename_str = std::string(EngineSetup::getInstance()->MAPS_FOLDER + paletteFilename);

    if (!LoadBSP(bspFilename_str.c_str())) {
        printf("[ERROR] Map::Initialize() Error loading bsp file\n");
        return false;
    }

    if (!LoadPalette(paletteFilename_str.c_str())) {
        printf("[ERROR] Map::Initialize() Error loading palette\n");
        return false;
    }

    this->InitializeSurfaces();
    this->InitializeTextures();
    this->InitializeLightmaps();
    this->InitializeTriangles();
    this->bindTrianglesLightmaps();
    this->InitializeEntities();                // necesario para getStartMapPosition
    //this->createBulletPhysicsShape();
    this->setLoaded(true);

    return true;
}

int BSPMap::LoadFile(const char *filename, void **bufferptr)
{
    struct stat st;
    void *buffer;
    int length;

    int f = open(filename, O_RDONLY);
    if (f == -1) {
        return 0;
    }

    stat(filename, &st);
    length = st.st_size;
    lseek(f, 0, SEEK_SET);
    buffer = malloc(length);
    read(f, buffer, length);
    close(f);

    *bufferptr = buffer;
    return length;
}

bool BSPMap::LoadPalette(const char *filename)
{
    unsigned char *tempPal;

    if (!LoadFile(filename, (void **)&tempPal)) {
        return false;
    }

    for (int i = 0; i < 256; i++) {
        unsigned int b = tempPal[i * 3 + 0];
        unsigned int g = tempPal[i * 3 + 1];
        unsigned int r = tempPal[i * 3 + 2];
        palette[i] = (r) | (g << 8) | (b << 16);
    }

    free(tempPal);
    return true;
}

bool BSPMap::LoadBSP(const char *filename)
{
    if (!LoadFile(filename, (void **)&bsp)) {
        return false;
    }

    header = (dheader_t *)bsp;
    if (header->version != BSP_VERSION) {
        printf("[ERROR] Map::LoadBSP() BSP file version mismatch!: %h\n", header->version);
        //return false;
    }

    return true;
}

// Calculate primitive surfaces
bool BSPMap::InitializeSurfaces(void)
{
    // Allocate memory for the visible surfaces array
    visibleSurfaces = new int [this->getNumSurfaceLists()];
    allSurfaces = new int [this->getNumSurfaceLists()];

    // Calculate max number of edges per surface
    numMaxEdgesPerSurface = 0;
    for (int i = 0; i < this->getNumSurfaces(); i++) {
        if (numMaxEdgesPerSurface < this->getNumEdges(i)) {
            numMaxEdgesPerSurface = this->getNumEdges(i);
        }
    }

    // Allocate memory for the surface primitive array
    surfacePrimitives = new primdesc_t [this->getNumSurfaces() * numMaxEdgesPerSurface];

    // Loop through all the surfaces to fetch the vertices and calculate it's texture coords
    for (int i = 0; i < this->getNumSurfaces(); i++) {
        int numEdges = this->getNumEdges(i);

        // Get a pointer to texinfo for this surface
        texinfo_t *textureInfo = this->getTextureInfo(i);

        // Get a pointer to the surface's miptextures
        miptex_t *mipTexture = this->getMipTexture(textureInfo->texid);

        // Point to a surface primitive array
        primdesc_t *primitives = &surfacePrimitives[i * numMaxEdgesPerSurface];

        for (int j = 0; j < numEdges; j++, primitives++) {
            // Get an edge id from the surface. Fetch the correct edge by using the id in the Edge List.
            // The winding is backwards!
            int edgeId = this->getEdgeList(this->getSurface(i)->firstedge + (numEdges - 1 - j));

            // Fetch one of the egde's vertex
            int vertexId = ((edgeId >= 0) ? this->getEdge(edgeId)->startVertex : this->getEdge(-edgeId)->endVertex);

            // Store the vertex in the primitive array
            vec3_t *vertex = this->getVertex(vertexId);
            primitives->v[0] = ((float *)vertex)[0]*scale;
            primitives->v[1] = ((float *)vertex)[1]*scale;
            primitives->v[2] = ((float *)vertex)[2]*scale;

            // Calculate the vertex's texture coords and store it in the primitive array
            //primitives->t[1] = (CalculateDistance(textureInfo->snrm, primitives->v) + textureInfo->soff) / mipTexture->width;
            //primitives->t[0] = (CalculateDistance(textureInfo->tnrm, primitives->v) + textureInfo->toff) / mipTexture->height;

            primitives->t[0] = (CalculateDistance(textureInfo->snrm, *vertex) + textureInfo->soff);
            primitives->t[1] = (CalculateDistance(textureInfo->tnrm, *vertex) + textureInfo->toff);

        }
    }

    return true;
}

// Build mipmaps from the textures, create texture objects and assign mipmaps to the created objects
bool BSPMap::InitializeTextures(void)
{
    int numAnimTextues = 0;
    int numSkyTextures = 0;

    // Loop through all textures to count animations and sky textures
    for (int i = 0; i < this->getNumTextures(); i++) {
        // Point to the stored mipmaps
        miptex_t *mipTexture = this->getMipTexture(i);
        if (strncmp(mipTexture->name, "*", 1) == 0) {
            numAnimTextues++;
        } else if (strncmp(mipTexture->name, "sky", 3) == 0) {
            numSkyTextures++;
        }
    }
    //printf("%d liquid textures, %d sky textures\n", numAnimTextues, numSkyTextures);

    // Calculate the total number of textures
    int numTotalTextures = this->getNumTextures() + numAnimTextues * (ANIM_TEX_FRAMES - 1) + numSkyTextures * 2;

    // Allocate memory for texture names and an array to store the textures we are using
    textureObjNames = new unsigned int [numTotalTextures];
    textures = new Texture[numTotalTextures];

    // Loop through all texture objects to associate them with a texture and calculate mipmaps from it
    for (int i = 0; i < this->getNumTextures(); i++) {
        miptex_t *mipTexture = this->getMipTexture(i);

        if (strlen(mipTexture->name) == 0) continue;

        int width  = mipTexture->width;
        int height = mipTexture->height;

        textures[i] = Texture(width, height);

        textures[i].setFilename( std::string(mipTexture->name) );

        if (strncmp(mipTexture->name, "*", 1) == 0) {
            textures[i].liquid = true;
        }
        if (strncmp(mipTexture->name, "+", 1) == 0) {
            textures[i].animated = true;
        }
        textures[i].setMipMapped(true);

        Logging::getInstance()->Log(
                "InitializeTextures: "+ std::string(mipTexture->name) +
                " | animated: " + std::to_string(textures[i].animated) +
                " | liquid: " + std::to_string(textures[i].liquid) +
                " | textureId: " + std::to_string(i) + " '" +
                " | w: " + std::to_string(textures[i].getSurface(1)->w) +
                " | h: " + std::to_string(textures[i].getSurface(1)->h),
                "Q1MAP"
        );

        // Guardamos la textura, con sus mip mappings
        for (int mip_m = 1; mip_m <= 8; mip_m = 2*mip_m) { // 1, 2, 4, 8

            int width_t  = mipTexture->width / mip_m;
            int height_t = mipTexture->height / mip_m;

            // Allocate memory for the texture which will be created
            unsigned int *texture = new unsigned int [width_t * height_t];

            // Point to the raw 8-bit texture data
            unsigned char *rawTexture = this->getRawTexture(i, mip_m);

            // Create a texture to assign with the texture object
            for (int x = 0; x < width_t; x++) {
                for (int y = 0; y < height_t; y++) {
                    unsigned int color = this->palette[rawTexture[x + y * width_t]];
                    texture[x + y * width_t] = color;
                }
            }

            textures[i].loadFromRaw(texture, width_t, height_t, mip_m);
            delete texture;
        }
    }

    // No podemos hacerlo en el bucle superior, pq no estarían rellenados todos los "animations" en cada iteracción
    // Así nos aseguramos de que todos los "+Xname" está cargado en textures[]
    for (int i = 0; i < this->getNumTextures(); i++) {
        if (textures[i].animated) {
            textures[i].numAnimatedFrames = getTextureAnimatedFrames(textures[i].getFilename());
        }
    }

    return true;
}

bool BSPMap::InitializeTriangles()
{
    // Creamos triángulos
    for (int i = 0; i < this->getNumSurfaces(); i++) {

        surface_t *s = this->getSurface(i);
        texinfo_t *tInfo =  this->getTextureInfo( i );

        int start_number_triangle = this->model_triangles.size();
        this->createTrianglesForSurface(i);

        // Hayamos el número de triángulos creados para esta superficie
        int end_number_triangle = this->model_triangles.size();
        int surface_num_triangles = end_number_triangle - start_number_triangle ;
        //Logging::getInstance()->Log("Surface: " + std::to_string(i) + ", NumTriangles: " + std::to_string(surface_num_triangles) +", startOffset: " + std::to_string(start_number_triangle) + ", endOffset: " + std::to_string(end_number_triangle), "");

        // Creamos el registro que relaciona una surface con sus triángulos
        this->surface_triangles[i].num = surface_num_triangles;
        this->surface_triangles[i].offset = start_number_triangle;
        this->surface_triangles[i].flags = tInfo->flag;

    }

    this->model_triangles[0]->A.consoleInfo("A", false);
    Logging::getInstance()->Log("BSP Num Triangles: " + std::to_string(this->model_triangles.size()), "");
    Logging::getInstance()->Log("BSP Num Surfaces: " + std::to_string(this->getNumSurfaces()), "");
}

bool BSPMap::InitializeLightmaps()
{
    for (int surfaceId = 0; surfaceId < this->getNumSurfaces(); surfaceId++) {
        surface_t *surf = this->getSurface(surfaceId);
        lightmap_t *lt = &surface_lightmaps[surfaceId];
        CalcSurfaceExtents(surfaceId, lt);

        if (lt->offset != -1) {
            this->lightmaps[surfaceId].setLightMapped(true);

            int smax = lt->width;
            int tmax = lt->height;
            int lightMapSize = lt->width * lt->height;

            unsigned int *texture  = new unsigned int [lightMapSize];
            unsigned int *texture2 = new unsigned int [lightMapSize];
            unsigned int *texture3 = new unsigned int [lightMapSize];
            unsigned int *texture4 = new unsigned int [lightMapSize];

            // Clear block
            for (int c = 0 ; c < lightMapSize; c++) {
                texture[c] = 0;
                texture2[c] = 0;
                texture3[c] = 0;
                texture4[c] = 0;
            }

            // Itering over lightmaps for create unique lightmap (max 4 lights)
            unsigned char *lm_data = getLightmap(lt->offset);
            for (int maps = 0 ; maps < MAXLIGHTMAPS && surf->lightstyle[maps] != 255 ; maps++) {

                for (int y = 0 ; y < tmax; y++) {
                    for (int x = 0 ; x < smax; x++) {
                        int i = x + y * smax;
                        unsigned char lumen = *lm_data;
                        Uint32 c = (Uint32) Tools::createRGB(lumen/5, lumen/5, lumen/5);
                        switch(maps) {
                            case 0:
                                texture[i] += c;
                                break;
                            case 1:
                                texture2[i] += c;
                                break;
                            case 2:
                                texture3[i] += c;
                                break;
                            case 3:
                                texture4[i] += c;
                                break;
                        }
                        lm_data++;
                    }
                }

                switch(maps) {
                    case 0:
                        this->lightmaps[surfaceId].loadLightmapFromRaw(maps, texture, smax, tmax);
                        break;
                    case 1:
                        this->lightmaps[surfaceId].loadLightmapFromRaw(maps, texture2, smax, tmax);
                        break;
                    case 2:
                        this->lightmaps[surfaceId].loadLightmapFromRaw(maps, texture3, smax, tmax);
                        break;
                    case 3:
                        this->lightmaps[surfaceId].loadLightmapFromRaw(maps, texture4, smax, tmax);
                        break;
                }
                this->lightmaps[surfaceId].numLightmaps++;
            }

            /*Logging::getInstance()->Log(
                    "InitializeLightmaps: surfaceId:  " + std::to_string(surfaceId) +
                    ", numLightmaps: " + std::to_string(this->lightmaps[surfaceId].numLightmaps) +
                    ", lightmap w: " + std::to_string(smax) +
                    ", height: " + std::to_string(tmax) +
                    ", mins[0]: " + std::to_string(lt->mins[0]) +
                    ", mins[1]: " + std::to_string(lt->mins[1]) +
                    ", maxs[0]: " + std::to_string(lt->maxs[0]) +
                    ", maxs[1]: " + std::to_string(lt->maxs[1])
            );*/

        } else {
            this->lightmaps[surfaceId].setLightMapped(false);
            //Logging::getInstance()->Log("InitializeLightmaps: surfaceId:  " + std::to_string(surfaceId) + ",  No lightmap");
        }
    }
}

void BSPMap::InitializeEntities()
{
    char *e = getEntities();
    this->parseEntities(e);
}

void BSPMap::bindTrianglesLightmaps()
{
    for (int surfaceId = 0; surfaceId < this->getNumSurfaces(); surfaceId++) {

        int num    = this->surface_triangles[surfaceId].num;
        int offset = this->surface_triangles[surfaceId].offset;

        for (int j = offset ; j < offset+num ; j++) {

            this->model_triangles[j]->surfaceBSPIndex = surfaceId;
            this->model_triangles[j]->isBSP = true;

            if (lightmaps[surfaceId].isLightMapped()) {
                lightmap_t *lt = &surface_lightmaps[surfaceId];

                this->model_triangles[j]->getLightmap()->mins[0] = lt->mins[0];
                this->model_triangles[j]->getLightmap()->mins[1] = lt->mins[1];
                this->model_triangles[j]->getLightmap()->maxs[0] = lt->maxs[0];
                this->model_triangles[j]->getLightmap()->maxs[1] = lt->maxs[1];

                this->model_triangles[j]->getLightmap()->minu[0] = lt->minu[0];
                this->model_triangles[j]->getLightmap()->minu[1] = lt->minu[1];
                this->model_triangles[j]->getLightmap()->maxv[0] = lt->maxv[0];
                this->model_triangles[j]->getLightmap()->maxv[1] = lt->maxv[1];

                this->model_triangles[j]->getLightmap()->extents[1] = abs(lt->maxs[1] - lt->mins[1]);
                this->model_triangles[j]->getLightmap()->extents[0] = abs(lt->maxs[0] - lt->mins[0]);

                this->model_triangles[j]->setLightmap(&lightmaps[surfaceId]);
            }
        }
    }
}

void BSPMap::createTrianglesForSurface(int surface)
{
    // Get the surface primitive
    primdesc_t *primitives = &surfacePrimitives[numMaxEdgesPerSurface * surface];

    Vertex3D vertices[30];  // Max face vertex
    int num_vertices = 0;

    // Loop through all vertices of the primitive
    int num_edges = this->getNumEdges(surface);
    for (int i = 0; i < num_edges ; i++, primitives++) {
        // IMPORTANTE: Las coordenadas del mapa de quake1 son distintas (la Z es la vertical) por eso usamos el orden (y, z, x)
        Vertex3D v = Vertex3D( primitives->v[1], primitives->v[2], primitives->v[0] );

        v.v = primitives->t[0];
        v.u = primitives->t[1];

        v = Transforms::objectToLocal(v, this);

        vertices[num_vertices] = v;
        num_vertices++;
    }

    triangulateQuakeSurface(vertices, num_vertices, surface);
}

bool BSPMap::triangulateQuakeSurface(Vertex3D vertices[], int num_vertices, int surface)
{
    Vertex3D normal;

    // Normal al triángulo
    for (int i = 0, j = 1; i<num_vertices; i++, j++) {
        if (j == num_vertices) j = 0;
        normal.x += (((vertices[i].z) + (vertices[j].z)) * ((vertices[j].y) - (vertices[i].y)));
        normal.y += (((vertices[i].x) + (vertices[j].x)) * ((vertices[j].z) - (vertices[i].z)));
        normal.z += (((vertices[i].y) + (vertices[j].y)) * ((vertices[j].x) - (vertices[i].x)));
    }

    normal = normal.getInverse();
    surface_t *surf = this->getSurface(surface);

    Maths::TriangulatePolygon(
            num_vertices,
            vertices,
            normal,
            model_triangles,
            this,
            &textures[this->getTextureInfo(surface)->texid],
            &lightmaps[surface],
            false,
            true,
            surf->lightstyle
    );

    return true;
}

void BSPMap::CalcSurfaceExtents (int surface, lightmap_t* l)
{
    float	mins[2], maxs[2];
    int		bmins[2], bmaxs[2];

    mins[0] = mins[1] = 999999;
    maxs[0] = maxs[1] = -99999;
    short extents[2];

    surface_t *surf = this->getSurface(surface);

    int numEdges = surf->numedge;
    texinfo_t *textureInfo = this->getTextureInfo(surface);
    miptex_t *mipTexture = this->getMipTexture(textureInfo->texid);

    float min_u = 999999, min_v = 999999;
    float max_u = -999999, max_v = -999999;

    for (int j = 0; j < numEdges; j++) {
        // Get an edge id from the surface. Fetch the correct edge by using the id in the Edge List.
        // The winding is backwards!
        int edgeId = this->getEdgeList(surf->firstedge + (numEdges - 1 - j));

        // Fetch one of the egde's vertex
        int vertexId = ((edgeId >= 0) ? this->getEdge(edgeId)->startVertex : this->getEdge(-edgeId)->endVertex);

        // Store the vertex in the primitive array
        vec3_t *vertex = this->getVertex(vertexId);
        vec3_t vertex_fixed;
        vertex_fixed[0] = ((float *)vertex)[0];
        vertex_fixed[1] = ((float *)vertex)[1];
        vertex_fixed[2] = ((float *)vertex)[2];

        float u = (CalculateDistance(textureInfo->tnrm, vertex_fixed) + textureInfo->toff) / mipTexture->width;
        float v = (CalculateDistance(textureInfo->snrm, vertex_fixed) + textureInfo->soff) / mipTexture->height;


        min_u = std::min((u), min_u);
        min_v = std::min((v), min_v);

        max_u = std::max((u), max_u);
        max_v = std::max((v), max_v);

        l->minu[0] = min_u;
        l->minu[1] = min_v;

        l->maxv[0] = max_u;
        l->maxv[1] = max_v;

        for (int k=0 ;  k < 2 ; k++) {
            float val = 0;
            if (k == 0)  {
                val = CalculateDistance(textureInfo->snrm, vertex_fixed);
            }

            if (k == 1) {
                val = CalculateDistance(textureInfo->tnrm, vertex_fixed);
            }

            if (val < mins[k]) {
                mins[k] = val;
            }
            if (val > maxs[k]) {
                maxs[k] = val;
            }
        }
    }

    for (int i = 0 ; i < 2 ; i++) {
        bmins[i] = floor(mins[i]/16);
        bmaxs[i] = ceil(maxs[i]/16);

        extents[i] = (bmaxs[i] - bmins[i]) * 16;

        if ( extents[i] > 512  ) {
            //Logging::get()->Log("Error lightmap surface: " + std::to_string(extents[i]), "");
        }
    }

    l->mins[0] = (mins[0]);
    l->mins[1] = (mins[1]);

    l->maxs[0] = (maxs[0]);
    l->maxs[1] = (maxs[1]);

    l->width  = (extents[0]>>4)+1;
    l->height = (extents[1]>>4)+1;

    l->offset = surf->lightmap;
}

// Return the dotproduct of two vectors
float BSPMap::CalculateDistance(vec3_t a, vec3_t b)
{
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void BSPMap::DrawSurfaceTriangles(int surface)
{
    const int offset = this->surface_triangles[surface].offset;
    const int num = this->surface_triangles[surface].num;

    for (int i = offset; i < offset+num; i++) {
        //this->model_triangles[i]->updateTextureAnimated();
        //this->model_triangles[i]->updateLightmapFrame();
        this->frameTriangles->emplace_back( this->model_triangles[i] );
    }
}

void BSPMap::createBulletPhysicsShape()
{
    int numVisibleSurfaces = 0;

    model_t *bspHull = getModel(0);
    int firstSurface = bspHull->firstsurf;
    int lastSurface = firstSurface + bspHull->numsurf;
    for (int k = firstSurface; k < lastSurface; k++) {
        allSurfaces[numVisibleSurfaces++] = this->getSurfaceList(k);
    }

    this->numAllSurfaces = numVisibleSurfaces;
    this->bspBtMesh = new btTriangleMesh();

    for (int surface = 0; surface < numAllSurfaces; surface++) {

        if (surface_triangles[surface].flags) continue; // Ignore physics for water/lava

        const int offset = surface_triangles[surface].offset;
        const int num = surface_triangles[surface].num;

        for (int i = offset; i < offset+num; i++){
            //model_triangles[i].drawWireframe();
            this->model_triangles[i]->updateFullVertexSpaces(this->camera);
            btVector3 a = btVector3( this->model_triangles[i]->Ao.x, this->model_triangles[i]->Ao.y, this->model_triangles[i]->Ao.z );
            btVector3 b = btVector3( this->model_triangles[i]->Bo.x, this->model_triangles[i]->Bo.y, this->model_triangles[i]->Bo.z );
            btVector3 c = btVector3( this->model_triangles[i]->Co.x, this->model_triangles[i]->Co.y, this->model_triangles[i]->Co.z );

            this->bspBtMesh.addTriangle(a, b, c, false);
        }
    }

    btTransform trans;
    trans.setIdentity();
    btVector3 localInertia(0, 0, 0);

    this->bspBtShape = new btBvhTriangleMeshShape(&bspBtMesh, true, true);
    this->bspBtShape->calculateLocalInertia(0, localInertia);

    Vertex3D pos = this->getPosition();
    trans.setOrigin(btVector3(pos.x, pos.y, pos.z));

    this->motionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo info(0.0f, motionState, bspBtShape, localInertia) ;
    this->bspRigidBody = new btRigidBody(info);

    this->bspRigidBody->setContactProcessingThreshold(BT_LARGE_FLOAT);
    this->bspRigidBody->setCcdMotionThreshold(.5);
    this->bspRigidBody->setCcdSweptSphereRadius(.5);
    this->bspRigidBody->setUserPointer(this);
    Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()->addRigidBody(this->bspRigidBody);
}

void BSPMap::setVisibleSet(dleaf_t *pLeaf)
{
    int numVisibleSurfaces = 0;
    unsigned char * visibilityList = this->getVisibilityList(pLeaf->vislist);

    /*
        // Suppose Leaf is the leaf the player is in.
        v = Leaf.vislist;
        for (L = 1; L < numleaves; v++)
          {
            if (visisz[v] == 0)           // value 0, leaves invisible
              {
                L += 8 * visisz[v + 1]    // skip some leaves
                v++;
              }
            else                          // tag 8 leaves, if needed
              {                           // examine bits right to left
                for (bit = 1; bit != 0; bit = bit * 2, L++)
                  {
                    if (visisz[v] & bit)
                      TagLeafAsVisible(L);
                  }
              }
          }
     */
    // Get a pointer to the visibility list that is associated with the BSP leaf
    for (int L = 1; L < this->getNumLeaves(); visibilityList++) {
        const unsigned char veces = *(visibilityList);
        if (veces == 0) {
            L += 8 * *(++visibilityList);
        } else {
            for (int j = 0; j < 8; j++, L++) {
                if (Tools::getBit(veces, j)) {
                    dleaf_t *visibleLeaf = this->getLeaf(L);
                    int firstSurface = visibleLeaf->firstsurf;
                    int lastSurface = firstSurface + visibleLeaf->numsurf;
                    for (int k = firstSurface; k < lastSurface ; k++) {
                        bool repeat = false;
                        for (int x = 0; x < numVisibleSurfaces; x++) {
                            if (visibleSurfaces[x] == this->getSurfaceList(k)) {
                                repeat = true;
                            }
                        }
                        if (!repeat)
                            visibleSurfaces[numVisibleSurfaces++] = this->getSurfaceList(k);
                    }
                }
            }
        }
    }

    this->numVisibleSurfacesFrame = numVisibleSurfaces;
}

// Calculate which other leaves are visible from the specified leaf, fetch the associated surfaces and draw them
void BSPMap::DrawVisibleLeaf(Camera3D *cam)
{
    if (this->currentLeaf == nullptr) return;

    DrawSurfaceList(visibleSurfaces, numVisibleSurfacesFrame);
}

void BSPMap::DrawHulls(Camera3D *cam)
{
    int numHulls = this->getNumModels();

    for (int m = 1; m < numHulls; m++) {
        model_t *h = this->getModel(m);
        if (m == EngineSetup::getInstance()->TESTING_INT) {

            std::cout << "max:" << h->box.max[0] << ", " << h->box.max[1] << ", " << h->box.max[2] << std::endl;
            std::cout << "min:" << h->box.min[0] << ", " << h->box.min[1] << ", " << h->box.min[2] << std::endl;

            for (int i = h->firstsurf; i < h->firstsurf + h->numsurf ; i++) {
                this->DrawSurfaceTriangles( i );
            }

        }
    }
}

// Draw the visible surfaces
void BSPMap::DrawSurfaceList(int *visibleSurfaces, int numVisibleSurfaces)
{
    // Loop through all the visible surfaces and activate the texture objects
    for (int i = 0; i < numVisibleSurfaces; i++) {
        DrawSurfaceTriangles( visibleSurfaces[i] );
    }
}

// Traverse the BSP tree to find the leaf containing visible surfaces from a specific position
dleaf_t *BSPMap::FindLeaf(Vertex3D camPosition, bool updateCurrentLeaft)
{
    dleaf_t *leaf = NULL;

    // Fetch the start node
    dnode_t *node = this->getStartNode();
    while (!leaf) {
        short nextNodeId;

        // Get a pointer to the plane which intersects the node
        plane_t *plane = this->getPlane(node->planenum);
        plane->normal[0] = plane->normal[0];
        plane->normal[1] = plane->normal[1];
        plane->normal[2] = plane->normal[2];

        // Calculate distance to the intersecting plane
        Vertex3D cp = camPosition;
        cp = M3(-90, 0, 0) * cp;

        vec3_t cam_pos;
        cam_pos[0] = cp.x;
        cam_pos[1] = cp.y;
        cam_pos[2] = cp.z;

        float distance = CalculateDistance(plane->normal, cam_pos);

        // If the camera is in front of the plane
        // traverse the right (front) node, otherwise traverse the left (back) node
        if (distance > plane->dist*scale) {
            nextNodeId = node->children[0];
        } else {
            nextNodeId = node->children[1];
        }

        // If next node >= 0, traverse the node
        // otherwise use the inverse of the node as the index to the leaf (and we are done!)
        if (nextNodeId >= 0) {
            node = this->getNode(nextNodeId);
        } else {
            leaf = this->getLeaf(~nextNodeId);
            if (updateCurrentLeaft) {
                this->currentLeaf = this->getLeaf(~nextNodeId);
            }

            if (EngineSetup::getInstance()->LOG_LEAF_TYPE) {
                Logging::getInstance()->Log(
                        "Leaf type: " + std::to_string(leaf->contents) +
                        ", numsurf: " + std::to_string(leaf->numsurf) +
                        ", distance: " + std::to_string(distance) +
                        ", firstsurf: " + std::to_string(leaf->firstsurf)
                );
            }
        }
    }

    return leaf;
}

// Original Quake source function :_)
char *BSPMap::parseEntities (char *s)
{
    char pkey[512];
    char value[512];
    int i = 0;

    bool saving_key = false;
    bool saving_value = false;
    int  count_commas = 0;

    int entityId = 0;
    int attributeId = 0;

    int t = 0;
    while (s[i] != '\0') {
        char c = s[i];
        if (s[i] == '{') {
            //printf("\r\nEntity: ");
            this->entities[entityId].id = entityId;
            attributeId = 0;
        } else if (s[i] == '\"' && count_commas == 0) {
            //printf("\r\nKey: ");
            saving_key = true;
            saving_value = false;
            count_commas++;
            t = 0;
        } else if (s[i] == '\"' && count_commas == 1) {
            count_commas++;
        } else if (s[i] == '\"' && count_commas == 2) {
            //printf("Value: ");
            saving_key = false;
            saving_value = true;
            count_commas++;
            t = 0;
        } else if (s[i] == '\"' && count_commas == 3) {
            count_commas = 0;
            attributeId++;
            this->entities[entityId].num_attributes++;
        } else {
            if (c != '{' && c != '}' && c != '\n') {
                if (saving_key && c != ' ') { // key doesnt accept whitespaces
                    this->entities[entityId].attributes[attributeId].key[t] = c;
                    pkey[t] = c;
                    t++;
                }
                if (saving_value) {
                    this->entities[entityId].attributes[attributeId].value[t] = c;
                    value[t] = c;
                    t++;
                }

                //printf("%c", c);
            }
            if (c == '}' ) {
                //printf("End element");
                entityId++;
                this->n_entities++;
            }
        }
        i++;
    }
}

char *BSPMap::getEntityValue(int entityId, char *key)
{
    for (int j = 0 ; j < this->entities[entityId].num_attributes ; j++ ) {
        if (!strcmp(this->entities[entityId].attributes[j].key, key)) {
            return this->entities[entityId].attributes[j].value;
        }
    }
}

int BSPMap::getIndexOfFirstEntityByClassname(char *value)
{
    for (int i = 0 ; i <= this->n_entities ; i++) {
        if (hasEntityAttribute(i, "classname")) {
            char *v = getEntityValue(i, "classname");
            if (!strcmp(v, value)) {
                return i;
            }
        }
    }

    return -1;
}

int  BSPMap::getIndexOfFirstEntityByTargetname(const char *value)
{
    for (int i = 0 ; i <= this->n_entities ; i++) {
        if (hasEntityAttribute(i, "targetname")) {
            char *v = getEntityValue(i, "targetname");
            if (!strcmp(v, value)) {
                return i;
            }
        }
    }

    return -1;
}

int  BSPMap::getIndexOfFirstEntityByTarget(const char *value)
{
    for (int i = 0 ; i <= this->n_entities ; i++) {
        if (hasEntityAttribute(i, "target")) {
            char *v = getEntityValue(i, "target");
            if (!strcmp(v, value)) {
                return i;
            }
        }
    }

    return -1;
}

int BSPMap::getIndexOfFirstEntityByModel(const char *value)
{
    for (int i = 0 ; i <= this->n_entities ; i++) {
        if (hasEntityAttribute(i, "model")) {
            char *v = getEntityValue(i, "model");
            if (!strcmp(v, value)) {
                return i;
            }
        }
    }

    return -1;
}

bool BSPMap::hasEntityAttribute(int entityId, char *key)
{
    for (int j = 0 ; j < this->entities[entityId].num_attributes ; j++ ) {
        if (!strcmp(this->entities[entityId].attributes[j].key, key)) {
            return true;
        }
    }

    return false;
}

Vertex3D BSPMap::parsePositionFromEntityAttribute(char *line)
{
    std::vector<std::string> line_chunks;
    line_chunks = Tools::split(line, ' ');

    float x = atof(line_chunks[0].c_str() );
    float y = -atof(line_chunks[2].c_str() );
    float z = atof(line_chunks[1].c_str() );

    return Vertex3D(x*scale, y*scale, z*scale);
}

Vertex3D BSPMap::getStartMapPosition()
{
    int entityID = this->getIndexOfFirstEntityByClassname("info_player_start");

    if (entityID != -1) {
        char *value = getEntityValue(entityID, "origin");
        //parsePositionFromEntityAttribute(value).consoleInfo("getStartMapPosition", false);
        return parsePositionFromEntityAttribute(value);
    } else {
        Logging::getInstance()->Log("Not exist entity for '" + std::string("info_player_start") + "'", "");
    }
}

bool BSPMap::isCurrentLeafLiquid()
{
    if (currentLeaf == nullptr) {
        return false;
    }

    return BSPMap::isLeafLiquid( this->currentLeaf->contents );
}

bool BSPMap::isLeafLiquid(int type)
{
    // [floor = -1 | out = -2 |water = -3 |mud = -4 | lava = -5]
    return (type < -2);
}

bool BSPMap::hasTexture(std::string name)
{
    for (int i = 0; i < this->getNumTextures(); i++) {
        if (!strcmp(name.c_str(), textures[i].getFilename().c_str())) {
            return true;
        }
    }

    return false;
}

int BSPMap::getTextureAnimatedFrames(std::string name)
{
    int count = atoi(name.substr(1, 1).c_str());
    std::string baseName = name.substr(2, name.length());

    bool end = false;
    while (!end) {
        std::string n = "+" + std::to_string(count + 1) + baseName;
        if (!this->hasTexture(n)) {
            end = true;
        }
        count++;
    }

    Logging::getInstance()->Log("[getTextureAnimatedFrames] baseName: " + baseName + ", max: " + std::to_string(count));

    return count;
}

Texture *BSPMap::getTexture(std::string name)
{
    for (int i = 0; i < this->getNumTextures(); i++) {
        if (!strcmp(name.c_str(), textures[i].getFilename().c_str())) {
            return &textures[i];
        }
    }
}

bool BSPMap::isLoaded() const {
    return loaded;
}

void BSPMap::setLoaded(bool loaded) {
    BSPMap::loaded = loaded;
}

void BSPMap::setFrameTriangles(std::vector<Triangle *> *frameTriangles) {
    BSPMap::frameTriangles = frameTriangles;
}

void BSPMap::drawClipNode(dnode_t *node)
{
    for(int i = node->firstsurf; i < node->firstsurf + node->numsurf; i++) {
        this->DrawSurfaceTriangles(i);
    }

    if (node->children[0] >= 0)
        this->drawClipNode( this->getNode(node->children[0]) );

    if (node->children[1] >= 0)
        this->drawClipNode( this->getNode(node->children[1]) );
}

void BSPMap::drawClipNodes(int i)
{
    // Fetch the start node
    dnode_t *node = this->getStartNode(i);
    this->drawClipNode( node );
}