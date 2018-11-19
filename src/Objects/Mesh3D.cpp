
#include <fstream>
#include <vector>
#include "../../headers/Objects/Mesh3D.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/Timer.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Objects/LightPoint3D.h"
#include "../../headers/Render/Logging.h"
#include <iostream>
#include <string>
#include <math.h>       /* modf */
#include <cstdio>

Mesh3D::Mesh3D()
{
    this->model_triangles = new Triangle[MAX_MODEL_TRIANGLES];
    this->model_vertex = new Vertex3D[MAX_VERTEX_MODEL];
    this->model_textures = new Texture[MAX_MESH_TEXTURES];
    this->n_triangles = 0;
    this->n_vertex = 0;
    this->n_textures = 0;

    for (int i = 0; i < MAX_MESH_TEXTURES; i++) {
        this->model_textures[i] = Texture();
    }

    // lights
    this->lightPoints = new LightPoint3D*[EngineSetup::getInstance()->ENGINE_MAX_GAMEOBJECTS];
    this->numberLightPoints = 0;

    this->billboard = new Billboard();
    this->billboard->setDrawable(false);
    this->billboard->loadTexture( EngineSetup::getInstance()->ICON_OBJECT3D_DEFAULT);

    shadowCaster = false;
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
        if (line[0] == 'v') {
            line_chunks = Tools::split(line, ' ');

            x = (float) atof(line_chunks[1].c_str() );
            y = (float) atof(line_chunks[2].c_str() );
            z = (float) atof(line_chunks[3].c_str() );

            this->model_vertex[i] = Vertex3D(x, y, z);
            i++;
        }
    }

    this->n_vertex = i;

    Logging::getInstance()->Log("OBJ Mesh vertex: " + std::to_string(this->n_vertex) + "", "INFO");

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

            this->model_vertex[i].u = u;
            this->model_vertex[i].v = v;

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
            t.loadTGA( line_chunks[1].c_str() );
            this->model_textures[i] = t;
            this->n_textures++;
            cont_materials++;
        }
    }

    Logging::getInstance()->Log("OBJ Materials: " + std::to_string(n_textures) + "", "INFO");
}

void Mesh3D::draw(Camera3D *cam)
{
    //EngineBuffers::getInstance()->resetBenchmarkValues();

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
        EngineBuffers::getInstance()->consoleInfo();
    }

    // Draw Billboard
    if (EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD) {
        this->billboard->updateUnconstrainedQuad( 1, 1, this, cam->AxisUp(), cam->AxisRight() );
        if (this->billboard->isDrawable()) {
            Drawable::drawBillboard(this->billboard, cam );
        }
    }
}

void Mesh3D::setLightPoints(LightPoint3D **lightPoints, int number)
{
    this->lightPoints = lightPoints;
    this->numberLightPoints = number;
}

void Mesh3D::shadowMapping(LightPoint3D *lp)
{
    for (int i = 0; i < this->n_triangles; i++) {
        this->model_triangles[i].shadowMapping(lp);
    }
}

bool Mesh3D::isShadowCaster() const {
    return shadowCaster;
}

void Mesh3D::setShadowCaster(bool shadow_caster) {
    Mesh3D::shadowCaster = shadow_caster;
}
