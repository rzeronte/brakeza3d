
#include <fstream>
#include <vector>
#include "../../headers/Mesh.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Q3Loader.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Timer.h"
#include "../../headers/Drawable.h"
#include "../../headers/LightPoint.h"
#include "../../headers/Logging.h"
#include <iostream>
#include <string>
#include <math.h>       /* modf */
#include <cstdio>

Mesh::Mesh()
{
    this->model_triangles = new Triangle[MAX_MODEL_TRIANGLES];
    this->model_vertex = new Vertex[MAX_VERTEX_MODEL];
    this->model_textures = new Texture[MAX_MESH_TEXTURES];
    this->n_triangles = 0;
    this->n_vertex = 0;
    this->n_textures = 0;

    for (int i = 0; i < MAX_MESH_TEXTURES; i++) {
        this->model_textures[i] = Texture();
    }

    // lights
    this->lightPoints = new LightPoint*[EngineSetup::getInstance()->ENGINE_MAX_GAMEOBJECTS];
    this->numberLightPoints = 0;

    this->billboard = new Billboard();
    this->billboard->setDrawable(false);
    this->billboard->loadTexture( EngineSetup::getInstance()->ICON_OBJECT3D_DEFAULT);

    shadowCaster = false;
}

bool Mesh::loadOBJBlender(const char *name)
{
    mesh_file = name;

    if (!Tools::fileExists(mesh_file)){
        Logging::getInstance()->Log("El fichero de modelo solicitado no existe.", "ERROR");

        return false;
    }

    Logging::getInstance()->Log("Loading OBJ: " + (std::string)name + "", "INFO");

    this->loadOBJBlenderVertex();
    this->loadOBJBlenderMaterials();
    this->loadOBJBlenderTextureCoordinates();
    this->loadOBJBlenderTriangles();
}


void Mesh::loadQ3Map(const std::string& pFilename)
{
    Logging::getInstance()->Log("Loading Q3BSP " + pFilename, "INFO");

    readMap(pFilename, this->lMap); // Load Q3Map Info Structure
    this->loadTexturesFromQ3Map();
    this->loadTrianglesFromQ3Map();

    Logging::getInstance()->Log("Loading Q3Map (Nº Faces: " + std::to_string(lMap.mFaces.size())+", Nº Textures: "+ std::to_string(lMap.mTextures.size())+")", "INFO");

    //FILE* lFile = fopen("final_debug.txt", "w+");
    //debugInformations(lMap, lFile);
    //fclose(lFile);
    //freeMap(lMap);
}

void Mesh::loadOBJBlenderVertex()
{
    std::vector<std::string> line_chunks;
    std::string line;
    float x, y, z;

    int i = 0;
    std::ifstream myfile (mesh_file);

    while(!myfile.eof()) {
        getline (myfile,line);

        //Si empieza por 'v' es un vértice
        if (line[0] == 'v') {
            line_chunks = Tools::split(line, ' ');

            x = (float) atof(line_chunks[1].c_str() );
            y = (float) atof(line_chunks[2].c_str() );
            z = (float) atof(line_chunks[3].c_str() );

            this->model_vertex[i] = Vertex(x, y, z);
            i++;
        }
    }

    this->n_vertex = i;

    Logging::getInstance()->Log("OBJ Mesh vertex: " + std::to_string(this->n_vertex) + "", "INFO");

    return;
}

void Mesh::loadOBJBlenderTextureCoordinates()
{
    std::vector<std::string> line_chunks;
    std::string line;
    float u, v;
    int i = 0;


    if (!Tools::fileExists(mesh_file)) {
        return;
    }
    std::ifstream myfile (mesh_file);
    while(!myfile.eof()) {
        getline (myfile,line);

        //Si empieza por 'vt' es una coordenada uv
        if (line[0] == 'v' && line[1] == 't') {
            line_chunks = Tools::split(line, ' ');

            u = atof(line_chunks[1].c_str() );
            v = atof(line_chunks[2].c_str() );

            this->model_vertex[i].u = u;
            this->model_vertex[i].v = v;

            //printf("u %f v%f:\r\n", u, v);

            i++;
        }
    }

    Logging::getInstance()->Log("OBJ UV Loaded vertex: " + std::to_string(i) + "", "INFO");

    return;
}

