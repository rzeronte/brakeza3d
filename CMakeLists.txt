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
        # Main
        main.cpp

        # Root
        src/Brakeza.cpp include/Brakeza.h
        src/Brakeza.cpp include/Brakeza.h
        src/Config.cpp include/Config.h
        src/Loaders/ProjectLoader.cpp include/Loaders/ProjectLoader.h
        src/Loaders/SceneLoader.cpp include/Loaders/SceneLoader.h
        include/LUA/BrakezaLuaBridge.h
        include/SceneObjectTypes.h

        # 2D
        src/Misc/Button.cpp include/Misc/Button.h
        src/2D/Image2D.cpp                                  include/2D/Image2D.h
        src/2D/Image2DAnimation.cpp                         include/2D/Image2DAnimation.h
        src/2D/Line2D.cpp include/Render/Line2D.h
        src/Misc/MenuOption.cpp include/Misc/MenuOption.h
        src/2D/Point2D.cpp include/Render/Point2D.h
        src/2D/TextureAnimated.cpp include/Render/TextureAnimated.h
        src/2D/TextureAnimatedDirectional.cpp include/Render/TextureAnimatedDirectional.h
        src/2D/TextWriter.cpp include/Render/TextWriter.h
        include/Misc/ButtonsCallbacks.h

        # Collision
        src/Render/Collider.cpp include/Render/Collider.h
        src/Render/ColliderMesh3DAnimation.cpp include/Render/AssimpAnimationService.h
        src/Render/CollisionInfo.cpp include/Render/CollisionInfo.h

        # Components
        src/Components/Component.cpp                        include/Components/Component.h
        src/Components/ComponentCamera.cpp                  include/Components/ComponentCamera.h
        src/Components/ComponentCollisions.cpp              include/Components/ComponentCollisions.h
        src/Components/ComponentInput.cpp                   include/Components/ComponentInput.h
        src/Components/ComponentRender.cpp                  include/Components/ComponentRender.h
        src/Components/ComponentScripting.cpp               include/Components/ComponentScripting.h
        src/Components/Components.cpp include/Components/Components.h
        src/Components/ComponentSound.cpp                   include/Components/ComponentSound.h
        src/Components/ComponentWindow.cpp                  include/Components/ComponentWindow.h
        include/Components/Camera3D.h

        # cxxxopts
        cxxxopts/cxxxopts.h

        # GUI
        src/GUI/GUIManager.cpp                              include/GUI/GUIManager.h
        include/GUI/AddOns/GUIAddonMenu.h
        include/GUI/AddOns/GUIAddonObject3DProperties.h
        include/GUI/AddOns/GUIAddonObjects3D.h
        include/GUI/AddOns/GUIAddonProjectSetup.h
        include/GUI/AddOns/GUIAddonToolbar.h
        include/GUI/AddOns/GUIConsole.h

        # GUI/Objects
        include/GUI/Objects/Image2DAnimationGUI.h
        include/GUI/Objects/Image3DAnimation360GUI.h
        include/GUI/Objects/Image3DAnimationGUI.h
        include/GUI/Objects/Image3DGUI.h
        include/GUI/Objects/LightSpotGUI.h
        include/GUI/Objects/Mesh3DAnimationGUI.h
        include/GUI/Objects/Object3DGUI.h
        include/GUI/Objects/ScriptLuaGUI.h
        include/GUI/Objects/SwarmGUI.h

        # imgui
        imgui/imgui.cpp                                     imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/ImGuizmo.cpp                                  imgui/ImGuizmo.h
        imgui/imconfig.h
        imgui/imgui_internal.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h

        # imgui/backends
        imgui/backends/imgui_impl_opengl3.cpp               imgui/backends/imgui_impl_opengl3.h
        imgui/backends/imgui_impl_sdl2.cpp                  imgui/backends/imgui_impl_sdl2.h

        # Misc
        src/Misc/cJSON.c                                    include/Misc/cJSON.h
        src/Render/Color.cpp include/Render/Color.h
        src/Misc/Counter.cpp                                include/Misc/Counter.h
        src/Render/Grid3D.cpp include/Render/Grid3D.h
        src/Render/Octree.cpp include/Render/Octree.h
        src/Misc/SoundPackage.cpp                           include/Misc/SoundPackage.h
        src/Misc/TextureAtlas.cpp                           include/Misc/TextureAtlas.h
        src/Misc/TexturePackage.cpp                         include/Misc/TexturePackage.h
        src/Misc/TileManager.cpp                            include/Misc/TileManager.h
        src/Misc/Timer.cpp                                  include/Misc/Timer.h
        src/Misc/Tools.cpp                                  include/Misc/Tools.h
        src/Misc/ToolsJSON.cpp                              include/Misc/ToolsJSON.h
        include/Misc/PathFinding.h

        # Objects
        src/3D/Camera3D.cpp include/Components/Camera3D.h
        src/3D/Cube3D.cpp include/3D/Cube3D.h
        src/3D/Decal.cpp include/3D/Decal.h
        src/3D/Image3D.cpp include/3D/Image3D.h
        src/3D/Image3DAnimation.cpp include/3D/Image3DAnimation.h
        src/3D/Image3DAnimation360.cpp include/3D/Image3DAnimation360.h
        src/3D/LightPoint.cpp include/3D/LightPoint.h
        src/3D/LightSpot.cpp include/3D/LightSpot.h
        src/3D/Mesh3D.cpp include/3D/Mesh3D.h
        src/3D/Mesh3DAnimation.cpp include/3D/Mesh3DAnimation.h
        src/3D/Object3D.cpp include/3D/Object3D.h
        src/3D/ParticleEmitter.cpp include/3D/ParticleEmitter.h
        src/Misc/ScriptLUA.cpp include/Misc/ScriptLUA.h
        src/3D/Swarm.cpp include/3D/Swarm.h
        src/3D/TentacleIK.cpp include/3D/TentacleIK.h
        src/Render/Triangle3D.cpp include/Render/Triangle3D.h
        src/2D/Vector2D.cpp include/Render/Vector2D.h
        src/Render/Vector3D.cpp include/3D/Vector3D.h
        src/Render/Vertex3D.cpp include/3D/Vertex3D.h
        include/Render/ParticlesContext.h

        # OpenGL
        src/OpenGL/ShaderBaseOpenGL.cpp                     include/OpenGL/ShaderBaseOpenGL.h
        src/OpenGL/ShaderBaseOpenGLQuad.cpp                 include/OpenGL/ShaderBaseOpenGLQuad.h
        src/OpenGL/ShaderOGLBonesTransforms.cpp             include/OpenGL/ShaderOGLBonesTransforms.h
        src/OpenGL/ShaderOGLColor.cpp                       include/OpenGL/ShaderOGLColor.h
        src/OpenGL/ShaderOGLCustom.cpp                      include/OpenGL/ShaderOGLCustom.h
        src/OpenGL/ShaderOGLCustomMesh3D.cpp                include/OpenGL/ShaderOGLCustomMesh3D.h
        src/OpenGL/ShaderOGLCustomPostprocessing.cpp        include/OpenGL/ShaderOGLCustomPostprocessing.h
        src/OpenGL/ShaderOGLDepthMap.cpp                    include/OpenGL/ShaderOGLDepthMap.h
        src/OpenGL/ShaderOGLDOF.cpp                         include/OpenGL/ShaderOGLDOF.h
        src/OpenGL/ShaderOGLFog.cpp                         include/OpenGL/ShaderOGLFog.h
        src/OpenGL/ShaderOGLImage.cpp                       include/OpenGL/ShaderOGLImage.h
        src/OpenGL/ShaderOGLLightPass.cpp                   include/OpenGL/ShaderOGLLightPass.h
        src/OpenGL/ShaderOGLLine.cpp                        include/OpenGL/ShaderOGLLine.h
        src/OpenGL/ShaderOGLLine3D.cpp                      include/OpenGL/ShaderOGLLine3D.h
        src/OpenGL/ShaderOGLOutline.cpp                     include/OpenGL/ShaderOGLOutline.h
        src/OpenGL/ShaderOGLParticles.cpp                   include/OpenGL/ShaderOGLParticles.h
        src/OpenGL/ShaderOGLPoints.cpp                      include/OpenGL/ShaderOGLPoints.h
        src/OpenGL/ShaderOGLRenderDeferred.cpp              include/OpenGL/ShaderOGLRenderDeferred.h
        src/OpenGL/ShaderOGLRenderForward.cpp               include/OpenGL/ShaderOGLRenderForward.h
        src/OpenGL/ShaderOGLShading.cpp                     include/OpenGL/ShaderOGLShading.h
        src/OpenGL/ShaderOGLShadowPass.cpp                  include/OpenGL/ShaderOGLShadowPass.h
        src/OpenGL/ShaderOGLShadowPassDebugLight.cpp        include/OpenGL/ShaderOGLShadowPassDebugLight.h
        src/OpenGL/ShaderOGLTint.cpp                        include/OpenGL/ShaderOGLTint.h
        src/OpenGL/ShaderOGLWire.cpp                        include/OpenGL/ShaderOGLWire.h
        include/OpenGL/CommonOGLShaderTypes.h

        # Persistence
        src/Serializers/Image2DAnimationSerializer.cpp include/Serializers/Image2DAnimationSerializer.h
        src/Serializers/Image2DSerializer.cpp include/Serializers/Image2DSerializer.h
        src/Serializers/Image3DAnimation360Serializer.cpp include/Serializers/Image3DAnimation360Serializer.h
        src/Serializers/Image3DAnimationSerializer.cpp include/Serializers/Image3DAnimationSerializer.h
        src/Serializers/Image3DSerializer.cpp include/Serializers/Image3DSerializer.h
        src/Render/JSONSerializerRegistry.cpp include/Render/JSONSerializerRegistry.h
        src/Serializers/LightPointSerializer.cpp include/Serializers/LightPointSerializer.h
        src/Serializers/LightSpotSerializer.cpp include/Serializers/LightSpotSerializer.h
        src/Serializers/Mesh3DAnimationSerializer.cpp include/Serializers/Mesh3DAnimationSerializer.h
        src/Serializers/Mesh3DSerializer.cpp include/Serializers/Mesh3DSerializer.h
        src/Serializers/Object3DSerializer.cpp include/Serializers/Object3DSerializer.h
        src/Serializers/ParticleEmmitterSerializer.cpp include/Serializers/ParticleEmmitterSerializer.h
        include/Render/JSONSerializer.h

        # Physics
        src/3D/RayCollisionable.cpp include/3D/RayCollisionable.h
        src/3D/SimplePendulum.cpp include/3D/SimplePendulum.h
        src/3D/Projectile.cpp include/3D/Projectile.h

        # Render
        src/Render/Drawable.cpp                             include/Render/Drawable.h
        src/Render/Frustum.cpp                              include/Render/Frustum.h
        src/Render/Image.cpp                                include/Render/Image.h
        src/Misc/Logging.cpp include/Misc/Logging.h
        src/Render/M3.cpp                                   include/Render/M3.h
        src/Misc/ToolsMaths.cpp include/Misc/ToolsMaths.h
        src/Render/PhysicsDebugDraw.cpp                     include/Render/PhysicsDebugDraw.h
        src/Render/Plane.cpp                                include/Render/Plane.h
        src/Render/Transforms.cpp                           include/Render/Transforms.h
        include/Render/AABB3D.h
        src/GUI/Objects/Mesh3DGUI.cpp
        src/GUI/Objects/Object3DGUI.cpp
        src/GUI/Objects/ScriptLuaGUI.cpp
        src/GUI/Objects/ShadersGUI.cpp
        include/GUI/Objects/ShadersGUI.h
        src/GUI/Objects/FilesystemGUI.cpp
        include/GUI/Objects/FilesystemGUI.h
        src/3D/Cone3D.cpp                   include/3D/Cone3D.h
        include/Render/Profiler.cpp
        include/Render/Profiler.h
        src/GUI/AddOns/GUIAddonMenu.cpp
        src/GUI/AddOns/GUIAddonToolbar.cpp
        include/GUI/GUI.h
        src/GUI/GUI.cpp
        include/GUI/Objects/IconsGUI.h
        include/GUI/Objects/IconsGUI.h
        src/GUI/AddOns/GUIAddonObject3DProperties.cpp
        src/GUI/AddOns/GUIAddonProjectSetup.cpp
        src/GUI/AddOns/GUIAddonObjects3D.cpp
        src/GUI/AddOns/GUIConsole.cpp
        src/GUI/Objects/Mesh3DGUIAnimation.cpp
        src/OpenGL/ShaderOGLGrid.cpp        include/OpenGL/ShaderOGLGrid.h
        src/Render/ThreadPool.cpp
        include/Render/ThreadPool.h
        include/Threads/ThreadJobBase.h
        include/Threads/ThreadJobReadFileScene.h
        include/Threads/ThreadJobReadSceneScript.h
        include/Threads/ThreadJobReadSceneShaders.h
        include/Threads/ThreadJobLoadMesh3D.h
        include/Threads/ThreadJobLoadObject.h
        include/Threads/ThreadJobLoadImage.h
        include/Threads/ThreadJobLoadMesh3DAnimation.h
        include/Threads/ThreadJobLoadImage3D.h
        include/Threads/ThreadJobLoadLightPoint.h
        include/Threads/ThreadJobLoadLightSpot.h
        include/Threads/ThreadJobLoadParticleEmitter.h
        include/Threads/ThreadJobLoadImage2D.h
        include/Threads/ThreadJobLoadImage3DAnimation360.h
        include/Threads/ThreadJobLoadImage2DAnimation.h
        include/Threads/ThreadJobLoadImage3DAnimation.h
        include/LUA/ObjectFactory.h
        include/LUA/ObjectFactory.h
        include/Threads/ThreadJobStepSimulation.h
        include/Threads/ThreadJobClearScene.h
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