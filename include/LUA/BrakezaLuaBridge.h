//
// Created by eduardo on 5/10/23.
//

#ifndef BRAKEZA3D_LUAINTEGRATION_H
#define BRAKEZA3D_LUAINTEGRATION_H

#include <chrono>
#include "ObjectFactory.h"
#include <iostream>
#include "../../sol/sol.hpp"
#include "../Cache/ImageCache.h"
#include "../3D/Vertex3D.h"
#include "../Misc/ScriptLUA.h"
#include "../Components/ComponentCamera.h"
#include "../Components/Components.h"
#include "../Brakeza.h"
#include "../Render/GlyphAtlas.h"
#include "../2D/Image2D.h"
#include "../2D/Image2DAnimation.h"
#include "../3D/Mesh3DAnimation.h"
#include "../3D/Image3D.h"
#include "../3D/LightPoint.h"
#include "../3D/LightSpot.h"
#include "../3D/Image3DAnimation.h"
#include "../3D/Image3DAnimation360.h"
#include "../3D/ParticleEmitter.h"
#include "../Render/Profiler.h"
#include "../3D/Sound3D.h"
#include "../Misc/VideoPlayer.h"
#include "../Threads/ThreadJobPathfinding.h"
#include "../Threads/ThreadJobReadObjectScript.h"
#include "../Loaders/FBXLightLoader.h"
#include "../Loaders/Scene.h"
#include "../Loaders/SceneLoader.h"
#include "../3D/Projectile.h"

// Undefine Windows API macro that conflicts with ComponentSound::PlaySound
#ifdef PlaySound
#undef PlaySound
#endif

// LUAIntegration se reparte en varias unidades de compilacion (src/LUA/BrakezaLuaBridge_*.cpp): todos los bindings de sol2 en
// una sola unidad pasaban de 7 GB de RAM en el compilador.
void LUAIntegration(sol::state &lua);
void LUAIntegrationBasics(sol::state &lua);
void LUAIntegrationComponents(sol::state &lua);
void LUAIntegrationBrakeza(sol::state &lua);
void LUAIntegrationMeshes(sol::state &lua);
void LUAIntegrationLights(sol::state &lua);
void LUAIntegrationShaders(sol::state &lua);
void LUAIntegrationTables(sol::state &lua);

#endif //BRAKEZA3D_LUAINTEGRATION_H
