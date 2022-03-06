cmake_minimum_required(VERSION 3.10)
project(Brakeza3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O3")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(SDL2_INCLUDE_DIRS /usr/include/SDL2)
set(SDL2_IMAGE_INCLUDE_DIRS /usr/include/SDL2)
set(SDL_MIXER_INCLUDE_DIRS /usr/include/SDL2)
set(SDL_TTF_INCLUDE_DIRS /usr/include/SDL2)
set(BULLET_INCLUDE_DIRS /usr/include/bullet)
set(ASSIMP_INCLUDE_DIRS /usr/include/assimp)
set(GLM_INCLUDE_DIRS /usr/include/glm)

if (WIN32)
    set(SDL2_INCLUDE_DIRS C:\\Users\\eduar\\Downloads\\SDL2-2.0.16\\x86_64-w64-mingw32\\include\\)
    set(SDL2_IMAGE_INCLUDE_DIRS C:\\Users\\eduar\\Desktop\\SDL2_image-2.0.5\\x86_64-w64-mingw32\\include)
    set(SDL_MIXER_INCLUDE_DIRS C:\\Users\\eduar\\Desktop\\SDL2_mixer-2.0.4\\x86_64-w64-mingw32\\include)
    set(SDL_TTF_INCLUDE_DIRS C:\\Users\\eduar\\Desktop\\SDL2_ttf-2.0.15\\x86_64-w64-mingw32\\include)
    set(BULLET_INCLUDE_DIRS C:\\Users\\eduar\\Desktop\\bullet)
    set(ASSIMP_INCLUDE_DIRS C:\\Users\\eduar\\Desktop\\assimp-5.0.1\\cmake-build-default-mingw\\include)
    set(GLM_INCLUDE_DIRS C:\\Users\\eduar\\Desktop\\glm)
endif (WIN32)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL_MIXER_INCLUDE_DIRS}
        ${SDL_TTF_INCLUDE_DIRS}
        ${BULLET_INCLUDE_DIRS}
        ${ASSIMP_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
)

