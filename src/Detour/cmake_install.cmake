# Install script for directory: /Users/darkhead/CLionProjects/recastnavigation/Detour

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibraryx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/darkhead/CLionProjects/recastnavigation/Detour/libDetour.VERSION.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.VERSION.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.VERSION.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "libDetour.VERSION.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.VERSION.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.VERSION.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibraryx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/darkhead/CLionProjects/recastnavigation/Detour/libDetour.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "libDetour.VERSION.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libDetour.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourAlloc.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourAssert.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourCommon.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourMath.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourNavMesh.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourNavMeshBuilder.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourNavMeshQuery.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourNode.h"
    "/Users/darkhead/CLionProjects/recastnavigation/Detour/Include/DetourStatus.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/darkhead/CLionProjects/recastnavigation/Detour/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
