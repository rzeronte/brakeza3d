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
// Created by Eduardo on 10/07/2026.
//

#include "../../include/3D/Sound3D.h"
#include "../../include/Misc/AssetPaths.h"
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include "../../include/Components/Components.h"
#include "../../include/GUI/Inspectors/Sound3DGUI.h"
#include "../../include/Misc/Logging.h"

float Sound3D::ambienceVolumeScale = 1.0f;
bool  Sound3D::panningEnabled = false;   // apagado por defecto: el RTS y los proyectos existentes suenan como siempre

namespace {
struct PcmCacheEntry
{
    std::mutex                    loadMutex;   // serializa la decodificacion de ESTE fichero
    std::weak_ptr<Sound3D::Pcm>   pcm;         // no retiene: el PCM muere con su ultimo Sound3D
};
std::mutex                                                     g_pcmMapMutex;
std::unordered_map<std::string, std::shared_ptr<PcmCacheEntry>> g_pcmMap;
}

std::shared_ptr<Sound3D::Pcm> Sound3D::DecodeShared(const std::string& srcFile)
{
    const std::string path = AssetPaths::Resolve(srcFile);

    std::shared_ptr<PcmCacheEntry> entry;
    {
        std::lock_guard<std::mutex> lk(g_pcmMapMutex);
        auto& slot = g_pcmMap[path];
        if (!slot) slot = std::make_shared<PcmCacheEntry>();
        entry = slot;
    }

    std::lock_guard<std::mutex> lk(entry->loadMutex);
    if (auto cached = entry->pcm.lock()) return cached;

    ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, 2, 44100);
    ma_uint64 frameCount  = 0;
    void*     pData       = nullptr;

    ma_result result = ma_decode_file(path.c_str(), &cfg, &frameCount, &pData);
    if (result != MA_SUCCESS) {
        LOG_ERROR("[Sound3D] ma_decode_file failed for '%s': result=%d", srcFile.c_str(), (int)result);
        return nullptr;
    }

    auto out        = std::make_shared<Pcm>();
    out->data       = pData;
    out->frames     = frameCount;
    out->format     = cfg.format;
    out->channels   = cfg.channels;
    out->sampleRate = cfg.sampleRate;
    entry->pcm      = out;
    return out;
}

bool Sound3D::AttachPcm(const std::shared_ptr<Pcm>& p)
{
    ReleaseBuffer();
    if (!p) return false;

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(p->format, p->channels, p->frames, p->data, nullptr);
    bufferConfig.sampleRate = p->sampleRate;

    if (ma_audio_buffer_init(&bufferConfig, &buffer) != MA_SUCCESS) {
        LOG_ERROR("[Sound3D] ma_audio_buffer_init failed for '%s'", sourceFile.c_str());
        return false;
    }

    pcm          = p;
    bufferLoaded = true;
    return true;
}

void Sound3D::ReleaseBuffer()
{
    stopVoice();
    if (bufferLoaded) {
        ma_audio_buffer_uninit(&buffer);
        bufferLoaded = false;
    }
    pcm.reset();
}

Sound3D::Sound3D()
{
    renderSettings.frustumCulling = false;
}

Sound3D::~Sound3D()
{
    ReleaseBuffer();
}

void Sound3D::onUpdate()
{
    if (!bufferLoaded) return;

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
    float volScaled = (vol / 128.0f) * ambienceVolumeScale;

    // Paneo: componente de la dirección a la fuente sobre el eje derecho de la cámara (-1 izquierda, +1 derecha).
    // Se atenúa cerca de la fuente: dentro de ~2 u la dirección cambia muy deprisa y el sonido bailaría.
    float pan = 0.0f;
    if (panningEnabled) {
        const Vertex3D toSource = position - camPos;
        const float len = toSource.getModule();
        if (len > 0.001f) {
            const Vertex3D camRight = cam->getRotation().getTranspose() * Vertex3D(1, 0, 0);   // mismo eje derecho que usa Camera3D::UpdateVelocity
            const float side = (toSource.x * camRight.x + toSource.y * camRight.y + toSource.z * camRight.z) / len;
            pan = std::clamp(side, -1.0f, 1.0f) * std::clamp(len / 2.0f, 0.0f, 1.0f);
        }
    }

    if (volScaled > 0.0f) {
        if (!voiceInitialized) {
            auto* engine = Components::get()->Sound()->getEngine();

            if (ma_audio_buffer_ref_init(buffer.ref.format, buffer.ref.channels,
                                          buffer.ref.pData, buffer.ref.sizeInFrames, &ref) == MA_SUCCESS) {
                refInitialized = true;

                if (ma_sound_init_from_data_source(engine, (ma_data_source*)&ref,
                                                    MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &voice) == MA_SUCCESS) {
                    voiceInitialized = true;
                    ma_sound_set_looping(&voice, loop ? MA_TRUE : MA_FALSE);
                    ma_sound_set_volume(&voice, volScaled);
                    ma_sound_set_pan(&voice, pan);
                    ma_sound_start(&voice);
                    isPlaying = true;
                } else {
                    ma_audio_buffer_ref_uninit(&ref);
                    refInitialized = false;
                }
            }
        } else {
            ma_sound_set_volume(&voice, volScaled);
            ma_sound_set_pan(&voice, pan);
        }
    } else {
        if (voiceInitialized) {
            stopVoice();
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