add_executable(Brakeza3D
        #WIN32
        main.cpp
        src/Components/ComponentCamera.cpp
        include/Components/ComponentCamera.h
        src/Objects/Mesh3D.cpp
        include/Objects/Mesh3D.h
        src/Objects/Triangle3D.cpp
        include/Objects/Triangle3D.h
        src/Objects/Vertex3D.cpp
        include/Objects/Vertex3D.h
        src/Objects/Vector3D.cpp
        include/Objects/Vector3D.h
        src/Objects/Point2D.cpp
        include/Objects/Point2D.h
        src/Misc/Tools.cpp
        include/Misc/Tools.h
        src/Objects/Line2D.cpp
        include/Objects/Line2D.h
        include/Misc/Color.h
        src/Misc/Color.cpp
        include/EngineSetup.h
        src/EngineSetup.cpp
        src/Render/Texture.cpp
        include/Render/Texture.h
        include/EngineBuffers.h
        src/EngineBuffers.cpp
        src/Render/Frustum.cpp
        include/Render/Frustum.h
        src/Render/Plane.cpp
        include/Render/Plane.h
        src/Objects/Object3D.cpp
        include/Objects/Object3D.h
        src/Misc/Timer.cpp
        include/Misc/Timer.h
        include/Render/Transforms.h
        src/Render/Transforms.cpp
        src/Render/Drawable.cpp
        include/Render/Drawable.h
        src/Objects/LightPoint3D.cpp
        include/Objects/LightPoint3D.h
        src/Misc/TextureAtlas.cpp
        include/Misc/TextureAtlas.h
        src/Render/Billboard.cpp
        include/Render/Billboard.h
        src/Render/M3.cpp
        include/Render/M3.h
        src/Render/Logging.cpp
        include/Render/Tile.h
        include/Render/Logging.h
        src/Objects/SpriteDirectional3D.cpp
        include/Objects/SpriteDirectional3D.h
        src/2D/TextureAnimationDirectional.cpp
        include/2D/TextureAnimationDirectional.h
        src/Objects/Sprite3D.cpp
        include/Objects/Sprite3D.h
        src/2D/TextureAnimation.cpp
        include/2D/TextureAnimation.h
        src/Render/Maths.cpp
        include/Render/Maths.h
        src/Render/BSPMap.cpp
        include/Render/BSPMap.h
        src/Collisions/Mesh3DBody.cpp
        include/Physics/Mesh3DBody.h
        src/Render/PhysicsDebugDraw.cpp
        include/Render/PhysicsDebugDraw.h
        src/Collisions/Mesh3DGhost.cpp
        include/Physics/Mesh3DGhost.h
        include/Misc/cJSON.h
        src/Misc/cJSON.c
        src/Collisions/SpriteDirectional3DBody.cpp
        include/Physics/SpriteDirectional3DBody.h
        src/Components/ComponentWeapons.cpp
        include/Components/ComponentWeapons.h
        src/2D/WeaponAnimation.cpp
        include/2D/WeaponAnimation.h
        src/Game/WeaponType.cpp
        include/Game/WeaponType.h
        src/Components/ComponentMenu.cpp
        include/Components/ComponentMenu.h
        src/2D/MenuOption.cpp
        include/2D/MenuOption.h
        src/Game/Enemy.cpp
        include/Game/Enemy.h
        src/Brakeza3D.cpp
        include/Brakeza3D.h
        src/Components/ComponentCollisions.cpp
        include/Components/ComponentCollisions.h
        src/Game/NPCEnemyBody.cpp
        include/Game/NPCEnemyBody.h
        src/Collisions/CollisionResolver.cpp
        include/Physics/CollisionResolver.h
        src/Collisions/Projectile3DBody.cpp
        include/Physics/Projectile3DBody.h
        src/Objects/Decal.cpp
        include/Objects/Decal.h
        src/Collisions/Sprite3DBody.cpp
        include/Physics/Sprite3DBody.h
        src/Collisions/Body.cpp
        include/Physics/Body.h
        include/Objects/Cube3D.cpp
        include/Objects/Cube3D.h
        include/Game/Player.cpp src/Game/Player.h
        src/Demo/Demo.cpp include/Demo/Demo.h
        src/Misc/Counter.cpp
        include/Misc/Counter.h
        src/Misc/TexturePackage.cpp
        include/Misc/TexturePackage.h
        src/Misc/SoundPackage.cpp
        include/Misc/SoundPackage.h
        include/Misc/Parallells.h
        include/Misc/VariablePackage.cpp
        include/Misc/VariablePackage.h
        src/Collisions/BillboardBody.cpp
        include/Physics/BillboardBody.h
        src/Game/ItemWeaponGhost.cpp
        include/Game/ItemWeaponGhost.h
        src/Game/ItemAmmoGhost.cpp
        include/Game/ItemAmmoGhost.h
        src/Game/ItemHealthGhost.cpp
        include/Game/ItemHealthGhost.h
        src/Game/AmmoType.cpp
        include/Game/AmmoType.h
        src/Components/Component.cpp
        include/Components/Component.h
        src/Components/ComponentRender.cpp
        include/Components/ComponentRender.h
        src/Components/ComponentWindow.cpp
        include/Components/ComponentWindow.h
        src/Components/ComponentInput.cpp
        include/Components/ComponentInput.h
        src/Components/ComponentSound.cpp
        include/Components/ComponentSound.h
        src/Objects/Camera3D.cpp
        include/Components/Camera3D.h
        src/Components/ComponentHUD.cpp
        include/Components/ComponentHUD.h
        src/ComponentsManager.cpp
        include/ComponentsManager.h
        src/Components/ComponentGUI.cpp
        include/Components/ComponentGUI.h
        src/Components/ComponentGame.cpp
        include/Components/ComponentGame.h
        src/Components/ComponentGameInput.cpp
        include/Components/ComponentGameInput.h
        src/Objects/Mesh3DAnimated.cpp
        include/Objects/Mesh3DAnimated.h
        src/Objects/Mesh3DAnimatedCollection.cpp
        include/Objects/Mesh3DAnimatedCollection.h
        src/Collisions/BSPCollider.cpp
        src/Collisions/BSPCollider.h
        src/2D/TextWriter.cpp
        include/2D/TextWriter.h
        include/Game/DoorGhost.cpp
        include/Game/DoorGhost.h
        src/Collisions/Ghost.cpp
        include/Physics/Ghost.h
        include/Render/AABB3D.h
        src/Misc/Octree.cpp
        include/Misc/Octree.h
        include/Misc/Grid3D.cpp
        include/Misc/Grid3D.h
        src/Misc/PathFinder.cpp
        include/Misc/PathFinder.h

        console/Command.cpp
        console/Command.h
        console/CommandArgument.cpp
        console/CommandArgument.h
        console/Prompt.cpp
        console/Prompt.h
        console/ConsoleManager.cpp
        console/ConsoleManager.h
        include/Components/ComponentConsole.cpp include/Components/ComponentConsole.h

        include/GUI/GUI.h
        include/GUI/GUI_Camera.h
        include/GUI/GUI_Menu.h
        include/GUI/GUI_Objects3D.h
        include/GUI/GUI_Tiles.h
        include/GUI/GUI_Weapons.h
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

        imgui/examples/imgui_impl_sdl.h
        imgui/examples/imgui_impl_sdl.cpp
        imgui/examples/imgui_impl_opengl2.h
        imgui/examples/imgui_impl_opengl2.cpp
        include/Render/Fragment.cpp
        include/Render/Fragment.h
        src/Render/M4.cpp
        include/Render/M4.h
        src/Objects/Vertex4D.cpp
        include/Objects/Vertex4D.h
        src/Particles/ParticleEmissor.cpp
        include/Particles/ParticleEmissor.h
        src/Particles/Particle.cpp
        include/Particles/Particle.h
        src/Particles/ParticleEmissorGravity.cpp
        include/Particles/ParticleEmissorGravity.h
        src/Collisions/SimplePendulum.cpp
        include/Physics/SimplePendulum.h
        src/Render/Shader.cpp
        include/Render/Shader.h
        include/Shaders/ShaderWater.h
        include/Shaders/ShaderFire.h
        include/Shaders/ShaderObjectSilhouette.h
        include/Shaders/ShaderImageBackground.h
        src/Misc/Image.cpp include/Misc/Image.h include/Shaders/ShaderTintScreen.h src/Collisions/Collisionable.cpp src/Collisions/Collisionable.h src/2D/Button.cpp include/2D/Button.h include/2D/ButtonsCallbacks.h include/GUI/GUI_Shaders.h include/Shaders/ShaderSmoke.h include/Shaders/ShaderBlink.h)

if (WIN32)
    target_link_libraries(Brakeza3D
            mingw32
            SDL2main
    )
endif (WIN32)

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
        GL
        )

