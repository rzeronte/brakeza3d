//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_OBJECTFACTORY_H
#define BRAKEZA3D_OBJECTFACTORY_H

#include "../2D/Image2D.h"
#include "../2D/Image2DAnimation.h"
#include "../3D/Image3D.h"
#include "../3D/Image3DAnimation.h"
#include "../3D/Image3DAnimation360.h"
#include "../3D/LightSpot.h"
#include "../3D/Mesh3D.h"
#include "../3D/Mesh3DAnimation.h"
#include "../3D/ParticleEmitter.h"
#include "../Render/TextWriter.h"

class ObjectFactory {

    public:
    static Object3D* CreateObject3D(const Vertex3D &position);
    static Image2D* CreateImage2D(const std::string &file, int x, int y, int w, int h);
    static Image2DAnimation* CreateImage2DAnimation(const std::string &file, int x, int y, int sw, int sh, int frames, int fps);
    static Image3D* CreateImage3D(const std::string &file, const Vertex3D &position, float width, float height);
    static Image3DAnimation* CreateImage3DAnimation(const std::string &file, const Vertex3D &position, float width, float height, float sw, float sh, int frames, int fps);
    static Image3DAnimation360* CreateImage3DAnimation360(const std::string &file, const Vertex3D &position, float w, float h);
    static Mesh3D* CreateMesh3D(const std::string &file, const Vertex3D &position);
    static Mesh3DAnimation* CreateMesh3DAnimation(const std::string &file, const Vertex3D &position);
    static LightPoint* CreateLightPoint(const Vertex3D &position, const Color &ambient, const Color &diffuse, const Color &specular);
    static LightSpot* CreateLightSpot(const Vertex3D &position, const Color &ambient, const Color &diffuse, const Color &specular);
    static ParticleEmitter* CreateParticleEmitter(const Vertex3D &position, const Color &from, const Color &to);
    static TextWriter* CreateTextWriter(const std::string& fontFile);
    static ScriptLUA* CreateScriptLUA(const std::string& scriptFile);

};

#endif //BRAKEZA3D_OBJECTFACTORY_H