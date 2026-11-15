/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_TEXTUREANIMATEDCACHE_H
#define BRAKEZA3D_TEXTUREANIMATEDCACHE_H

#include "ResourceCacheBase.h"
#include "../Render/TextureAnimated.h"

// Cachea sprite sheets ya recortados en frames (TextureAnimated) por
// (fichero, ancho de frame, alto de frame, numFrames, fps). El objeto
// devuelto es una plantilla compartida: los frames (Image*/SDL_Texture) no
// deben mutarse ni liberarse por el llamador. Para reproducir la animación
// de forma independiente por instancia, copiar con `new TextureAnimated(template)`
// (constructor de copia: comparte frames, resetea currentFrame/counter propios).
class TextureAnimatedCache : public ResourceCacheBase {
public:
    TextureAnimated* getOrLoadTemplate(const std::string &spriteSheetFile, int frameWidth, int frameHeight, int numFrames, int fps);
};

extern TextureAnimatedCache textureAnimatedCache;

#endif //BRAKEZA3D_TEXTUREANIMATEDCACHE_H