void Mesh::loadOBJBlenderTriangles()
{
    std::string line, v;
    std::vector<std::string> line_chunks;
    std::vector<std::string> vertex_chunks;

    int i = 0;
    int idx1, idx2, idx3;

    if (!Tools::fileExists(mesh_file)) {
        return;
    }

    std::ifstream myfile (mesh_file);

    while(!myfile.eof()) {
        getline (myfile,line);

        //Si empieza por 'f' es un triángulo
        if (line[0] == 'f') {
            line_chunks = Tools::split(line, ' ');

            // Example:  f 123//121 312//043 094//234 -> (123, 321, 094)
            vertex_chunks = Tools::split(line_chunks[1], '/');
            idx1 = std::stoi(vertex_chunks[0]);

            vertex_chunks = Tools::split(line_chunks[2], '/');
            idx2 = std::stoi( vertex_chunks[0]);

            vertex_chunks = Tools::split(line_chunks[3], '/');
            idx3 = std::stoi( vertex_chunks[0]);

            // El Blender el índice empieza en 1, nosotros usamos el 0.
            idx1--; idx2--;idx3--;

            // triangle geometry
            this->model_triangles[i] = Triangle(
                    this->model_vertex[idx1--],
                    this->model_vertex[idx2--],
                    this->model_vertex[idx3--],
                    this
            );

            // set texture
            if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED && this->model_textures[0].loaded) {
                this->model_triangles[i].setTexture(&this->model_textures[0]);
            }

            // set light points
            this->model_triangles[i].setLightPoints(this->lightPoints, this->numberLightPoints);

            // triangle order in mesh
            this->model_triangles[i].order = i;

            i++;
        }
    }

    this->n_triangles = i;

    Logging::getInstance()->Log("OBJ Mesh Triangles: " + std::to_string(this->n_triangles) + "", "INFO");
}

void Mesh::loadOBJBlenderMaterials() {
    std::string line, v;
    std::vector<std::string> line_chunks;
    std::vector<std::string> vertex_chunks;

    int i = 0;

    std::string mlt_filename = mesh_file;
    mlt_filename.replace( mlt_filename.end() -3, mlt_filename.end(), "mtl");

    std::ifstream myfile (mlt_filename);

    int cont_materials = 0;

    if (!Tools::fileExists(mlt_filename)) {
        return;
    }

    while(!myfile.eof()) {
        getline (myfile,line);
        line_chunks = Tools::split(line, ' ');

        if (line_chunks[0].compare("map_Kd") == 0) {
            Texture t = Texture();
            t.loadTGA( line_chunks[1].c_str() );
            this->model_textures[i] = t;
            this->n_textures++;
            cont_materials++;
        }
    }

    Logging::getInstance()->Log("OBJ Materials: " + std::to_string(n_textures) + "", "INFO");
}

void Mesh::draw(Camera *cam)
{
    EngineBuffers::getInstance()->resetBenchmarkValues();

    // Object's axis
    if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this, cam, true, true, true);
    }

    // Transforms
    for (int i = 0; i < this->n_triangles ; i++) {
        this->model_triangles[i].draw(cam);
    }

    // Console info
    if (EngineSetup::getInstance()->MESH_DEBUG_INFO) {
        getPosition()->consoleInfo("T", false);
        EngineBuffers::getInstance()->consoleInfo();
    }

    // Draw Billboard
    if (EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD) {
        this->billboard->updateUnconstrainedQuad( 1, 1, this, cam->upVector(), cam->rightVector() );
        if (this->billboard->isDrawable()) {
            Drawable::drawBillboard(this->billboard, cam );
        }
    }
}

