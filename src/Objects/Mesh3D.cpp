
#include <fstream>
#include <vector>
#include "../../headers/Objects/Mesh3D.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Brakeza3D.h"
#include <string>

Mesh3D::Mesh3D()
{
    this->modelVertices = new Vertex3D[MAX_VERTEX_MODEL];
    this->modelTextures = new Texture[MAX_MESH_TEXTURES];
    this->verticesTextureCoordsList = new vec3_t[MAX_VERTEX_MODEL];

    this->numVertices = 0;
    this->numTextures = 0;

    for (int i = 0; i < MAX_MESH_TEXTURES; i++) {
        this->modelTextures[i] = Texture();
    }

    shadowCaster = false;
    BSPEntityIndex = -1;
    decal = false;

    this->octree = NULL;
    this->grid = NULL;
}

bool Mesh3D::loadOBJBlender(const char *name)
{
    source_file = name;

    if (!Tools::fileExists(source_file)){
        Logging::getInstance()->Log("El fichero de modelo solicitado no existe.", "ERROR");

        return false;
    }

    Logging::getInstance()->Log("Loading OBJ: " + (std::string)name + "", "INFO");

    this->loadOBJBlenderVertex();
    this->loadOBJBlenderMaterials();
    this->loadOBJBlenderTextureCoordinates();
    this->loadOBJBlenderTriangles();
}

void Mesh3D::loadOBJBlenderVertex()
{
    std::vector<std::string> line_chunks;
    std::string line;
    float x, y, z;

    int i = 0;
    std::ifstream myfile (source_file);

    while(!myfile.eof()) {
        getline (myfile,line);

        //Si empieza por 'v' es un vértice
        if (line[0] == 'v' && line[1] != 't') {
            line_chunks = Tools::split(line, ' ');

            x = (float) atof(line_chunks[1].c_str() );
            y = (float) atof(line_chunks[2].c_str() );
            z = (float) atof(line_chunks[3].c_str() );

            this->modelVertices[i] = Vertex3D(-x, -y, z);
            i++;
        }
    }

    this->numVertices = i;

    Logging::getInstance()->Log("OBJ Mesh vertex: " + std::to_string(this->numVertices) + "", "INFO");

    return;
}

void Mesh3D::loadOBJBlenderTextureCoordinates()
{
    std::vector<std::string> line_chunks;
    std::string line;
    float u, v;
    int i = 0;


    if (!Tools::fileExists(source_file)) {
        return;
    }

    std::ifstream myfile (source_file);
    while(!myfile.eof()) {
        getline (myfile,line);

        //Si empieza por 'vt' es una coordenada uv
        if (line[0] == 'v' && line[1] == 't') {
            line_chunks = Tools::split(line, ' ');

            u = atof(line_chunks[1].c_str() );
            v = atof(line_chunks[2].c_str() );

            this->verticesTextureCoordsList[i][0] = u;
            this->verticesTextureCoordsList[i][1] = v;
            //printf("u %f v%f:\r\n", u, v);
            i++;
        }
    }

    Logging::getInstance()->Log("OBJ UV Loaded vertex: " + std::to_string(i) + "", "INFO");

    return;
}

