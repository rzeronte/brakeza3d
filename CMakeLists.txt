cmake_minimum_required(VERSION 3.10)
project(Brakeza3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS_DEBUG "-O3 -fsanitize=leak")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -BRAKEZA_DEBUG")

set(CMAKE_VERBOSE_MAKEFILE ON)

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
        include/EngineBuffers.h src/EngineBuffers.cpp
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
        include/GUI/GUI_Objects3D.h
    include/GUI/GUI_Tiles.h
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
    src/Particles/ParticleEmitter.cpp include/Particles/ParticleEmitter.h
    src/Collisions/SimplePendulum.cpp include/Physics/SimplePendulum.h
    src/Render/Shader.cpp include/Render/Shader.h
    src/Misc/Image.cpp include/Misc/Image.h
    src/Collisions/Collisionable.cpp src/Collisions/Collisionable.h
    src/2D/Button.cpp include/2D/Button.h
    include/2D/ButtonsCallbacks.h include/Shaders/ShaderBlink.h

    src/Behaviors/Object3DBehavior.cpp include/Behaviors/Object3DBehavior.h
    src/Particles/ParticleEmitterFireworks.cpp include/Particles/ParticleEmitterFireworks.h

    include/Objects/Vector2D.cpp include/Objects/Vector2D.h
        src/Render/ShaderOpenCL.cpp include/Render/ShaderOpenCL.h
    src/Shaders/ShaderImage.cpp include/Shaders/ShaderImage.h
    src/Shaders/ShaderEdgeObject.cpp include/Shaders/ShaderEdgeObject.h
    src/Shaders/ShaderColor.cpp include/Shaders/ShaderColor.h
    src/Collisions/Mesh3DAnimatedGhost.cpp include/Physics/Mesh3DAnimatedGhost.h
    src/Collisions/RayCollisionable.cpp include/Physics/RayCollisionable.h
    src/Shaders/ShockWave.cpp include/Shaders/ShockWave.h

        src/Shaders/ShaderBilinear.cpp include/Shaders/ShaderBilinear.h
    src/Objects/Swarm.cpp include/Objects/Swarm.h
    src/Shaders/ShaderExplosion.cpp include/Shaders/ShaderExplosion.h
    src/Shaders/ShaderDepthOfField.cpp include/Shaders/ShaderDepthOfField.h
    src/Shaders/ShaderImageMask.cpp include/Shaders/ShaderImageMask.h
    src/Behaviors/RotatableToTarget.cpp include/Behaviors/RotatableToTarget.h

    src/Shaders/ShaderBlink.cpp
        include/Objects/OpenCLTypes.h
    include/Physics/Projectile.h

        src/2D/Sprite2D.cpp include/2D/Sprite2D.h

        kernels/renderer.cl

        src/2D/Sprite2D.cpp
        include/2D/Sprite2D.h
        include/Shaders/ShaderParticles.h
        src/Shaders/ShaderParticles.cpp
        src/Shaders/ShaderBlurBuffer.cpp
        src/Shaders/ShaderParticlesBlurBuffer.h
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
        include/Render/ObjectShaderOpenCL.h
        include/Misc/TileManager.cpp
        include/Misc/TileManager.h
        include/OpenGL/ShaderOpenGL.cpp
        include/OpenGL/ShaderOpenGL.h
        include/OpenGL/ShaderOpenGLImage.cpp
        include/OpenGL/ShaderOpenGLImage.h
        include/OpenGL/ShaderOpenGLRender.cpp
        include/OpenGL/ShaderOpenGLRender.h
        include/Objects/OpenGLShaderTypes.h
        include/OpenGL/ShaderOpenGLLine.cpp
        include/OpenGL/ShaderOpenGLLine.h
        src/Objects/SpotLight3D.cpp
        src/Objects/SpotLight3D.h
        include/OpenGL/ShaderOpenGLWireframe.cpp
        include/OpenGL/ShaderOpenGLWireframe.h
        include/OpenGL/ShaderOpenGLShading.cpp
        include/OpenGL/ShaderOpenGLShading.h
        include/OpenGL/ShaderOpenGLPoints.cpp
        include/OpenGL/ShaderOpenGLPoints.h
        include/OpenGL/ShaderOpenGLCustom.cpp
        include/OpenGL/ShaderOpenGLCustom.h
        include/OpenGL/ShaderCustomOpenGLParticles.cpp
        include/OpenGL/ShaderCustomOpenGLParticles.h
        include/OpenGL/ShaderOpenGLCustom.cpp
        include/OpenGL/ShaderOpenGLOutliner.cpp
        include/OpenGL/ShaderOpenGLOutliner.h
        include/OpenGL/ShaderOpenGLColor.cpp
        include/OpenGL/ShaderOpenGLColor.h

        ProjectDarkHeaZ/Components/ComponentGameInput.h ProjectDarkHeaZ/Components/ComponentGameInput.cpp
        ProjectDarkHeaZ/Components/ComponentMenu.cpp ProjectDarkHeaZ/Components/ComponentMenu.h
        ProjectDarkHeaZ/Components/ComponentGame.cpp ProjectDarkHeaZ/Components/ComponentGame.h
        ProjectDarkHeaZ/Components/ComponentHUD.h ProjectDarkHeaZ/Components/ComponentHUD.cpp

        ProjectDarkHeaZ/Behaviors/EnemyBehaviorPatrol.cpp
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorPatrol.h
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorCircle.cpp
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorCircle.h
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorFollow.cpp
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorFollow.h
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorPath.cpp
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorPath.h
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorRandom.cpp
        ProjectDarkHeaZ/Behaviors/EnemyBehaviorRandom.h

        ProjectDarkHeaZ/Enemies/AsteroidEnemyGhost.h ProjectDarkHeaZ/Enemies/AsteroidEnemyGhost.cpp
        ProjectDarkHeaZ/Enemies/Enemy.h ProjectDarkHeaZ/Enemies/Enemy.cpp
        ProjectDarkHeaZ/Enemies/EnemyGhost.h ProjectDarkHeaZ/Enemies/EnemyGhost.cpp
        ProjectDarkHeaZ/Enemies/EnemyGhostEmitter.h ProjectDarkHeaZ/Enemies/EnemyGhostEmitter.cpp

        ProjectDarkHeaZ/Items/ItemBombGhost.h ProjectDarkHeaZ/Items/ItemBombGhost.cpp
        ProjectDarkHeaZ/Items/ItemHumanGhost.h ProjectDarkHeaZ/Items/ItemHumanGhost.cpp
        ProjectDarkHeaZ/Items/ItemEnergyGhost.h ProjectDarkHeaZ/Items/ItemEnergyGhost.cpp
        ProjectDarkHeaZ/Items/ItemHealthGhost.h ProjectDarkHeaZ/Items/ItemHealthGhost.cpp
        ProjectDarkHeaZ/Items/ItemShieldGhost.h ProjectDarkHeaZ/Items/ItemShieldGhost.cpp
        ProjectDarkHeaZ/Items/ItemWeaponGhost.h ProjectDarkHeaZ/Items/ItemWeaponGhost.cpp
        ProjectDarkHeaZ/Items/ItemWithTutorial.h ProjectDarkHeaZ/Items/ItemWithTutorial.cpp
        ProjectDarkHeaZ/Items/StoreItem.h ProjectDarkHeaZ/Items/StoreItem.cpp
        ProjectDarkHeaZ/Items/LivingObject.h ProjectDarkHeaZ/Items/LivingObject.cpp
        ProjectDarkHeaZ/Items/EnemyDialog.h ProjectDarkHeaZ/Items/EnemyDialog.cpp
        ProjectDarkHeaZ/Items/PlayerSatellite.h ProjectDarkHeaZ/Items/PlayerSatellite.cpp
        ProjectDarkHeaZ/Items/PlayerReflection.h ProjectDarkHeaZ/Items/PlayerReflection.cpp

        ProjectDarkHeaZ/Player.h ProjectDarkHeaZ/Player.cpp
        ProjectDarkHeaZ/LevelLoader.h ProjectDarkHeaZ/LevelLoader.cpp
        ProjectDarkHeaZ/StoreManager.h ProjectDarkHeaZ/StoreManager.cpp
        ProjectDarkHeaZ/ProjectileRay.h ProjectDarkHeaZ/ProjectileRay.cpp
        ProjectDarkHeaZ/FaderToGameStates.h

        ProjectDarkHeaZ/Weapons/AmmoProjectile.h
        ProjectDarkHeaZ/Weapons/AmmoProjectileBody.h ProjectDarkHeaZ/Weapons/AmmoProjectileBody.cpp
        ProjectDarkHeaZ/Weapons/AmmoProjectileBodyEmitter.h ProjectDarkHeaZ/Weapons/AmmoProjectileBodyEmitter.cpp
        ProjectDarkHeaZ/Weapons/Weapon.h ProjectDarkHeaZ/Weapons/Weapon.cpp

        ProjectDarkHeaZ/Shaders/RayLight.h ProjectDarkHeaZ/Shaders/RayLight.cpp
        ProjectDarkHeaZ/Shaders/ShaderShockWave.h ProjectDarkHeaZ/Shaders/ShaderShockWave.cpp
        ProjectDarkHeaZ/Shaders/ShaderCRT.h ProjectDarkHeaZ/Shaders/ShaderCRT.cpp
        ProjectDarkHeaZ/Shaders/ShaderEnergyShield.h ProjectDarkHeaZ/Shaders/ShaderEnergyShield.cpp
        ProjectDarkHeaZ/Shaders/ShaderProjectiles.h ProjectDarkHeaZ/Shaders/ShaderProjectiles.cpp
        ProjectDarkHeaZ/Shaders/ShaderZombie.h ProjectDarkHeaZ/Shaders/ShaderZombie.cpp

        ProjectDarkHeaZ/Bosses/BossEnemy.h ProjectDarkHeaZ/Bosses/BossEnemy.cpp
        ProjectDarkHeaZ/Bosses/BossLevel10.h ProjectDarkHeaZ/Bosses/BossLevel10.cpp
        ProjectDarkHeaZ/Bosses/BossLevel20.h ProjectDarkHeaZ/Bosses/BossLevel20.cpp
        ProjectDarkHeaZ/Bosses/BossLevel30.h ProjectDarkHeaZ/Bosses/BossLevel30.cpp
        ProjectDarkHeaZ/PaletteColors.h ProjectDarkHeaZ/Items/SalvageSpaceship.cpp
        ProjectDarkHeaZ/Items/SalvageSpaceship.h
        ProjectDarkHeaZ/Enemies/BoidEnemyGhost.cpp
        ProjectDarkHeaZ/Enemies/BoidEnemyGhost.h
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
    OpenCL
    lua5.2
    ${OPENGL_LIBRARIES}
)

