//
// Created by Eduardo on 20/12/2025.
//

#include "ObjectFactory.h"

#include "../2D/Image2D.h"
#include "../2D/Image2DAnimation.h"
#include "../3D/Image3D.h"
#include "../3D/Image3DAnimation.h"
#include "../3D/Image3DAnimation360.h"
#include "../3D/LightSpot.h"
#include "../3D/Mesh3DAnimation.h"
#include "../3D/ParticleEmitter.h"
#include "../Components/Components.h"
#include "../Render/JSONSerializerRegistry.h"
#include "../Render/TextWriter.h"
#include "../Threads/ThreadJobLoadImage2D.h"
#include "../Threads/ThreadJobLoadImage2DAnimation.h"
#include "../Threads/ThreadJobLoadImage3D.h"
#include "../Threads/ThreadJobLoadImage3DAnimation.h"
#include "../Threads/ThreadJobLoadImage3DAnimation360.h"
#include "../Threads/ThreadJobLoadLightPoint.h"
#include "../Threads/ThreadJobLoadLightSpot.h"
#include "../Threads/ThreadJobLoadMesh3D.h"
#include "../Threads/ThreadJobLoadMesh3DAnimation.h"
#include "../Threads/ThreadJobLoadObject.h"
#include "../Threads/ThreadJobLoadParticleEmitter.h"

void ObjectFactory::CreateObject3D(const Vertex3D &position)
{
    auto o = new Object3D();
    o->setPosition(position);

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadObject>(json));
}

void ObjectFactory::CreateImage2D(const std::string &file, int x, int y, int w, int h)
{
    auto *o = new Image2D(file, w, h);
    o->setScreenPosition(x, y);
    o->setName(Brakeza::UniqueObjectLabel("Image2D"));

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage2D>(o, json));
}

void ObjectFactory::CreateImage2DAnimation(const std::string &file, int x, int y, int sw, int sh, int frames, int fps)
{
    auto *o = new Image2DAnimation(
        x,
        y,
        false,
        new TextureAnimated(file,sw,sh, frames,fps)
    );

    o->setName(Brakeza::UniqueObjectLabel("Image2DAnimation"));

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage2DAnimation>(o, json));
}

void ObjectFactory::CreateImage3D(const std::string &file, const Vertex3D &position, float width, float height)
{
    auto *o = new Image3D(position, width, height, file);
    o->setName(Brakeza::UniqueObjectLabel("Image3D"));

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3D>(o, json));
}

void ObjectFactory::CreateImage3DAnimation(const std::string &file, const Vertex3D &position, float width, float height, float sw, float sh, int frames, int fps)
{
    auto *o = new Image3DAnimation(position, width, height);
    o->setName(Brakeza::UniqueObjectLabel("Image3DAnimation"));

    o->CreateAnimation(file,sw,sh,frames,fps);
    o->setAnimation(0);

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3DAnimation>(o, json));
}

void ObjectFactory::CreateImage3DAnimation360(const std::string &file, const Vertex3D &position, float w, float h)
{
    auto o = new Image3DAnimation360(position, w, h);
    o->setName(Brakeza::UniqueObjectLabel("Image3DAnimation360"));

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3DAnimation360>(o, json));
}

void ObjectFactory::CreateMesh3D(const std::string &file, const Vertex3D &position)
{
    auto *o = new Mesh3D(file);
    o->setName(Brakeza::UniqueObjectLabel("Mesh3D"));
    o->setPosition(position);

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadMesh3D>(o, json));
}

void ObjectFactory::CreateMesh3DAnimation(const std::string &file, const Vertex3D &position)
{
    auto *o = new Mesh3DAnimation();
    o->setSourceFile(file);
    o->setName(Brakeza::UniqueObjectLabel("Mesh3DAnimation"));
    o->setPosition(position);

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadMesh3DAnimation>(o, json));
}

void ObjectFactory::CreateLightPoint(const Vertex3D &position, const Color &ambient, const Color &diffuse, const Color &specular)
{
    auto o = new LightPoint(
        glm::vec4(ambient.toGLM(), 0),
        glm::vec4(diffuse.toGLM(), 0),
        glm::vec4(specular.toGLM(), 0),
        1.0f,
        0.09f,
        0.032f
    );
    o->setName(Brakeza::UniqueObjectLabel("LightPoint"));
    o->setPosition(position);

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadLightPoint>(o, json));
}

void ObjectFactory::CreateLightSpot(const Vertex3D &position, const Color &ambient, const Color &diffuse, const Color &specular)
{
    auto o = new LightSpot(
    glm::vec4(ambient.toGLM(), 0),
    glm::vec4(diffuse.toGLM(), 0),
    glm::vec4(specular.toGLM(), 0),
        1.0f,
        0.09f,
        0.032f,
        0.9763,
        0.9659
    );

    o->setName(Brakeza::UniqueObjectLabel("LightSpot"));
    o->setPosition(position);

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadLightSpot>(o, json));
}

void ObjectFactory::CreateParticleEmitter(const Vertex3D &position, const Color &from, const Color &to)
{
    auto *o = new ParticleEmitter(
            DEFAULT,
            nullptr,
            position,
            9999,
            from,
            to,
            ParticlesContext(
            0.0f,
            0.5f,
            1.5f,
            25.0f,
            1,
            10,
            125.0f,
            255.0f,
            1,
            1,
            0.99f
        ),
        nullptr
    );

    o->setName(Brakeza::UniqueObjectLabel("ParticleEmitter"));

    auto json = JSONSerializerRegistry::instance().serialize(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadParticleEmitter>(o, json));
}

TextWriter* ObjectFactory::CreateTextWriter(const std::string& fontFile)
{
    if (!Tools::FileExists(fontFile.c_str())) {
        Logging::Message("[TextWriter] Cannot open font file: %s", fontFile.c_str());

        return nullptr;
    }

    return new TextWriter(
        Components::get()->Window()->getRenderer(),
        TTF_OpenFont(fontFile.c_str(), 35)
    );
}
