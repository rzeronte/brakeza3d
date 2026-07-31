//
// Created by Eduardo on 10/07/2026.
//

#ifndef BRAKEZA3D_SOUND3D_H
#define BRAKEZA3D_SOUND3D_H

#include "Object3D.h"
#include <SDL_mixer.h>
#include <string>

class Sound3D : public Object3D
{
public:
    static float ambienceVolumeScale;

    std::string sourceFile;
    Mix_Chunk*  mixChunk    = nullptr;
    int         channel     = -1;
    float       innerRadius = 15.0f;
    float       outerRadius = 60.0f;
    int         baseVolume  = 128;
    bool        loop        = true;
    bool        isPlaying   = false;
    bool        debugDraw   = false;

    Sound3D();
    ~Sound3D() override;

    void onUpdate()           override;
    void DrawPropertiesGUI()  override;
    GUIType::Sheet getIcon()  override;
    ObjectType getTypeObject() const override;

    void setEnabled(bool enabled) override
    {
        if (!enabled) {
            if (channel >= 0 && Mix_GetChunk(channel) == mixChunk)
                Mix_HaltChannel(channel);
            channel   = -1;
            isPlaying = false;
        }
        Object3D::setEnabled(enabled);
    }
};

#endif //BRAKEZA3D_SOUND3D_H
