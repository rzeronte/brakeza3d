cmake_minimum_required(VERSION 3.10)
project(Brakeza3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS_DEBUG "-O3 -fsanitize=leak")

set(SDL2_INCLUDE_DIRS /usr/include/SDL2)
set(BULLET_INCLUDE_DIRS /usr/include/bullet)
set(ASSIMP_INCLUDE_DIRS /usr/include/assimp)
set(GLM_INCLUDE_DIRS /usr/include/glm)
set(DEARIMGUI_INCLUDE_DIRS imgui)

find_package(OpenGL REQUIRED)

if (WIN32)
    set(BASE_INCLUDE_DIRECTORY C:\\Users\\Eduardo\\Desktop\\include\\)
    set(SDL2_INCLUDE_DIRS C:\\Users\\Eduardo\\Desktop\\include\\SDL2)
    set(ASSIMP_INCLUDE_DIRS C:\\Users\\Eduardo\\Desktop\\include\\assimp)
    set(GLM_INCLUDE_DIRS C:\\Users\\Eduardo\\Desktop\\include\\glm)
    set(BULLET_INCLUDE_DIRS C:\\Users\\eduardo\\Desktop\\include\\bullet)
    set(OPENCL_LIBRARIES_DIR C:\\Users\\Eduardo\\Desktop\\include\\CL)
endif (WIN32)

