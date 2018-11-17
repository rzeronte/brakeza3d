cmake_minimum_required(VERSION 3.10)
project(brakeda3d)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${brakeda3d_SOURCE_DIR}/cmake")

set(SDL2_dir /Library/Frameworks/SDL2.framework/Headers)
set(SDL2_image_dir /Library/Frameworks/SDL2_image.framework/Headers)
set(SDL2_ttf_dir /Library/Frameworks/SDL2_ttf.framework/Headers)
set(OpenGL_dir /Library/Frameworks/OpenGL.framework/Headers/)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SDL2_dir} ${SDL2_image_dir} ${SDL2_ttf_dir} ${OPENGL_INCLUDE_DIRS})

add_executable(brakeda3d
        main.cpp
        src/Objects/Camera3D.cpp
        headers/Objects/Camera3D.h
        src/Render/Controller.cpp
        headers/Render/Controller.h
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
        src/Render/Tools.cpp
        headers/Render/Tools.h
        src/Objects/Line2D.cpp
        headers/Objects/Line2D.h
        src/Render/Color.cpp
        headers/Render/Color.h
        src/Render/Engine.cpp
        headers/Render/Engine.h
        headers/Render/EngineSetup.h
        src/Render/EngineSetup.cpp
        src/Render/Texture.cpp
        headers/Render/Texture.h
        src/Q3MapLoader/Q3Loader.cpp
        headers/Q3MapLoader/Q3Loader.h
        headers/Render/EngineBuffers.h
        src/Render/EngineBuffers.cpp
        src/Render/Frustum.cpp
        headers/Render/Frustum.h
        src/Render/Plane.cpp
        headers/Render/Plane.h
        src/Objects/Object3D.cpp
        headers/Objects/Object3D.h
        src/Render/Timer.cpp
        headers/Render/Timer.h
        headers/Render/Transforms.h
        src/Render/Transforms.cpp
        src/Render/Drawable.cpp
        headers/Render/Drawable.h
        src/Objects/LightPoint3D.cpp
        headers/Objects/LightPoint3D.h
        Game.cpp Game.h
        src/Render/Billboard.cpp
        headers/Render/Billboard.h
        src/Render/M3.cpp
        headers/Render/M3.h

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

        src/GUI/GUI.h
        src/GUI/GUI_Menu.h
        src/GUI/GUI_ObjectsInspector.h
        src/GUI/GUI_Engine.h
        src/GUI/GUI_LightsInspector.h
        src/GUI/GUI_Log.h
        src/Render/Logging.cpp

        headers/Render/Logging.h
        src/Objects/SpriteDirectional3D.cpp
        headers/Objects/SpriteDirectional3D.h
        src/2D/AnimationDirectional2D.cpp
        headers/2D/AnimationDirectional2D.h
        src/Objects/Sprite3D.cpp
        headers/Objects/Sprite3D.h
        src/2D/Animation2D.cpp
        headers/2D/Animation2D.h
        src/Render/Maths.cpp
        headers/Render/Maths.h
        src/Objects/Weapon3D.cpp
        headers/Objects/Weapon3D.h

        WAD/Constants.cpp
        WAD/Constants.h
        WAD/Directory.cpp
        WAD/Directory.h
        WAD/Identification.cpp
        WAD/Identification.h
        WAD/Signature.cpp
        WAD/Signature.h
        WAD/WAD.cpp
        WAD/WAD.h
        WAD/WADSector.cpp WAD/WADSector.h WAD/WADLinedef.cpp WAD/WADLinedef.h WAD/WADSidedef.cpp WAD/WADSidedef.h WAD/WADVertex.cpp WAD/WADVertex.h Map.cpp Map.h src/GUI/GUI_Camera.h )

target_link_libraries(brakeda3d ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${SDLTTF_LIBRARY} ${OPENGL_LIBRARIES})