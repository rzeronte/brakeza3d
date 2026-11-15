/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
// Translation unit unica para la implementacion de miniaudio (single-header lib).
// El resto del proyecto solo hace #include "miniaudio.h" (modo declaracion), este
// .cpp es el UNICO sitio donde se define MINIAUDIO_IMPLEMENTATION -- si se define en
// mas de una TU, el linker falla por simbolos duplicados.
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
