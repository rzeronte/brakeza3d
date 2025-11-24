cmake_minimum_required(VERSION 3.10)
project(Brakeza3D)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-O0 -Wall -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -w")
endif()

# SDL
set(SDL2_LIBRARY "SDL2")
set(SDL2_IMAGE_LIBRARY "SDL2_image")
set(SDL2_TTF_LIBRARY "SDL2_ttf")
set(SDL2_MIXER_LIBRARY "SDL2_mixer")
# BULLET
set(BULLET_SOFTBODY_LIBRARY "BulletSoftBody")
set(BULLET_DYNAMICS_LIBRARY "BulletDynamics")
set(BULLET_COLLISION_LIBRARY "BulletCollision")
set(BULLET_LINEARMATH_LIBRARY "LinearMath")
#ASSIMP
set(ASSIMP_LIBRARY "assimp")
#LUA
if(WIN32)
    set(LUA_LIBRARY "lua52")
endif()
if(UNIX)
    set(LUA_LIBRARY "lua5.2")
endif()
#OPENGL
if(WIN32)
    set(OPENGL_LIBRARY "opengl32")
endif()
if(UNIX)
    set(OPENGL_LIBRARY "GL")
endif()
#GLEW
if(WIN32)
    set(GLEW_LIBRARY "glew32")
endif()
if(UNIX)
    set(GLEW_LIBRARY "GLEW")
endif()

if(WIN32)
    set(SDL2_INCLUDE_DIRS "${CMAKE_CXX_COMPILER}/../../include/SDL2")
    set(BULLET_INCLUDE_DIRS "${CMAKE_CXX_COMPILER}/../../include/bullet")
    set(ASSIMP_INCLUDE_DIRS "${CMAKE_CXX_COMPILER}/../../include/assimp")
    set(LUA_INCLUDE_DIR "${CMAKE_CXX_COMPILER}/../../include/")
    set(OPENGL_INCLUDE_DIR "${CMAKE_CXX_COMPILER}/../../include/")
    set(GLEW_INCLUDE_DIRS "${CMAKE_CXX_COMPILER}/../../include/")
endif()

if(UNIX AND NOT APPLE)
    set(SDL2_INCLUDE_DIRS "/usr/include/SDL2")
    set(BULLET_INCLUDE_DIRS "/usr/include/bullet")
    set(ASSIMP_INCLUDE_DIRS "/usr/include/assimp")
    set(LUA_INCLUDE_DIR "/usr/include/")
    set(OPENGL_INCLUDE_DIR "/usr/include/")
    set(GLEW_INCLUDE_DIRS "/usr/include/")
endif()

set(DEARIMGUI_INCLUDE_DIRS imgui)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${DEARIMGUI_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR}
)