include_directories(
    ${BASE_INCLUDE_DIRECTORY}
    ${SDL2_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${DEARIMGUI_INCLUDE_DIRS}
    ${OPENCL_LIBRARIES_DIR}
    ${OPENGL_INCLUDE_DIR}
)

add_executable(Brakeza3D
    main.cpp
    src/Components/ComponentCamera.cpp include/Components/ComponentCamera.h
    src/Objects/Mesh3D.cpp include/Objects/Mesh3D.h
    src/Objects/Triangle3D.cpp include/Objects/Triangle3D.h
    src/Objects/Vertex3D.cpp include/Objects/Vertex3D.h
    src/Objects/Vector3D.cpp include/Objects/Vector3D.h
    src/Objects/Point2D.cpp include/Objects/Point2D.h
    src/Misc/Tools.cpp include/Misc/Tools.h
    src/Objects/Line2D.cpp include/Objects/Line2D.h
    include/Misc/Color.h src/Misc/Color.cpp
    include/EngineSetup.h src/EngineSetup.cpp
        include/LUAManager.h src/EngineBuffers.cpp
    src/Render/Frustum.cpp include/Render/Frustum.h
    src/Render/Plane.cpp include/Render/Plane.h
    src/Objects/Object3D.cpp include/Objects/Object3D.h
    src/Misc/Timer.cpp include/Misc/Timer.h
    include/Render/Transforms.h src/Render/Transforms.cpp
    src/Render/Drawable.cpp include/Render/Drawable.h
    src/Objects/LightPoint3D.cpp include/Objects/LightPoint3D.h
    src/Misc/TextureAtlas.cpp include/Misc/TextureAtlas.h
    src/Render/Billboard.cpp include/Render/Billboard.h
    src/Render/M3.cpp include/Render/M3.h
    src/Render/Logging.cpp include/Render/Logging.h
    include/Render/Tile.h
    src/Objects/SpriteDirectional3D.cpp include/Objects/SpriteDirectional3D.h
    src/2D/TextureAnimatedDirectional.cpp include/2D/TextureAnimatedDirectional.h
    src/Objects/Sprite3D.cpp include/Objects/Sprite3D.h
    src/2D/TextureAnimated.cpp include/2D/TextureAnimated.h
    src/Render/Maths.cpp include/Render/Maths.h
    src/Render/BSPMap.cpp include/Render/BSPMap.h
    src/Collisions/Mesh3DBody.cpp include/Physics/Mesh3DBody.h
    src/Render/PhysicsDebugDraw.cpp include/Render/PhysicsDebugDraw.h
    src/Collisions/Mesh3DGhost.cpp include/Physics/Mesh3DGhost.h
    include/Misc/cJSON.h src/Misc/cJSON.c
    src/Collisions/SpriteDirectional3DBody.cpp include/Physics/SpriteDirectional3DBody.h
    src/2D/MenuOption.cpp include/2D/MenuOption.h
    src/Brakeza3D.cpp include/Brakeza3D.h
    src/Components/ComponentCollisions.cpp include/Components/ComponentCollisions.h
    src/Collisions/Projectile3DBody.cpp include/Physics/Projectile3DBody.h
    src/Objects/Decal.cpp include/Objects/Decal.h
    src/Collisions/Sprite3DBody.cpp include/Physics/Sprite3DBody.h
    src/Collisions/Body.cpp include/Physics/Body.h
    src/Objects/Cube3D.cpp include/Objects/Cube3D.h
    src/Misc/Counter.cpp include/Misc/Counter.h
    src/Misc/TexturePackage.cpp include/Misc/TexturePackage.h
    src/Misc/SoundPackage.cpp include/Misc/SoundPackage.h
        src/Collisions/BillboardBody.cpp include/Physics/BillboardBody.h
    src/Components/Component.cpp include/Components/Component.h
    src/Components/ComponentRender.cpp include/Components/ComponentRender.h
    src/Components/ComponentWindow.cpp include/Components/ComponentWindow.h
    src/Components/ComponentInput.cpp include/Components/ComponentInput.h
    src/Components/ComponentSound.cpp include/Components/ComponentSound.h
    src/Objects/Camera3D.cpp include/Components/Camera3D.h
    src/ComponentsManager.cpp include/ComponentsManager.h
    src/Objects/Mesh3DAnimated.cpp include/Objects/Mesh3DAnimated.h
    src/Objects/Mesh3DAnimatedCollection.cpp include/Objects/Mesh3DAnimatedCollection.h
    src/Collisions/BSPCollider.cpp src/Collisions/BSPCollider.h
    src/2D/TextWriter.cpp include/2D/TextWriter.h
    src/Collisions/Ghost.cpp include/Physics/Ghost.h
    include/Render/AABB3D.h
    src/Misc/Octree.cpp include/Misc/Octree.h
    include/Misc/Grid3D.cpp include/Misc/Grid3D.h
    src/Misc/PathFinder.cpp include/Misc/PathFinder.h

    include/GUI/GUI.h
        include/GUI/GUIManager.h

    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/imgui_tables.cpp

    imgui/backends/imgui_impl_sdl2.h
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3.cpp

    include/Render/Fragment.cpp include/Render/Fragment.h
    src/Render/M4.cpp include/Render/M4.h
    src/Objects/Vertex4D.cpp include/Objects/Vertex4D.h
        src/Objects/ParticleEmitter.cpp include/Objects/ParticleEmitter.h
    src/Collisions/SimplePendulum.cpp include/Physics/SimplePendulum.h
        include/OpenGL/FXEffectBase.h
    src/Misc/Image.cpp include/Misc/Image.h
    src/Collisions/Collisionable.cpp src/Collisions/Collisionable.h
    src/2D/Button.cpp include/2D/Button.h
    include/2D/ButtonsCallbacks.h include/FXEffect/FXBlink.h

    src/Behaviors/Object3DBehavior.cpp include/Behaviors/Object3DBehavior.h
        src/Objects/ParticleEmitterFireworks.cpp include/Objects/ParticleEmitterFireworks.h

    include/Objects/Vector2D.cpp include/Objects/Vector2D.h
        include/OpenGL/FXEffectOpenGL.h
        src/FXEffect/FXOutliner.cpp include/FXEffect/FXOutliner.h
        src/FXEffect/FXColorTint.cpp include/FXEffect/FXColorTint.h
    src/Collisions/Mesh3DAnimatedGhost.cpp include/Physics/Mesh3DAnimatedGhost.h
    src/Collisions/RayCollisionable.cpp include/Physics/RayCollisionable.h

        src/Objects/Swarm.cpp include/Objects/Swarm.h
        src/Behaviors/RotatableToTarget.cpp include/Behaviors/RotatableToTarget.h

        src/FXEffect/FXBlink.cpp
        include/Objects/OpenCLTypes.h
    include/Physics/Projectile.h

        src/2D/Sprite2D.cpp include/2D/Sprite2D.h

        src/2D/Sprite2D.cpp
        include/2D/Sprite2D.h
        src/Objects/TentacleIK.cpp
        include/Objects/TentacleIK.h
        src/Objects/ScriptLUA.cpp
        include/Objects/ScriptLUA.h
        include/GUI/GUIConsole.h
        src/LUAIntegration.h
        include/Misc/SharedLUAContext.h
        src/Misc/SceneLoader.cpp
        include/Misc/SceneLoader.h
        include/Render/ObjectShaderOpenCL.cpp
        include/OpenGL/FXEffectOpenGLObject.h
        include/Misc/TileManager.cpp
        include/Misc/TileManager.h
        src/OpenGL/ShaderOpenGL.cpp
        include/OpenGL/ShaderOpenGL.h
        include/OpenGL/ShaderOpenGLParticles.h
        src/OpenGL/ShaderOpenGLParticles.cpp
        src/OpenGL/ShaderOpenGLImage.cpp
        include/OpenGL/ShaderOpenGLImage.h
        src/OpenGL/ShaderOpenGLRender.cpp
        include/OpenGL/ShaderOpenGLRender.h
        include/Objects/OpenGLShaderTypes.h
        src/OpenGL/ShaderOpenGLLine.cpp
        include/OpenGL/ShaderOpenGLLine.h
        src/Objects/SpotLight3D.cpp
        include/Objects/SpotLight3D.h
        src/OpenGL/ShaderOpenGLWireframe.cpp
        include/OpenGL/ShaderOpenGLWireframe.h
        src/OpenGL/ShaderOpenGLShading.cpp
        include/OpenGL/ShaderOpenGLShading.h
        src/OpenGL/ShaderOpenGLPoints.cpp
        include/OpenGL/ShaderOpenGLPoints.h
        src/OpenGL/ShaderOpenGLCustom.cpp
        include/OpenGL/ShaderOpenGLCustom.h
        src/OpenGL/ShaderOpenGLCustom.cpp
        src/OpenGL/ShaderOpenGLOutline.cpp
        include/OpenGL/ShaderOpenGLOutline.h
        src/OpenGL/ShaderOpenGLColor.cpp
        include/OpenGL/ShaderOpenGLColor.h


        src/OpenGL/ShaderOpenGLDOF.cpp
        include/OpenGL/ShaderOpenGLDOF.h
        src/OpenGL/ShaderOpenGLDepthMap.cpp
        include/OpenGL/ShaderOpenGLDepthMap.h
        src/OpenGL/ShaderOpenGLFOG.cpp
        include/OpenGL/ShaderOpenGLFOG.h
        src/OpenGL/ShaderQuadOpenGL.cpp
        include/OpenGL/ShaderQuadOpenGL.h
        src/OpenGL/ShaderOpenGLShockWave.cpp
        include/OpenGL/ShaderOpenGLShockWave.h
        src/FXEffect/FXShockWave.cpp
        include/FXEffect/FXShockWave.h
        include/GUI/GUIWidgetObjects3D.h
        include/GUI/GUIWidgetObject3DProperties.h
        include/GUI/GUIWidgetProjectSettings.h
        include/GUI/GUIWidgetMenu.h
        include/GUI/GUIWidgetToolbar.h
        src/OpenGL/ShaderOpenGLTint.cpp
        src/OpenGL/ShaderOpenGLTint.h
)

target_link_libraries(Brakeza3D
    pthread
    SDL2
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    BulletSoftBody
    BulletDynamics
    BulletCollision
    LinearMath
    assimp
    lua5.2
    ${OPENGL_LIBRARIES}
)

