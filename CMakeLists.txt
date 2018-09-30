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
        src/Camera.cpp
        headers/Camera.h
        src/Controller.cpp
        headers/Controller.h
        src/Mesh.cpp
        headers/Mesh.h
        src/Triangle.cpp
        headers/Triangle.h
        src/Vertex.cpp
        headers/Vertex.h
        src/Vector3D.cpp
        headers/Vector3D.h
        src/Point2D.cpp
        headers/Point2D.h
        src/Tools.cpp
        headers/Tools.h
        src/Line2D.cpp
        headers/Line2D.h
        src/Color.cpp
        headers/Color.h
        src/Engine.cpp
        headers/Engine.h
        headers/EngineSetup.h
        src/EngineSetup.cpp
        src/Texture.cpp
        headers/Texture.h
        src/Q3Loader.cpp
        headers/Q3Loader.h
        headers/EngineBuffers.h
        src/EngineBuffers.cpp
        src/Frustum.cpp
        headers/Frustum.h
        src/Plane.cpp
        headers/Plane.h
        src/Object3D.cpp
        headers/Object3D.h
        src/LTimer.cpp
        headers/LTimer.h
        headers/Render.h
        src/Render.cpp
        src/Drawable.cpp
        headers/Drawable.h
        src/LightPoint.cpp
        headers/LightPoint.h
        Game.cpp Game.h
        src/Billboard.cpp
        headers/Billboard.h
        src/M3.cpp
        headers/M3.h

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

        headers/GUI/GUI.h
        headers/GUI/GUI_Menu.h
        headers/GUI/GUI_ObjectsInspector.h
        headers/GUI/GUI_Engine.h
        headers/GUI/GUI_LightsInspector.h
        headers/GUI/GUI_Log.h
        headers/Core/Logging.cpp
        headers/Core/Logging.h
        )

target_link_libraries(brakeda3d ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${SDLTTF_LIBRARY} ${OPENGL_LIBRARIES})