add_executable(Brakeza3D
    main.cpp

    src/LUAIntegration.h
    src/Brakeza3D.cpp                               include/Brakeza3D.h
    src/Objects/Mesh3D.cpp                          include/Objects/Mesh3D.h
    src/Objects/Triangle3D.cpp                      include/Objects/Triangle3D.h
    src/Objects/Vertex3D.cpp                        include/Objects/Vertex3D.h
    src/Objects/Vector3D.cpp                        include/Objects/Vector3D.h
        src/2D/Point2D.cpp include/2D/Point2D.h
        src/2D/Line2D.cpp include/2D/Line2D.h
    src/Objects/Object3D.cpp                        include/Objects/Object3D.h
        src/Objects/LightPoint.cpp include/Objects/LightPoint.h
        src/Objects/Image3DAnimation8Directions.cpp include/Objects/Image3DAnimation8Directions.h
        src/Objects/Image3DAnimation.cpp include/Objects/Image3DAnimation.h
    src/Objects/Decal.cpp                           include/Objects/Decal.h
    src/Objects/Cube3D.cpp                          include/Objects/Cube3D.h
    src/Objects/Camera3D.cpp                        include/Components/Camera3D.h
    src/Objects/Mesh3DAnimation.cpp                 include/Objects/Mesh3DAnimation.h
        src/Objects/ParticleEmitter.cpp                 include/Objects/ParticleEmitter.h
    src/Objects/ParticleEmitterFireworks.cpp        include/Objects/ParticleEmitterFireworks.h
    src/Objects/TentacleIK.cpp                      include/Objects/TentacleIK.h
    src/Objects/ScriptLUA.cpp                       include/Objects/ScriptLUA.h
    src/Objects/Swarm.cpp                           include/Objects/Swarm.h
        src/Objects/LightSpot.cpp include/Objects/LightSpot.h
    src/Objects/Image3D.cpp                         include/Objects/Image3D.h

    src/Misc/TextureAtlas.cpp                       include/Misc/TextureAtlas.h
    src/Misc/Timer.cpp                              include/Misc/Timer.h
    src/Misc/Tools.cpp                              include/Misc/Tools.h
    src/Misc/Octree.cpp                             include/Misc/Octree.h
    src/Misc/Counter.cpp                            include/Misc/Counter.h
    src/Misc/TexturePackage.cpp                     include/Misc/TexturePackage.h
    src/Misc/SoundPackage.cpp                       include/Misc/SoundPackage.h
        src/Render/Image.cpp include/Render/Image.h
        src/SceneLoader.cpp include/SceneLoader.h
    src/Misc/Grid3D.cpp                             include/Misc/Grid3D.h

        include/BrakezaSetup.h src/BrakezaSetup.cpp

    include/Misc/Color.h                            src/Misc/Color.cpp
    include/Misc/cJSON.h                            src/Misc/cJSON.c

    include/Render/AABB3D.h
    include/Render/Transforms.h                     src/Render/Transforms.cpp

    include/GUI/GUI.h
    include/GUI/GUIManager.h                        src/GUI/GUIManager.cpp

    src/Render/Frustum.cpp                          include/Render/Frustum.h
    src/Render/Plane.cpp                            include/Render/Plane.h
    src/Render/Drawable.cpp                         include/Render/Drawable.h
    src/Render/Maths.cpp                            include/Render/Maths.h
        src/Objects/BSPMap.cpp include/Objects/BSPMap.h
    src/Render/PhysicsDebugDraw.cpp                 include/Render/PhysicsDebugDraw.h
        src/Render/M3.cpp                               include/Render/M3.h
    src/Render/Logging.cpp                          include/Render/Logging.h

    src/2D/MenuOption.cpp                           include/2D/MenuOption.h
    src/2D/TextureAnimatedDirectional.cpp           include/2D/TextureAnimatedDirectional.h
    src/2D/TextureAnimated.cpp                      include/2D/TextureAnimated.h
    src/2D/TextWriter.cpp                           include/2D/TextWriter.h
    src/2D/Button.cpp                               include/2D/Button.h
    src/2D/Image2DAnimation.cpp                     include/2D/Image2DAnimation.h
    src/2D/Image2DAnimation.cpp                     include/2D/Image2DAnimation.h
    src/2D/Image2D.cpp                              include/2D/Image2D.h

        src/Components/ComponentsManager.cpp include/Components/ComponentsManager.h
    src/Components/ComponentScripting.cpp           include/Components/ComponentScripting.h
    src/Components/ComponentCamera.cpp              include/Components/ComponentCamera.h
    src/Components/ComponentCollisions.cpp          include/Components/ComponentCollisions.h
    src/Components/Component.cpp                    include/Components/Component.h
    src/Components/ComponentRender.cpp              include/Components/ComponentRender.h
    src/Components/ComponentWindow.cpp              include/Components/ComponentWindow.h
    src/Components/ComponentInput.cpp               include/Components/ComponentInput.h
    src/Components/ComponentSound.cpp               include/Components/ComponentSound.h

        src/OpenGL/ShaderOGLDOF.cpp include/OpenGL/ShaderOGLDOF.h
        src/OpenGL/ShaderOGLDepthMap.cpp include/OpenGL/ShaderOGLDepthMap.h
        src/OpenGL/ShaderOGLFog.cpp include/OpenGL/ShaderOGLFog.h
        src/OpenGL/ShaderBaseOpenGLQuad.cpp include/OpenGL/ShaderBaseOpenGLQuad.h
        src/OpenGL/ShaderOGLColor.cpp include/OpenGL/ShaderOGLColor.h
        src/OpenGL/ShaderBaseOpenGL.cpp include/OpenGL/ShaderBaseOpenGL.h
        src/OpenGL/ShaderOGLImage.cpp include/OpenGL/ShaderOGLImage.h
        src/OpenGL/ShaderOGLRenderForward.cpp include/OpenGL/ShaderOGLRenderForward.h
        src/OpenGL/ShaderOGLLine.cpp include/OpenGL/ShaderOGLLine.h
        src/OpenGL/ShaderOGLWire.cpp include/OpenGL/ShaderOGLWire.h
        src/OpenGL/ShaderOGLShading.cpp include/OpenGL/ShaderOGLShading.h
        src/OpenGL/ShaderOGLPoints.cpp include/OpenGL/ShaderOGLPoints.h
        src/OpenGL/ShaderOGLCustom.cpp include/OpenGL/ShaderOGLCustom.h
        src/OpenGL/ShaderOGLTint.cpp include/OpenGL/ShaderOGLTint.h

    src/Collisions/SimplePendulum.cpp               include/Physics/SimplePendulum.h
    src/Collisions/Collider.cpp                     include/Collision/Collider.h
    src/Collisions/RayCollisionable.cpp             include/Physics/RayCollisionable.h

    src/Behaviors/Object3DBehavior.cpp              include/Behaviors/Object3DBehavior.h
    src/Behaviors/RotatableToTarget.cpp             include/Behaviors/RotatableToTarget.h

    src/FXEffect/FXOutliner.cpp                     include/FXEffect/FXOutliner.h
    src/FXEffect/FXColorTint.cpp                    include/FXEffect/FXColorTint.h
    src/FXEffect/FXBlink.cpp                        include/FXEffect/FXBlink.h

    imgui/imconfig.h
    imgui/imgui.h                                   imgui/imgui.cpp
    imgui/imgui_demo.cpp                            imgui/imgui_draw.cpp
    imgui/imgui_internal.h                          imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h                          imgui/imstb_textedit.h
    imgui/imstb_truetype.h                          imgui/imgui_tables.cpp

    imgui/backends/imgui_impl_sdl2.h                imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.h             imgui/backends/imgui_impl_opengl3.cpp

    imgui/ImGuizmo.h                                imgui/ImGuizmo.cpp

    include/2D/ButtonsCallbacks.h
    include/Physics/Projectile.h                    include/Physics/Projectile.cpp

    include/GUI/GUIConsole.h
        include/GUI/GUIAddonProjectSettings.h
        include/GUI/GUIAddonMenu.h
        include/GUI/GUIAddonToolbar.h
        include/GUI/GUIAddonObjects3D.h include/GUI/GUIAddonObject3DProperties.h

        src/Misc/TileManager.cpp                        include/Misc/TileManager.h

    include/OpenGL/FXEffectOpenGLObject.h
    include/OpenGL/FXBase.h
    include/OpenGL/FXOpenGL.h
        include/OpenGL/ShaderOGLParticles.h src/OpenGL/ShaderOGLParticles.cpp
        include/OpenGL/ShaderOGLOutline.h src/OpenGL/ShaderOGLOutline.cpp

        include/OpenGL/OGLShaderTypes.h
    include/Objects/ParticlesContext.h
    include/Objects/Vector2D.cpp                    include/Objects/Vector2D.h

    include/Render/ObjectShaderOpenCL.cpp
    src/Misc/ToolsJSON.cpp                          include/Misc/ToolsJSON.h
        src/OpenGL/ShaderOGLLine3D.cpp include/OpenGL/ShaderOGLLine3D.h
        src/ProjectLoader.cpp include/ProjectLoader.h
    include/cxxxopts.h
    include/Misc/PathFinding.h
        src/OpenGL/ShaderOGLCustomPostprocessing.cpp include/OpenGL/ShaderOGLCustomPostprocessing.h
        src/OpenGL/ShaderOGLCustomMesh3D.cpp include/OpenGL/ShaderOGLCustomMesh3D.h
        src/OpenGL/ShaderOGLBonesTransforms.cpp
        include/OpenGL/ShaderOGLBonesTransforms.h
        src/Collisions/ColliderMesh3DAnimation.cpp
        include/Collision/AssimpAnimationService.h
        include/Collision/CollisionInfo.cpp
        include/Collision/CollisionInfo.h
        src/OpenGL/ShaderOGLRenderDeferred.cpp
        include/OpenGL/ShaderOGLRenderDeferred.h
        src/OpenGL/ShaderOGLLightPass.cpp
        include/OpenGL/ShaderOGLLightPass.h
        src/OpenGL/ShaderOGLShadowPass.cpp
        include/OpenGL/ShaderOGLShadowPass.h
        src/OpenGL/ShaderOGLShadowPassDebugLight.cpp
        include/OpenGL/ShaderOGLShadowPassDebugLight.h
        include/Persistence/JSONSerializer.h
        include/Persistence/JSONSerializerRegistry.h
        include/Persistence/Object3DSerializer.h
        src/Persistence/Object3DSerializer.cpp
        src/Persistence/JSONSerializerRegistry.cpp
        src/Persistence/Mesh3DSerializer.cpp
        include/Persistence/Mesh3DSerializer.h
        src/Persistence/Mesh3DAnimationSerializer.cpp
        include/Persistence/Mesh3DAnimationSerializer.h
        src/Persistence/LightPointSerializer.cpp
        include/Persistence/LightPointSerializer.h
        src/Persistence/LightSpotSerializer.cpp
        include/Persistence/LightSpotSerializer.h
        src/Persistence/Image3DSerializer.cpp
        include/Persistence/Image3DSerializer.h
        src/Persistence/Image2DSerializer.cpp
        include/Persistence/Image2DSerializer.h
        src/Persistence/ParticleEmmitterSerializer.cpp
        include/Persistence/ParticleEmmitterSerializer.h
        src/Persistence/Image2DAnimationSerializer.cpp
        include/Persistence/Image2DAnimationSerializer.h
        src/Persistence/Image3DAnimationSerializer.cpp
        include/Persistence/Image3DAnimationSerializer.h
        src/Persistence/Image3DAnimation8DirectionsSerializer.cpp
        include/Persistence/Image3DAnimation8DirectionsSerializer.h
        include/GUI/Objects/Image3DGUI.h
        include/GUI/Objects/Object3DGUI.h
        include/GUI/Objects/SwarmGUI.h
        include/GUI/Objects/Image2DAnimationGUI.h
        include/GUI/Objects/Image3DAnimation8DirectionsGUI.h
        include/GUI/Objects/LightSpotGUI.h
        include/GUI/Objects/Mesh3DAnimationGUI.h
        include/GUI/Objects/ScriptLuaGUI.h
        include/GUI/Objects/Image3DAnimationGUI.h
)

target_link_libraries(Brakeza3D
    pthread
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    ${BULLET_SOFTBODY_LIBRARY}
    ${BULLET_DYNAMICS_LIBRARY}
    ${BULLET_COLLISION_LIBRARY}
    ${BULLET_LINEARMATH_LIBRARY}
    ${ASSIMP_LIBRARY}
    ${LUA_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
)