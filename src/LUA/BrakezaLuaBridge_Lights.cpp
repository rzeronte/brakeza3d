#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegrationLights(sol::state &lua)
{
    lua.new_usertype<ScriptLUA>(
        "ScriptLUA",
        "content", sol::property(
                [](const ScriptLUA& script) { return script.content; },
                [](ScriptLUA& script, const std::string& newContent) { script.content = newContent; }
        ),
        "scriptFilename", sol::property(
                [](const ScriptLUA& script) { return script.getScriptFilename(); }
        ),
        "fileTypes", sol::property(
                [](const ScriptLUA& script) { return script.fileTypes; },
                [](ScriptLUA& script, const std::string& newFileTypes) { script.fileTypes = newFileTypes; }
        ),
        "UpdateFileTypes", &ScriptLUA::UpdateFileTypes,
        "getCode", &ScriptLUA::getCode,
        "dataTypesFileFor", &ScriptLUA::dataTypesFileFor
    );

    lua.new_usertype<ParticlesContext>("ParticlesContext",
    sol::constructors<
            ParticlesContext(),
                ParticlesContext(
                   float, float, float, int, float, float, int, int, float, float, float
           )
        >(),
        "GRAVITY", &ParticlesContext::GRAVITY,
        "PARTICLES_BY_SECOND", &ParticlesContext::PARTICLES_BY_SECOND,
        "PARTICLE_LIFESPAN", &ParticlesContext::PARTICLE_LIFESPAN,
        "SMOKE_ANGLE_RANGE", &ParticlesContext::SMOKE_ANGLE_RANGE,
        "MIN_VELOCITY", &ParticlesContext::MIN_VELOCITY,
        "MAX_VELOCITY", &ParticlesContext::MAX_VELOCITY,
        "MIN_ALPHA", &ParticlesContext::MIN_ALPHA,
        "MAX_ALPHA", &ParticlesContext::MAX_ALPHA,
        "POSITION_NOISE", &ParticlesContext::POSITION_NOISE,
        "VELOCITY_NOISE", &ParticlesContext::VELOCITY_NOISE,
        "DECELERATION_FACTOR", &ParticlesContext::DECELERATION_FACTOR,
        "PARTICLE_SIZE_SCALE", &ParticlesContext::PARTICLE_SIZE_SCALE,
        "SIZE_START", &ParticlesContext::SIZE_START,
        "SIZE_END", &ParticlesContext::SIZE_END,
        "ROTATION_RANDOM_START", &ParticlesContext::ROTATION_RANDOM_START,
        "ROTATION_SPEED_MIN", &ParticlesContext::ROTATION_SPEED_MIN,
        "ROTATION_SPEED_MAX", &ParticlesContext::ROTATION_SPEED_MAX,
        "ROTATION_RANDOM_DIR", &ParticlesContext::ROTATION_RANDOM_DIR,
        "ORIENTATION", &ParticlesContext::ORIENTATION,
        "FLUTTER_ANGLE", &ParticlesContext::FLUTTER_ANGLE,
        "FLUTTER_SPEED", &ParticlesContext::FLUTTER_SPEED,
        "SWIRL_SPEED", &ParticlesContext::SWIRL_SPEED,
        "SWIRL_PULL", &ParticlesContext::SWIRL_PULL,
        "SPIRAL_RADIAL_SPEED", &ParticlesContext::SPIRAL_RADIAL_SPEED,
        "SPIRAL_ARMS", &ParticlesContext::SPIRAL_ARMS,
        "SPIRAL_SPIN", &ParticlesContext::SPIRAL_SPIN,
        "SHAPE", &ParticlesContext::SHAPE,
        "SHAPE_RADIUS", &ParticlesContext::SHAPE_RADIUS,
        "SHAPE_HEIGHT", &ParticlesContext::SHAPE_HEIGHT,
        "SHAPE_TUBE_RADIUS", &ParticlesContext::SHAPE_TUBE_RADIUS,
        "SHAPE_SIZE_X", &ParticlesContext::SHAPE_SIZE_X,
        "SHAPE_SIZE_Y", &ParticlesContext::SHAPE_SIZE_Y,
        "SHAPE_SIZE_Z", &ParticlesContext::SHAPE_SIZE_Z,
        "SHAPE_SURFACE_ONLY", &ParticlesContext::SHAPE_SURFACE_ONLY,
        "SHAPE_DIRECTION", &ParticlesContext::SHAPE_DIRECTION,
        "SHAPE_MESH", &ParticlesContext::SHAPE_MESH,
        "defaultParticlesContext", &ParticlesContext::defaultParticlesContext
    );

    lua.new_usertype<ParticleEmitter>("ParticleEmitter",
        sol::base_classes, sol::bases<Object3D>(),
        "setContext", &ParticleEmitter::setContext,
        "setColorTo", &ParticleEmitter::setColorTo,
        "setColorFrom", &ParticleEmitter::setColorFrom,
        "setTexture", sol::overload(
            &ParticleEmitter::setTexture,
            [](ParticleEmitter& e, const std::string& path) {
                auto* img = new Image(FilePath::ImageFile(path));
                e.setTexture(img);
            }
        ),
        "setStopAdd", &ParticleEmitter::setStopAdd,
        "isActive", &ParticleEmitter::isActive,
        "getColorTo", &ParticleEmitter::getColorTo,
        "getColorFrom", &ParticleEmitter::getColorFrom,
        "getTexture", &ParticleEmitter::getTexture,
        "setGPUMode", &ParticleEmitter::setGPUMode,
        "isGPUMode", &ParticleEmitter::isGPUMode,
        "createAttachedLight", &ParticleEmitter::createAttachedLight,
        "removeAttachedLight", &ParticleEmitter::removeAttachedLight,
        "getAttachedLight", &ParticleEmitter::getAttachedLight,
        "getContext",      [](ParticleEmitter& e) -> ParticlesContext& { return e.getContextPointer(); },
        // detach iba por puntero a miembro directo (mismo problema que Object3D::isRemoved/
        // getLocalScriptVar de arriba): un ParticleEmitter* colgante que sobrevive a un reinicio
        // en una var de script Lua no es nullptr, ->detach() escribe igual sobre memoria ya
        // liberada. Crash real reproducido: ParticleEmitter::detach (ParticleEmitter.cpp:97)
        // escribiendo followTarget=nullptr sobre un puntero ya inválido.
        "detach", [](ParticleEmitter* obj, float fadeTime) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[ParticleEmitter] detach() llamado sobre un objeto invalido (Lua)"); return; }
            obj->detach(fadeTime);
        },
        "setFollowTarget", &ParticleEmitter::setFollowTarget,
        "getFollowTarget", &ParticleEmitter::getFollowTarget
    );

    lua.new_enum("EmitterShape",
        "POINT", EmitterShape::POINT,
        "BOX", EmitterShape::BOX,
        "SPHERE", EmitterShape::SPHERE,
        "CONE", EmitterShape::CONE,
        "CYLINDER", EmitterShape::CYLINDER,
        "TORUS", EmitterShape::TORUS,
        "MESH", EmitterShape::MESH
    );

    lua.new_enum("ParticleOrientation",
        "BILLBOARD", ParticleOrientation::BILLBOARD,
        "FLAT_GROUND", ParticleOrientation::FLAT_GROUND,
        "EMITTER_PLANE", ParticleOrientation::EMITTER_PLANE,
        "VERTICAL_BILLBOARD", ParticleOrientation::VERTICAL_BILLBOARD
    );

    lua.new_enum("EmitterShapeDirection",
        "AXIS", EmitterShapeDirection::AXIS,
        "OUTWARD", EmitterShapeDirection::OUTWARD
    );

    lua.new_enum("CollisionShape",
       "SIMPLE_SHAPE", SIMPLE_SHAPE,
       "TRIANGLE_MESH_SHAPE", TRIANGLE_MESH_SHAPE
    );

    lua.new_enum("CollisionGroups",
        "Player",          Config::Player,
        "Enemy",           Config::Enemy,
        "ProjectileGroup", Config::Projectile,
        "ProjectileEnemy", Config::ProjectileEnemy,
        "Health",          Config::Health,
        "Weapon",          Config::Weapon,
        "StaticWorld",     Config::StaticWorld,
        "AllFilter",       Config::AllFilter
    );

    lua.new_usertype<TextWriter>(
        "TextWriter",
        "writeTextTTFAutoSize", &TextWriter::WriteTextTTFAutoSize,
        "writeTextTTF", &TextWriter::WriteTextTTF,
        "writeTextTTFMiddleScreen", &TextWriter::WriteTextTTFMiddleScreen,
        "writeTTFCenterHorizontal", &TextWriter::WriteTTFCenterHorizontal,
        "getAlpha", &TextWriter::getAlpha,
        "setAlpha", &TextWriter::setAlpha,
        "setFont", &TextWriter::setFont,
        "beginTextCache", &TextWriter::beginTextCache,
        "endTextCache",   &TextWriter::endTextCache,
        "drawTextCache",  &TextWriter::drawTextCache,
        "writeTextAtlas",      &TextWriter::writeTextAtlas,
        "writeTextAtlasCache", &TextWriter::writeTextAtlasCache,
        "writeTextAtlasMiddleScreen",      &TextWriter::writeTextAtlasMiddleScreen,
        "writeTextAtlasCenterHorizontal",  &TextWriter::writeTextAtlasCenterHorizontal,
        "flushTextBatch",      &TextWriter::flushTextBatch,
        "flushTextBatchToFB",  &TextWriter::flushTextBatchToFB,
        // Ancho en px de ventana de un texto con el atlas de este writer (para centrar texto
        // dibujado por lotes con writeTextAtlasCache, que no tiene variante centrada)
        "measureTextWidthAtlas", &TextWriter::measureTextWidthAtlas,
        "buildGlyphAtlas", &TextWriter::buildGlyphAtlas,
        "getGlyphAtlas",   &TextWriter::getGlyphAtlas
    );

    lua.new_usertype<GlyphAtlas>("GlyphAtlas",
        "isBuilt", &GlyphAtlas::isBuilt,
        "getAtlasTexture", &GlyphAtlas::getAtlasTexture,
        "getLineHeight", &GlyphAtlas::getLineHeight,
        "getAscent", &GlyphAtlas::getAscent
    );

    lua.new_usertype<Image>("Image",
        "createEmpty",  [](int w, int h) { return Image::createEmpty(w, h); },
        "destroy",      &Image::destroy,
        "clearChannel", &Image::clearChannel,
        "fillCircle",     &Image::fillCircle,
        "fillCircleGrad", &Image::fillCircleGrad,
        "upload",         &Image::upload,
        "setClampToEdge", &Image::setClampToEdge
    );

}
