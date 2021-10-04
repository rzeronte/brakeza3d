cmake_minimum_required(VERSION 3.10)
project(Brakeza3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O3")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Bullet REQUIRED)
find_package(Assimp REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL_MIXER_INCLUDE_DIRS}
        ${SDL_TTF_INCLUDE_DIRS}
        ${BULLET_INCLUDE_DIRS}
        ${assimp_INCLUDE_DIRS}
        /usr/include/bullet
)

add_executable(Brakeza3D
        #        WIN32
        main.cpp
        src/Components/ComponentCamera.cpp
        headers/Components/ComponentCamera.h
        src/Objects/Mesh3D.cpp
        headers/Objects/Mesh3D.h
        src/Objects/Triangle3D.cpp
        headers/Objects/Triangle3D.h
        src/Objects/Vertex3D.cpp
        headers/Objects/Vertex3D.h
        src/Objects/Vector3D.cpp
        headers/Objects/Vector3D.h
        src/Objects/Point2D.cpp
        headers/Objects/Point2D.h
        src/Misc/Tools.cpp
        headers/Misc/Tools.h
        src/Objects/Line2D.cpp
        headers/Objects/Line2D.h
        headers/Misc/Color.h
        headers/OpenCL/OCLTriangle.h
        headers/EngineSetup.h
        src/EngineSetup.cpp
        src/Render/Texture.cpp
        headers/Render/Texture.h
        headers/EngineBuffers.h
        src/EngineBuffers.cpp
        src/Render/Frustum.cpp
        headers/Render/Frustum.h
        src/Render/Plane.cpp
        headers/Render/Plane.h
        src/Objects/Object3D.cpp
        headers/Objects/Object3D.h
        src/Misc/Timer.cpp
        headers/Misc/Timer.h
        headers/Render/Transforms.h
        src/Render/Transforms.cpp
        src/Render/Drawable.cpp
        headers/Render/Drawable.h
        src/Objects/LightPoint3D.cpp
        headers/Objects/LightPoint3D.h

        src/Misc/TextureAtlas.cpp
        headers/Misc/TextureAtlas.h

        src/Render/Billboard.cpp
        headers/Render/Billboard.h
        src/Render/M3.cpp
        headers/Render/M3.h

        src/Render/Logging.cpp

        headers/Render/Tile.h

        headers/Render/Logging.h
        src/Objects/SpriteDirectional3D.cpp
        headers/Objects/SpriteDirectional3D.h
        src/2D/TextureAnimationDirectional.cpp
        headers/2D/TextureAnimationDirectional.h
        src/Objects/Sprite3D.cpp
        headers/Objects/Sprite3D.h
        src/2D/TextureAnimation.cpp
        headers/2D/TextureAnimation.h
        src/Render/Maths.cpp
        headers/Render/Maths.h

        src/Render/BSPMap.cpp
        headers/Render/BSPMap.h
        src/Physics/Mesh3DBody.cpp
        headers/Physics/Mesh3DBody.h
        src/Render/PhysicsDebugDraw.cpp
        headers/Render/PhysicsDebugDraw.h
        src/Physics/Mesh3DGhost.cpp
        headers/Physics/Mesh3DGhost.h
        headers/Misc/cJSON.h
        src/Misc/cJSON.c
        src/Physics/SpriteDirectional3DBody.cpp
        headers/Physics/SpriteDirectional3DBody.h
        src/Components/ComponentWeapons.cpp
        headers/Components/ComponentWeapons.h
        src/2D/WeaponAnimation.cpp
        headers/2D/WeaponAnimation.h
        src/Game/WeaponType.cpp
        headers/Game/WeaponType.h
        src/Components/ComponentMenu.cpp
        headers/Components/ComponentMenu.h
        src/2D/MenuOption.cpp
        headers/2D/MenuOption.h
        src/Game/Enemy.cpp
        headers/Game/Enemy.h
        src/Brakeza3D.cpp
        headers/Brakeza3D.h
        src/Components/ComponentCollisions.cpp
        headers/Components/ComponentCollisions.h
        src/Game/NPCEnemyBody.cpp
        headers/Game/NPCEnemyBody.h
        src/Collisions/CollisionResolver.cpp
        headers/Collisions/CollisionResolver.h
        src/Game/Projectile3DBody.cpp
        headers/Game/Projectile3DBody.h
        headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h
        headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h
        headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h
        headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h
        headers/Collisions/CollisionResolverBetweenEnemyPartAndBSPMap.h
        headers/Collisions/CollisionResolverBetweenProjectileAndPlayer.h
        src/Objects/Decal.cpp
        headers/Objects/Decal.h
        src/Game/NPCEnemyPartBody.cpp
        headers/Game/NPCEnemyPartBody.h
        src/Physics/Sprite3DBody.cpp
        headers/Physics/Sprite3DBody.h
        src/Physics/Body.cpp
        headers/Physics/Body.h
        headers/Objects/Cube3D.cpp
        headers/Objects/Cube3D.h

        headers/Game/Player.cpp src/Game/Player.h
        src/Demo/Demo.cpp headers/Demo/Demo.h
        src/Misc/Counter.cpp
        headers/Misc/Counter.h
        src/Misc/TexturePackage.cpp
        headers/Misc/TexturePackage.h
        src/Misc/SoundPackage.cpp
        headers/Misc/SoundPackage.h
        headers/Misc/Parallells.h
        headers/Misc/VariablePackage.cpp
        headers/Misc/VariablePackage.h
        src/Physics/BillboardBody.cpp
        headers/Physics/BillboardBody.h
        src/Game/ItemWeaponGhost.cpp
        headers/Game/ItemWeaponGhost.h
        headers/Collisions/CollisionResolverBetweenCamera3DAndItemWeapon.h
        src/Game/ItemAmmoGhost.cpp
        headers/Game/ItemAmmoGhost.h
        src/Game/ItemHealthGhost.cpp
        headers/Game/ItemHealthGhost.h
        headers/Collisions/CollisionResolverBetweenCamera3DAndItemHealth.h
        headers/Collisions/CollisionResolverBetweenCamera3DAndItemAmmo.h
        src/Game/AmmoType.cpp
        headers/Game/AmmoType.h
        src/Components/Component.cpp
        headers/Components/Component.h
        src/Components/ComponentRender.cpp
        headers/Components/ComponentRender.h
        src/Components/ComponentWindow.cpp
        headers/Components/ComponentWindow.h
        src/Components/ComponentInput.cpp
        headers/Components/ComponentInput.h
        src/Components/ComponentSound.cpp
        headers/Components/ComponentSound.h
        src/Components/ComponentBSP.cpp
        headers/Components/ComponentBSP.h
        src/Objects/Camera3D.cpp
        headers/Components/Camera3D.h
        src/Components/ComponentHUD.cpp
        headers/Components/ComponentHUD.h
        src/ComponentsManager.cpp
        headers/ComponentsManager.h
        src/Components/ComponentGUI.cpp
        headers/Components/ComponentGUI.h
        src/Components/ComponentGame.cpp
        headers/Components/ComponentGame.h
        src/Components/ComponentGameInput.cpp
        headers/Components/ComponentGameInput.h
        src/Objects/Mesh3DAnimated.cpp
        headers/Objects/Mesh3DAnimated.h
        src/Objects/Mesh3DAnimatedCollection.cpp
        headers/Objects/Mesh3DAnimatedCollection.h
        src/Physics/BSPCollider.cpp
        src/Physics/BSPCollider.h
        src/2D/TextWriter.cpp
        headers/2D/TextWriter.h
        headers/Game/DoorGhost.cpp
        headers/Game/DoorGhost.h
        headers/Physics/Ghost.cpp
        headers/Physics/Ghost.h
        headers/Render/AABB3D.h
        src/Misc/Octree.cpp
        headers/Misc/Octree.h
        headers/Misc/Grid3D.cpp
        headers/Misc/Grid3D.h
        src/Misc/PathFinder.cpp
        headers/Misc/PathFinder.h
        )

if (WIN32)
    target_link_libraries(Brakeza3D
            mingw32
            SDL2main
    )
endif (WIN32)

target_link_libraries(Brakeza3D
        pthread
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${BULLET_LIBRARIES}
        ${assimp_LIBRARIES}
        )