void Mesh3D::loadOBJBlenderTriangles()
{
    std::string line, v;
    std::vector<std::string> line_chunks;
    std::vector<std::string> vertex_chunks;

    int i = 0;
    int idx1_vertex = -1, idx2_vertex = -1, idx3_vertex;
    int idx1_uv = -1, idx2_uv = -1, idx3_uv = -1;

    if (!Tools::fileExists(source_file)) {
        return;
    }

    std::ifstream myfile (source_file);

    while(!myfile.eof()) {
        getline (myfile,line);

        //Si empieza por 'f' es un triángulo
        if (line[0] == 'f') {
            line_chunks = Tools::split(line, ' ');

            // Example:  f 123//121 312//043 094//234 -> (123, 321, 094)
            vertex_chunks = Tools::split(line_chunks[1], '/');
            idx1_vertex = std::stoi(vertex_chunks[0]);
            if (!vertex_chunks[1].empty()) {
                idx1_uv =  std::stoi(vertex_chunks[1]);
            }

            vertex_chunks = Tools::split(line_chunks[2], '/');
            idx2_vertex = std::stoi( vertex_chunks[0]);
            if (!vertex_chunks[1].empty()) {
                idx2_uv =  std::stoi(vertex_chunks[1]);
            }

            vertex_chunks = Tools::split(line_chunks[3], '/');
            idx3_vertex = std::stoi( vertex_chunks[0]);
            if (!vertex_chunks[1].empty()) {
                idx3_uv =  std::stoi(vertex_chunks[1]);
            }

            // El Blender el índice empieza en 1, nosotros usamos el 0.
            idx1_vertex--; idx2_vertex--;idx3_vertex--;
            idx1_uv--; idx2_uv--;idx3_uv--;

            Vertex3D V1 = this->modelVertices[idx1_vertex];
            Vertex3D V2 = this->modelVertices[idx2_vertex];
            Vertex3D V3 = this->modelVertices[idx3_vertex];

            if (idx1_uv >= 0) {
                V1.u = this->verticesTextureCoordsList[idx1_uv][0];
                V1.v = this->verticesTextureCoordsList[idx1_uv][1];
            }

            if (idx2_uv >= 0) {
                V2.u = this->verticesTextureCoordsList[idx2_uv][0];
                V2.v = this->verticesTextureCoordsList[idx2_uv][1];
            }

            if (idx3_uv >= 0) {
                V3.u = this->verticesTextureCoordsList[idx3_uv][0];
                V3.v = this->verticesTextureCoordsList[idx3_uv][1];
            }

            // triangle geometry
            this->modelTriangles.emplace_back( new Triangle(V1, V2, V3, this) );

            // set texture
            if (this->modelTextures[0].loaded) {
                this->modelTriangles[i]->setTexture(&this->modelTextures[0]);
            }

            // set light points
            //this->modelTriangles[i]->setLightPoints(this->lightPoints);

            // triangle order in mesh
            this->modelTriangles[i]->order = i;

            i++;
        }
    }

    Logging::getInstance()->Log(this->label + ": OBJ Mesh Triangles: " + std::to_string(this->modelTriangles.size()) + "", "INFO");
}

void Mesh3D::loadOBJBlenderMaterials() {
    std::string line, v;
    std::vector<std::string> line_chunks;
    std::vector<std::string> vertex_chunks;

    int i = 0;

    std::string mlt_filename = source_file;
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
            t.loadTGA( line_chunks[1].c_str(), 1 );
            this->modelTextures[i] = t;
            this->numTextures++;
            cont_materials++;
        }
    }

    Logging::getInstance()->Log("OBJ Materials: " + std::to_string(numTextures) + "", "INFO");
}

void Mesh3D::sendTrianglesToFrame(std::vector<Triangle*> *frameTriangles)
{
    // draw triangles of mesh
    ComponentsManager::get()->getComponentRender()->lockFrameTriangles.lock();
    for (int i = 0; i < this->modelTriangles.size() ; i++) {
        this->modelTriangles[i]->updateTextureAnimated();
        this->modelTriangles[i]->updateLightmapFrame();
        frameTriangles->push_back( this->modelTriangles[i] );
    }
    ComponentsManager::get()->getComponentRender()->lockFrameTriangles.unlock();
}

/*void Mesh3D::setLightPoints(std::vector<LightPoint3D *> &lightPoints)
{
    this->lightPoints = lightPoints;
}*/

/*void Mesh3D::shadowMapping(LightPoint3D *lp)
{
    for (int i = 0; i < this->modelTriangles.size(); i++) {
        this->modelTriangles[i]->shadowMapping(lp);
    }
}*/

bool Mesh3D::isShadowCaster() const {
    return shadowCaster;
}

void Mesh3D::setShadowCaster(bool shadow_caster) {
    Mesh3D::shadowCaster = shadow_caster;
}

int Mesh3D::getBspEntityIndex() const {
    return BSPEntityIndex;
}

void Mesh3D::setBspEntityIndex(int bspEntityIndex) {
    BSPEntityIndex = bspEntityIndex;
}

