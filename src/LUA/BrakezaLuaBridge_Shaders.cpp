#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegrationShaders(sol::state &lua)
{
    lua.new_usertype<ShaderBaseCustomOGLCode>("ShaderOpenGLCustom",
            "setDataTypeValue", sol::overload(
                [](ShaderBaseCustomOGLCode& shader, const std::string& name, const sol::object& value) {
                    if (value.is<int>()) {
                        shader.setDataTypeValue(name, static_cast<float>(value.as<int>()));  // Convierte int a float
                    } else if (value.is<double>()) {
                        shader.setDataTypeValue(name, static_cast<float>(value.as<double>()));  // Convierte double a float
                    } else if (value.is<float>()) {
                        shader.setDataTypeValue(name, value.as<float>());
                    } else if (value.is<glm::vec2>()) {
                        shader.setDataTypeValue(name, value.as<glm::vec2>());
                    } else if (value.is<glm::vec3>()) {
                        shader.setDataTypeValue(name, value.as<glm::vec3>());
                    } else if (value.is<glm::vec4>()) {
                        shader.setDataTypeValue(name, value.as<glm::vec4>());
                    } else {
                        std::cerr << "Error: Tipo no soportado en setDataTypeValue para '" << name << "'\n";
                    }
                },
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, int)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, float)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, glm::vec2)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, glm::vec3)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, glm::vec4)>(&ShaderBaseCustomOGLCode::setDataTypeValue)
            ),
            "setTextureValue", [](ShaderBaseCustomOGLCode& s, const std::string& name, Image* img) {
                s.setDataTypeValue(name, ShaderOpenGLCustomDataValue(img));
            },
            "setEnabled", [](ShaderBaseCustomOGLCode& s, bool v) { s.setEnabled(v); },
            "isEnabled",  [](ShaderBaseCustomOGLCode& s) { return s.isEnabled(); }
    );

    // Shader de escena genérico (de nodos u otro tipo sin uniforms accesibles desde Lua)
    lua.new_usertype<ShaderBaseCustom>("ShaderCustom",
        "getLabel",   [](ShaderBaseCustom& s) { return s.getLabel(); },
        "setEnabled", [](ShaderBaseCustom& s, bool v) { s.setEnabled(v); },
        "isEnabled",  [](ShaderBaseCustom& s) { return s.isEnabled(); }
    );

    lua.new_usertype<glm::vec2>("vec2",
        sol::constructors<glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y
    );

    lua.new_usertype<glm::vec3>("vec3",
        sol::constructors<glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z
    );

    lua.new_usertype<glm::vec4>("vec4",
        sol::constructors<glm::vec4(float, float, float, float)>(),
        "x", &glm::vec4::x,
        "y", &glm::vec4::y,
        "z", &glm::vec4::z,
        "w", &glm::vec4::w
    );

    lua.new_usertype<CollisionInfo>("CollisionInfo",
        sol::constructors<CollisionInfo(void*, int, int)>(),
        "with", &CollisionInfo::with,
        "getSource", &CollisionInfo::getSource,
        "getBoneIndexMapping", &CollisionInfo::getBoneIndexMapping,
        "getObject", &CollisionInfo::getObject
    );

    lua.new_usertype<Sound3D>("Sound3D",
        sol::base_classes, sol::bases<Object3D>(),
        "setSoundLoop",    [](Sound3D* s, bool v){ s->loop = v; },
        "setInnerRadius",  [](Sound3D* s, float v){ s->innerRadius = v; },
        "setOuterRadius",  [](Sound3D* s, float v){ s->outerRadius = v; },
        "setBaseVolume",   [](Sound3D* s, int v){ s->baseVolume = v; }
    );

    lua.new_usertype<ObjectFactory>("ObjectFactory",
        "Object3D",           &ObjectFactory::CreateObject3D,
        "Image2D",            &ObjectFactory::CreateImage2D,
        "Image2DAnimation",   &ObjectFactory::CreateImage2DAnimation,
        "Image3D",            &ObjectFactory::CreateImage3D,
        "Image3DAnimation",   &ObjectFactory::CreateImage3DAnimation,
        "Image3DAnimation360",&ObjectFactory::CreateImage3DAnimation360,
        // ObjectFactory.Mesh3D(file, pos [, mipmaps]): el tercer argumento (opcional, por defecto false) activa mipmaps+trilineal+anisotropico
        "Mesh3D",             sol::overload(
                                  [](const std::string &f, const Vertex3D &p) { return ObjectFactory::CreateMesh3D(f, p, false); },
                                  [](const std::string &f, const Vertex3D &p, bool mip) { return ObjectFactory::CreateMesh3D(f, p, mip); }),
        "Mesh3DAnimation",    &ObjectFactory::CreateMesh3DAnimation,
        "LightPoint",         &ObjectFactory::CreateLightPoint,
        "LightSpot",          &ObjectFactory::CreateLightSpot,
        "ParticleEmitter",    &ObjectFactory::CreateParticleEmitter,
        "Projectile",         &ObjectFactory::CreateProjectile,
        "TextWriter",         &ObjectFactory::CreateTextWriter,
        "ScriptLUA",          &ObjectFactory::CreateScriptLUA,
        "Sound3D",            &ObjectFactory::CreateSound3D,
        // Bloqueante: reproduce el vídeo a pantalla completa hasta que acaba o se pulsa una tecla /
        // botón del ratón. fadeOut (opcional, segundos): fundido a negro al terminar.
        // Uso: ObjectFactory.PlayVideoCutscene("../assets/videos/x.mp4", 0.6)
        "PlayVideoCutscene",  [](const std::string &path, sol::optional<float> fadeOut) {
            VideoPlayer vp(path);
            vp.play(fadeOut.value_or(0.0f));
        }
    );

}
