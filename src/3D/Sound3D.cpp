//
// Created by Eduardo on 10/07/2026.
//

#include "../../include/3D/Sound3D.h"
#include <algorithm>
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/Sound3DGUI.h"
#include "../../include/Misc/Logging.h"

float Sound3D::ambienceVolumeScale = 1.0f;

bool Sound3D::DecodeFile(const std::string& srcFile, ma_audio_buffer& outBuffer, void*& outData)
{
    ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, 2, 44100);
    ma_uint64 frameCount  = 0;
    void*     pData       = nullptr;

    ma_result result = ma_decode_file(srcFile.c_str(), &cfg, &frameCount, &pData);
    if (result != MA_SUCCESS) {
        LOG_ERROR("[Sound3D] ma_decode_file failed for '%s': result=%d", srcFile.c_str(), (int)result);
        return false;
    }

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(cfg.format, cfg.channels, frameCount, pData, nullptr);
    bufferConfig.sampleRate = cfg.sampleRate;

    if (ma_audio_buffer_init(&bufferConfig, &outBuffer) != MA_SUCCESS) {
        LOG_ERROR("[Sound3D] ma_audio_buffer_init failed for '%s'", srcFile.c_str());
        ma_free(pData, nullptr);
        return false;
    }

    outData = pData;
    return true;
}

Sound3D::Sound3D()
{
    renderSettings.frustumCulling = false;
}

Sound3D::~Sound3D()
{
    stopVoice();
    if (bufferLoaded) {
        ma_audio_buffer_uninit(&buffer);
        bufferLoaded = false;
    }
    if (pDecodedData) {
        ma_free(pDecodedData, nullptr);
        pDecodedData = nullptr;
    }
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
                    ma_sound_start(&voice);
                    isPlaying = true;
                } else {
                    ma_audio_buffer_ref_uninit(&ref);
                    refInitialized = false;
                }
            }
        } else {
            ma_sound_set_volume(&voice, volScaled);
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
