
#include <fstream>
#include <vector>
#include "../../headers/Objects/Mesh3D.h"
#include "../../headers/Render/Logging.h"
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
}

bool Mesh3D::loadOBJBlender(const char *name)
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

void Mesh3D::loadOBJBlenderVertex()
{
    std::vector<std::string> line_chunks;
    std::string line;
    float x, y, z;

    int i = 0;
    std::ifstream myfile (mesh_file);

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
            t.loadTGA( line_chunks[1].c_str(), 1 );
            this->modelTextures[i] = t;
            this->numTextures++;
            cont_materials++;
        }
    }

    Logging::getInstance()->Log("OBJ Materials: " + std::to_string(numTextures) + "", "INFO");
}

void Mesh3D::draw(std::vector<Triangle*> *frameTriangles)
{
    // draw triangles of mesh
    for (int i = 0; i < this->modelTriangles.size() ; i++) {
        this->modelTriangles[i]->updateTextureAnimated();
        this->modelTriangles[i]->updateLightmapFrame();
        Triangle *t = this->modelTriangles[i];
        frameTriangles->emplace_back( t );
    }
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

