
#include <fcntl.h> // open
#include <stdio.h> // printf
#include <stdlib.h> // malloc, free
#include <sys/stat.h> // stat
#include <unistd.h> // lseek, read, close
#include <cstring>
#include "Map.h"
#include "headers/Objects/Vertex3D.h"
#include "headers/Render/Drawable.h"
#include "headers/Render/Maths.h"
#include "headers/Render/Transforms.h"
#include "headers/Render/EngineBuffers.h"
#include "headers/Render/Logging.h"

bool Map::Initialize(const char *bspFilename, const char *paletteFilename)
{
    if (!LoadBSP(bspFilename)) {
        printf("[ERROR] Map::Initialize() Error loading bsp file\n");
        return false;
    }

    if (!LoadPalette(paletteFilename)) {
        printf("[ERROR] Map::Initialize() Error loading palette\n");
        return false;
    }

    return true;
}

int Map::LoadFile(const char *filename, void **bufferptr)
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

bool Map::LoadPalette(const char *filename)
{
    unsigned char *tempPal;

    if (!LoadFile(filename, (void **)&tempPal)) {
        return false;
    }

    for (int i = 0; i < 256; i++) {
        unsigned int r = tempPal[i * 3 + 0];
        unsigned int g = tempPal[i * 3 + 1];
        unsigned int b = tempPal[i * 3 + 2];
        palette[i] = (r) | (g << 8) | (b << 16);
    }

    free(tempPal);
    return true;
}

bool Map::LoadBSP(const char *filename)
{
    if (!LoadFile(filename, (void **)&bsp)) {
        return false;
    }

    header = (dheader_t *)bsp;
    if (header->version != BSP_VERSION) {
        printf("[ERROR] Map::LoadBSP() BSP file version mismatch!\n");
        return false;
    }

    return true;
}

//
// Calculate primitive surfaces
//
bool Map::InitializeSurfaces(void)
{
    // Allocate memory for the visible surfaces array
    visibleSurfaces = new int [this->getNumSurfaceLists()];

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
            primitives->v[0] = ((float *)vertex)[0];
            primitives->v[1] = ((float *)vertex)[1];
            primitives->v[2] = ((float *)vertex)[2];

            // Calculate the vertex's texture coords and store it in the primitive array
            primitives->t[0] = (CalculateDistance(textureInfo->snrm, primitives->v) + textureInfo->soff) / mipTexture->width;
            primitives->t[1] = (CalculateDistance(textureInfo->tnrm, primitives->v) + textureInfo->toff) / mipTexture->height;
        }
    }

    return true;
}

//
// Build mipmaps from the textures, create texture objects and assign mipmaps to the created objects
//
bool Map::InitializeTextures(void)
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
    //printf("%d animated textures, %d sky textures\n", numAnimTextues, numSkyTextures);

    // Calculate the total number of textures
    int numTotalTextures = this->getNumTextures() + numAnimTextues * (ANIM_TEX_FRAMES - 1) + numSkyTextures * 2;

    // Allocate memory for texture names and an array to store the textures we are using
    textureObjNames = new unsigned int [numTotalTextures];

    // Get unused texture names
    //glGenTextures(numTotalTextures, textureObjNames);

    // Loop through all texture objects to associate them with a texture and calculate mipmaps from it
    for (int i = 0; i < this->getNumTextures(); i++) {
        // Point to the stored mipmaps
        miptex_t *mipTexture = this->getMipTexture(i);

        // NULL textures exist, don't use them!
        if (!mipTexture->name[0]) {
            continue;
        }

        int width = mipTexture->width;
        int height = mipTexture->height;

        // Allocate memory for the texture which will be created
        unsigned int *texture = new unsigned int [width * height];

        // Point to the raw 8-bit texture data
        unsigned char *rawTexture = this->getRawTexture(i);

        // Create a texture to assign with the texture object
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                texture[x + y * width] = this->palette[rawTexture[x + y * width]];
            }
        }

        // Create a new texture object
        //glBindTexture(GL_TEXTURE_2D, textureObjNames[i]);
        // Create mipmaps from the created texture
        //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture);

        delete texture;
    }

    return true;
}