void Mesh3D::updateBoundingBox()
{
    float maxX = -9999999, minX = 9999999, maxY = -9999999, minY = 9999999, maxZ = -9999999, minZ = 9999999;

    for (int i = 0; i < this->modelTriangles.size(); i++) {
        this->modelTriangles[i]->updateObjectSpace();

        maxX = std::fmax(maxX, this->modelTriangles[i]->Ao.x);
        minX = std::fmin(minX, this->modelTriangles[i]->Ao.x);

        maxY = std::fmax(maxY, this->modelTriangles[i]->Ao.y);
        minY = std::fmin(minY, this->modelTriangles[i]->Ao.y);

        maxZ = std::fmax(maxZ, this->modelTriangles[i]->Ao.z);
        minZ = std::fmin(minZ, this->modelTriangles[i]->Ao.z);

        //
        maxX = std::fmax(maxX, this->modelTriangles[i]->Bo.x);
        minX = std::fmin(minX, this->modelTriangles[i]->Bo.x);

        maxY = std::fmax(maxY, this->modelTriangles[i]->Bo.y);
        minY = std::fmin(minY, this->modelTriangles[i]->Bo.y);

        maxZ = std::fmax(maxZ, this->modelTriangles[i]->Bo.z);
        minZ = std::fmin(minZ, this->modelTriangles[i]->Bo.z);

        //
        maxX = std::fmax(maxX, this->modelTriangles[i]->Co.x);
        minX = std::fmin(minX, this->modelTriangles[i]->Co.x);

        maxY = std::fmax(maxY, this->modelTriangles[i]->Co.y);
        minY = std::fmin(minY, this->modelTriangles[i]->Co.y);

        maxZ = std::fmax(maxZ, this->modelTriangles[i]->Co.z);
        minZ = std::fmin(minZ, this->modelTriangles[i]->Co.z);
    }

    this->aabb.max.x = maxX;
    this->aabb.max.y = maxY;
    this->aabb.max.z = maxZ;

    this->aabb.min.x = minX;
    this->aabb.min.y = minY;
    this->aabb.min.z = minZ;

    this->aabb.vertices[0] = this->aabb.max;
    this->aabb.vertices[1] = this->aabb.min;
    this->aabb.vertices[2] = Vertex3D(this->aabb.max.x, this->aabb.max.y, this->aabb.min.z);
    this->aabb.vertices[3] = Vertex3D(this->aabb.max.x, this->aabb.min.y, this->aabb.max.z);
    this->aabb.vertices[4] = Vertex3D(this->aabb.min.x, this->aabb.max.y, this->aabb.max.z);
    this->aabb.vertices[5] = Vertex3D(this->aabb.max.x, this->aabb.min.y, this->aabb.min.z);
    this->aabb.vertices[6] = Vertex3D(this->aabb.min.x, this->aabb.max.y, this->aabb.min.z);
    this->aabb.vertices[7] = Vertex3D(this->aabb.min.x, this->aabb.min.y, this->aabb.max.z);
}

void Mesh3D::copyFrom(Mesh3D *source)
{
    Logging::getInstance()->Log("Mesh3D: copyFrom " + source->getLabel() + " to " + this->getLabel());

    // Triangles
    for (auto & modelTriangle : source->modelTriangles) {
        auto *t = new Triangle(
            modelTriangle->A,
            modelTriangle->B,
            modelTriangle->C,
            this
        );

        t->setTexture(modelTriangle->getTexture());

        this->modelTriangles.push_back(t);
    }

    // Textures
    this->numTextures   = source->numTextures;
    this->modelTextures = source->modelTextures;
    this->scale         = source->scale;
    this->source_file   = source->source_file;
}

void Mesh3D::onUpdate()
{
    this->sendTrianglesToFrame(&ComponentsManager::get()->getComponentRender()->getFrameTriangles()) ;

    if (EngineSetup::getInstance()->DRAW_MESH3D_OCTREE) {
        if (this->octree != NULL) {
            Drawable::drawOctree(this->octree, true);
        }
    }

    if (EngineSetup::getInstance()->DRAW_MESH3D_GRID) {
        if (this->grid != NULL) {
            Drawable::drawGrid3D(this->grid);
        }
    }

    if (EngineSetup::getInstance()->DRAW_MESH3D_AABB) {
        this->updateBoundingBox();
        Drawable::drawAABB(&this->aabb, Color::white());
    }
}

