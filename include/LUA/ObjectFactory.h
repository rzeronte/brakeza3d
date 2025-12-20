//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_OBJECTFACTORY_H
#define BRAKEZA3D_OBJECTFACTORY_H

#include "../Brakeza.h"
#include "../3D/Mesh3D.h"

class ObjectFactory {

    public:
    static void CreateObject3D(const Vertex3D &position);
    static void CreateImage2D(const std::string &file, int x, int y, int w, int h);
    static void CreateImage2DAnimation(const std::string &file, const Vertex3D &position, int x, int y, int sw, int sh, int frames, int fps);
    static void CreateImage3D(const std::string &file, const Vertex3D &position, float width, float height);
    static void CreateImage3DAnimation(const std::string &file, const Vertex3D &position, float width, float height, float sw, float sh, int frames, int fps);
    static void CreateImage3DAnimation360(const std::string &file, const Vertex3D &position, float w, float h);
    static void CreateMesh3D(const std::string &file, const Vertex3D &position);
    static void CreateMesh3DAnimation(const std::string &file, const Vertex3D &position);
    static void CreateLightPoint(const Vertex3D &position, const Color &ambient, const Color &diffuse, const Color &specular);
    static void CreateLightSpot(const Vertex3D &position, const Color &ambient, const Color &diffuse, const Color &specular);
    static void CreateParticleEmitter(const Vertex3D &position, const Color &from, const Color &to);
};

#endif //BRAKEZA3D_OBJECTFACTORY_H