//
// Return the dotproduct of two vectors
//
float Map::CalculateDistance(vec3_t a, vec3_t b)
{
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void Map::loadTriangles(Camera3D *cam)
{
    for (int i = 0; i < this->getNumSurfaces(); i++) {
        Vertex3D vertices[1000];
        int num_vertices = 0;

        //printf("Surface: %d edges\r\n", this->getNumEdges(i));

        primdesc_t *primitives = &surfacePrimitives[numMaxEdgesPerSurface * i];
        for (int j = 0; j < this->getNumEdges(i); j++, primitives++) {
            Vertex3D v = Vertex3D(
                primitives->v[0],
                primitives->v[1],
                primitives->v[2]
            );
            vertices[num_vertices] = v ; num_vertices++;
        }

        triangulateQuakeSurface(vertices, num_vertices, cam);
    }

    Logging::getInstance()->Log("\"Map Triangles: " + std::to_string(this->n_triangles), "QUAKE");
}


//
// Draw the surface
//
void Map::DrawSurface(int surface, Camera3D *cam)
{
    //printf("Drawing Surface\r\n");
    // Get the surface primitive
    primdesc_t *primitives = &surfacePrimitives[numMaxEdgesPerSurface * surface];

    Vertex3D vertices[20];
    int num_vertices = 0;

    // Loop through all vertices of the primitive and draw a surface
    int num_edges = this->getNumEdges(surface);

    Vertex3D rot = Vertex3D(0, 0, 0);
    for (int i = 0; i < num_edges ; i++, primitives++) {
        // Brakeza (x, y, z) = quake = (y = 1, z = 2, x = 0))
        Vertex3D v = Vertex3D(
            primitives->v[1],
            primitives->v[2],
            primitives->v[0]
        );

        v = Transforms::objectToLocal(v, this);

        vertices[num_vertices] = v ; num_vertices++;
        if ( i+1 < num_edges ) {
            Vertex3D next = Vertex3D(
                (primitives+1)->v[1],
                (primitives+1)->v[2],
                (primitives+1)->v[0]
            );
            next = Transforms::objectToLocal(next, this);

            Vector3D vector_points = Vector3D(v, next);
            Drawable::drawVector3D(vector_points, cam, Color::green());
        }
    }
    triangulateQuakeSurface(vertices, num_vertices, cam);
}

void Map::drawTriangles(Camera3D *cam)
{
    for (int i = 0; i <= this->n_triangles ; i++) {
        this->model_triangles[i].parent = this;
        this->model_triangles[i].draw(cam);
    }
}

bool Map::triangulateQuakeSurface(Vertex3D vertexes[], int num_vertex, Camera3D *cam)
{
    // triangulamos con un sencillo algoritmo que recorre los vértices: 012, 023, 034...
    int current = 1;
    while(current < num_vertex-1 ) {
        int next = current + 1;

        if (next+1 <= num_vertex) {
            EngineBuffers::getInstance()->trianglesClippingCreated++;

            // Vertex new triangles
            Vertex3D tv1 = Transforms::objectToLocal(vertexes[0], this);
            Vertex3D tv2 = Transforms::objectToLocal(vertexes[current], this);
            Vertex3D tv3 = Transforms::objectToLocal(vertexes[next], this);

            this->model_triangles[this->n_triangles] = Triangle(tv1, tv2, tv3, this);
            this->n_triangles++;
        }

        current+=1;
    }
}

//
// Calculate which other leaves are visible from the specified leaf, fetch the associated surfaces and draw them
//
void Map::DrawLeafVisibleSet(bspleaf_t *pLeaf, Camera3D *cam)
{
    // Object's axis
    if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this, cam, true, true, true);
    }

    this->n_triangles = 0;
    int numVisibleSurfaces = 0;

    // Get a pointer to the visibility list that is associated with the BSP leaf
    unsigned char * visibilityList = this->getVisibilityList(pLeaf->vislist);

    for ( int i = 1; i < this->getNumLeaves(); visibilityList++) {
        /*if (*visibilityList) {
            // Loop through the visible leaves
            for (int j = 0; j < 8; j++, i++) {
                // Fetch the leaf that is seen from the array of leaves
                bspleaf_t *visibleLeaf = this->getLeaf(i);
                // Copy all the visible surfaces from the List of surfaces
                int firstSurface = visibleLeaf->firstsurf;
                int lastSurface = firstSurface + visibleLeaf->numsurf;
                for (int k = firstSurface; k < lastSurface; k++) {
                    visibleSurfaces[numVisibleSurfaces++] = this->getSurfaceList(k);
                }
            }
        } else {
            // No, skip some leaves
            i += (*visibilityList++) << 3;
        }

        continue;*/

        unsigned char veces = *(visibilityList);
        if (veces == 0) {
            i += 8 * *(++visibilityList);
        } else {
            for (int j = 0; j < 8; j++, i++) {
                if (Tools::getBit(veces, j)) {
                    bspleaf_t *visibleLeaf = this->getLeaf(i);
                    int firstSurface = visibleLeaf->firstsurf;
                    int lastSurface = firstSurface + visibleLeaf->numsurf;
                    for (int k = firstSurface; k < lastSurface; k++) {
                        visibleSurfaces[numVisibleSurfaces++] = this->getSurfaceList(k);
                    }
                }
            }
        }
    }

    DrawSurfaceList(visibleSurfaces, numVisibleSurfaces, cam);
}