bool Mesh3D::AssimpLoadGeometryFromFile(std::string fileName)
{
    setSourceFile( fileName );

    Logging::getInstance()->Log("AssimpLoadGeometryFromFile for " + fileName);

    Assimp::Importer importer;
    const aiScene* scene;

    scene = importer.ReadFile( fileName, aiProcess_Triangulate |
                                               aiProcess_JoinIdenticalVertices |
                                               aiProcess_SortByPType |
                                               aiProcess_FlipUVs
    );

    if( !scene ) {
        Logging::getInstance()->Log("Error import 3D file for ASSIMP");
        exit(-1);
        return false;
    }

    AssimpInitMaterials(scene, fileName);
    AssimpProcessNodes( scene, scene->mRootNode );
}

bool Mesh3D::AssimpInitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    } else if (SlashIndex == 0) {
        Dir = "/";
    } else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    Logging::getInstance()->Log("ASSIMP: mNumMaterials: " + std::to_string(pScene->mNumMaterials), "Mesh3DAnimated");

    for (uint i = 0 ; i < pScene->mNumMaterials ; i++) {
        aiMaterial *pMaterial = pScene->mMaterials[i];
        std::cout << "Import material: " << pMaterial->GetName().C_Str() << std::endl;

        if  (std::string(pMaterial->GetName().C_Str()) == AI_DEFAULT_MATERIAL_NAME) {
            this->numTextures++;
            continue;
        };

        //if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string p(Path.data);

                std::string base_filename = p.substr(p.find_last_of("/\\") + 1);

                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                std::string FullPath = EngineSetup::getInstance()->TEXTURES_FOLDER + this->prefix_texture_folder + base_filename;

                std::cout << "Import texture " << FullPath << " for ASSIMP Mesh" << std::endl;
                Texture *t = new Texture();
                if (t->loadTGA(FullPath.c_str(), 1) ) {
                    this->modelTextures[ this->numTextures ] = *t;
                    this->numTextures++;
                }
            } else {
                Logging::getInstance()->Log("ERROR: mMaterial["+std::to_string(i) + "]: Not valid color", "Mesh3DAnimated");
            }
        //}
    }

    return Ret;
}

void Mesh3D::AssimpProcessNodes(const aiScene *scene, aiNode *node)
{
    for (int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = node->mMeshes[x];
        this->AssimpLoadMesh( scene->mMeshes[ idMesh ] );
    }

    for (int j = 0; j < node->mNumChildren; j++) {
        AssimpProcessNodes( scene, node->mChildren[j] );
    }
}

void Mesh3D::AssimpLoadMesh(aiMesh *mesh)
{

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::getInstance()->Log("Skip mesh non triangle");
        return;
    }

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);
    for (unsigned int j = 0 ; j < mesh->mNumVertices ; j++) {

        aiVector3t vf = mesh->mVertices[j];

        Vertex3D v( vf.x, -vf.y, vf.z );

        const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        localMeshVertices[j] = v;
    }

    for (unsigned int k = 0 ; k < mesh->mNumFaces ; k++) {
        const aiFace& Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        this->modelTriangles.push_back( new Triangle(V3, V2, V1, this) );

        if (this->numTextures > 0) {
            this->modelTriangles[k]->setTexture( &this->modelTextures[ mesh->mMaterialIndex ] );
        }
    }
}

const std::string &Mesh3D::getSourceFile() const {
    return source_file;
}

void Mesh3D::setSourceFile(const std::string &sourceFile) {
    source_file = sourceFile;
}

Octree *Mesh3D::getOctree() const {
    return octree;
}

Grid3D *Mesh3D::getGrid3D() const
{
    return grid;
}

void Mesh3D::buildGrid3D(int sizeX, int sizeY, int sizeZ, Grid3D::EmptyStrategies strategy)
{
    this->updateBoundingBox();
    this->grid = new Grid3D(&this->modelTriangles, this->aabb,sizeX, sizeY, sizeZ, strategy);
    this->grid->applyEmptyStrategy();
}

void Mesh3D::buildGrid3D(int sizeX, int sizeY, int sizeZ, Grid3D::EmptyStrategies strategy, Vertex3D direction)
{
    this->updateBoundingBox();
    this->grid = new Grid3D(&this->modelTriangles, this->aabb, sizeX, sizeY, sizeZ, strategy);
    this->grid->setRayIntersectionDirection( direction );
    this->grid->applyEmptyStrategy();
}

void Mesh3D::buildOctree() 
{
    this->updateBoundingBox();
    this->octree = new Octree(this->modelTriangles, this->aabb);
}
