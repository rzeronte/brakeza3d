//
// Created by Eduardo on 10/07/2026.
//

#include "../../include/3D/Sound3D.h"
#include <algorithm>
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/Sound3DGUI.h"
#include "../../include/Misc/Logging.h"

float Sound3D::ambienceVolumeScale = 1.0f;

Sound3D::Sound3D()
{
    renderSettings.frustumCulling = false;
}

Sound3D::~Sound3D()
{
    if (channel >= 0 && Mix_GetChunk(channel) == mixChunk) {
        Mix_HaltChannel(channel);
    }
    if (mixChunk) {
        Mix_FreeChunk(mixChunk);
        mixChunk = nullptr;
    }
}

void Sound3D::onUpdate()
{
    if (!mixChunk) return;

    auto* cam = Components::get()->Camera()->getCamera();
    Vertex3D camPos = cam->getPosition();
    float    dist   = position.distance(camPos);

    int vol = 0;
    if (dist <= innerRadius) {
        vol = baseVolume;
    } else if (dist < outerRadius) {
        float t = (dist - innerRadius) / (outerRadius - innerRadius);
        vol = static_cast<int>(baseVolume * (1.0f - t));
    }
    vol = static_cast<int>(vol * ambienceVolumeScale);

    bool channelOwned = (channel >= 0 && Mix_GetChunk(channel) == mixChunk);

    if (vol > 0) {
        if (!channelOwned) {
            channel   = Mix_PlayChannel(-1, mixChunk, loop ? -1 : 0);
            isPlaying = (channel >= 0);
        } else {
            Mix_Volume(channel, vol);
        }
    } else {
        if (channelOwned) {
            Mix_HaltChannel(channel);
            channel   = -1;
            isPlaying = false;
        }
    }

    if (debugDraw) {
        auto* render = Components::get()->Render();
        // inner radius: green — full-volume zone boundary
        render->drawGroundCircle(this, 0.2f, 1.0f, 0.2f, 0.9f, innerRadius, 0.15f);
        float falloffThickness = std::max(0.1f, (outerRadius - innerRadius) * 0.005f);
        render->drawGroundCircle(this, 1.0f, 0.55f, 0.1f, 0.55f, outerRadius, falloffThickness);
    }
}

void Sound3D::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Sound3DGUI::DrawPropertiesGUI(this);
}

GUIType::Sheet Sound3D::getIcon()
{
    return IconObject::SOUND_3D;
}

ObjectType Sound3D::getTypeObject() const
{
    return ObjectType::Sound3D;
}