//
// Draw the visible surfaces
//
void Map::DrawSurfaceList(int *visibleSurfaces, int numVisibleSurfaces, Camera3D *cam)
{
    //printf("pinto %d surface\r\n", visibleSurfaces);
    // Loop through all the visible surfaces and activate the texture objects
    for (int i = 0; i < numVisibleSurfaces; i++) {
        // Get a pointer to the texture info so we know which texture we should bind and draw
        //texinfo_t *textureInfo = this->getTextureInfo(visibleSurfaces[i]);
        // Bind the previously created texture object
        //glBindTexture(GL_TEXTURE_2D, textureObjNames[textureInfo->texid]);
        // Draw the surface
        DrawSurface(visibleSurfaces[i], cam);
    }

    if (EngineSetup::getInstance()->MESH_DEBUG_INFO) {
        Logging::getInstance()->Log("\"DrawSurfaceList Triangles: " + std::to_string(this->n_triangles) + " / numVisibleSurfaces: " + std::to_string(numVisibleSurfaces), "BSP");
    }
}

//
// Traverse the BSP tree to find the leaf containing visible surfaces from a specific position
//
bspleaf_t *Map::FindLeaf(Camera3D *camera)
{
    bspleaf_t *leaf = NULL;

    // Fetch the start node
    bspnode_t *node = this->getStartNode();

    while (!leaf) {
        short nextNodeId;

        // Get a pointer to the plane which intersects the node
        plane_t *plane = this->getPlane(node->planenum);

        // Calculate distance to the intersecting plane
        Vertex3D cp = Vertex3D( camera->head[0], camera->head[1], camera->head[2]);
        cp = Maths::rotateVertex(cp, M3(-90, 0, 0));

        vec3_t cam_pos;
        cam_pos[0] = cp.x;
        cam_pos[1] = cp.y;
        cam_pos[2] = cp.z;

        float distance = CalculateDistance(plane->normal, cam_pos);

        // If the camera is in front of the plane, traverse the right (front) node, otherwise traverse the left (back) node
        if (distance > plane->dist) {
            nextNodeId = node->front;
        } else {
            nextNodeId = node->back;
        }

        // If next node >= 0, traverse the node, otherwise use the inverse of the node as the index to the leaf (and we are done!)
        if (nextNodeId >= 0) {
            node = this->getNode(nextNodeId);
        } else {
            leaf = this->getLeaf(~nextNodeId);
        }
    }

    return leaf;
}