// http://www.mralligator.com/q3/
void Mesh::loadTrianglesFromQ3Map()
{
    int cont = 0;
    Vertex A, B, C;

    float scale = 0.001f;

    for (int i = 0 ; i < lMap.mFaces.size() ; i++) {

        // polygon
        int vertexIndex = lMap.mFaces[i].mVertex;
        int firstMeshVertexIndex = lMap.mFaces[i].mMeshVertex;

        int textureIndex = lMap.mFaces[i].mTextureIndex;

        //printf("%s\r\n",lMap.mTextures[textureIndex].mName );

        if ( lMap.mFaces[i].mType == 1 ) {

            for ( int j = 0 ; j < lMap.mFaces[i].mNbMeshVertices ; j+=3 ) {

                int i0 = lMap.mMeshVertices[firstMeshVertexIndex+0].mMeshVert;
                int i1 = lMap.mMeshVertices[firstMeshVertexIndex+1].mMeshVert;
                int i2 = lMap.mMeshVertices[firstMeshVertexIndex+2].mMeshVert;

                A = Vertex (
                    lMap.mVertices[vertexIndex+i0].mPosition[0] * scale,
                    lMap.mVertices[vertexIndex+i0].mPosition[1] * scale,
                    lMap.mVertices[vertexIndex+i0].mPosition[2] * scale
                );
                A.u = lMap.mVertices[vertexIndex+i0].mTexCoord[0][0];
                A.v = lMap.mVertices[vertexIndex+i0].mTexCoord[0][1];

                // --

                B = Vertex (
                    lMap.mVertices[vertexIndex+i1].mPosition[0] * scale,
                    lMap.mVertices[vertexIndex+i1].mPosition[1] * scale,
                    lMap.mVertices[vertexIndex+i1].mPosition[2] * scale
                );
                B.u = lMap.mVertices[vertexIndex+i1].mTexCoord[0][0];
                B.v = lMap.mVertices[vertexIndex+i1].mTexCoord[0][1];

                C = Vertex (
                    lMap.mVertices[vertexIndex+i2].mPosition[0] * scale,
                    lMap.mVertices[vertexIndex+i2].mPosition[1] * scale,
                    lMap.mVertices[vertexIndex+i2].mPosition[2] * scale
                );
                C.u = lMap.mVertices[vertexIndex+i2].mTexCoord[0][0];
                C.v = lMap.mVertices[vertexIndex+i2].mTexCoord[0][1];

                Triangle t = Triangle(A, B, C, this);

                this->model_triangles[cont] = t;
                this->model_triangles[cont].order = cont;

                if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
                    if (this->model_textures[textureIndex].loaded) {
                        this->model_triangles[cont].setTexture(&this->model_textures[textureIndex]);
                    }
                }

                cont++;
                firstMeshVertexIndex+=3;
            }
        }
    }

    this->n_triangles = cont;

    Logging::getInstance()->Log("Q3Map Triangles 'polygon': " + std::to_string(this->n_triangles), "INFO");
}

void Mesh::loadTexturesFromQ3Map()
{
    char tgaExt[64] = ".tga";
    char jpgExt[64] = ".jpg";

    for (int i=0 ; i< lMap.mTextures.size() ; i++) {
        char tgaNameTexture[64];
        char jpgNameTexture[64];

        Texture t = Texture();

        strcpy(tgaNameTexture, "../pak0/");
        strcat(tgaNameTexture, lMap.mTextures[i].mName);
        strcat(tgaNameTexture, tgaExt);

        strcpy(jpgNameTexture, "../pak0/");
        strcat(jpgNameTexture, lMap.mTextures[i].mName);
        strcat(jpgNameTexture, jpgExt);

        int existsTGATexture = 0;
        int existsJPGTexture = 0;
        int existsTexture = 0;

        if (Tools::fileExists(tgaNameTexture) ) {
            existsTGATexture = 1;
            existsTexture = 1;
        }

        if (Tools::fileExists(jpgNameTexture) ) {
            existsJPGTexture = 1;
            existsTexture = 1;
        }

        if (existsTexture && existsJPGTexture) {
            t.loadJPG( jpgNameTexture );
            this->model_textures[i] = t;
            this->n_textures++;
        }

        if (existsTexture && existsTGATexture) {
            t.loadTGA( tgaNameTexture);
            this->model_textures[i] = t;
            this->n_textures++;
        }
        //printf("Loading Texture: index: %d - '%s' texture (existsTexture: %d, existsJPGTexture: %d, existsTGATexture: %d, Content: %d, Flags: %d)\r\n", i, lMap.mTextures[i].mName, existsTexture, existsJPGTexture, existsTGATexture, lMap.mTextures[i].mContents, lMap.mTextures[i].mFlags);
    }

    Logging::getInstance()->Log("Q3Map Textures: " + std::to_string(this->n_textures), "INFO");
}

void Mesh::setLightPoints(LightPoint **lightPoints, int number)
{
    this->lightPoints = lightPoints;
    this->numberLightPoints = number;
}

void Mesh::shadowMapping(LightPoint *lp)
{
    for (int i = 0; i < this->n_triangles; i++) {
        this->model_triangles[i].shadowMapping(lp);
    }
}

bool Mesh::isShadowCaster() const {
    return shadowCaster;
}

void Mesh::setShadowCaster(bool shadow_caster) {
    Mesh::shadowCaster = shadow_caster;
}
