/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 03/01/2026.
//

#ifndef BRAKEZA3D_OPENGLSHADERTYPES_H
#define BRAKEZA3D_OPENGLSHADERTYPES_H
#include <map>
#include <string>
#include <mutex>

#include "../Base/SharedOpenGLStructs.h"

// GLSLTypeMapping es un set fijo de claves precargado en estatico (ver .cpp), pero se consulta
// concurrentemente desde varios workers del ThreadPool durante la carga de Mesh3D/Mesh3DAnimation
// (ApplyShadersBackground) a la vez que el hilo principal itera el mapa en ShadersGUI. operator[]
// inserta si la clave no existe -- una clave desconocida (typo o tipo nuevo sin registrar) llegada
// desde dos hilos a la vez corrompe el arbol. GLSLTypeMappingMutex + GetGLSLTypeInfo() serializan
// todo acceso; usar SIEMPRE estas dos en vez de tocar GLSLTypeMapping directamente.
extern std::map<std::string, ShaderTypeInfo> GLSLTypeMapping;
extern std::mutex GLSLTypeMappingMutex;

ShaderTypeInfo GetGLSLTypeInfo(const std::string& key);

#endif //BRAKEZA3D_OPENGLSHADERTYPES_H