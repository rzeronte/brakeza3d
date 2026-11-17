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

#ifndef BRAKEZA3D_SOUND3D_H
#define BRAKEZA3D_SOUND3D_H

#include "Object3D.h"
#include "miniaudio.h"
#include <string>
#include <memory>

class Sound3D : public Object3D
{
public:
    static float ambienceVolumeScale;
    // Paneo izquierda/derecha según dónde queda la fuente respecto a la cámara (derecha de la cámara = canal derecho).
    // Desactivado (por defecto): sonido mono centrado, solo cambia el volumen con la distancia.
    // Se activa desde Lua con Components:Sound():setAmbiencePanning(true) (lo hace la demo Sound3D).
    static bool  panningEnabled;

    std::string sourceFile;

    // PCM decodificado una vez (buffer/pcm) + voz propia (ref/voice), fuera del
    // pool de 32 canales de ComponentSound -- este objeto vive toda la escena y su volumen
    // se recalcula cada frame segun distancia a camara (ver onUpdate()).
    // El PCM (pcm) es compartido entre todos los Sound3D que usan el mismo fichero: se decodifica
    // una sola vez (ver DecodeShared) y se libera cuando el ultimo lo suelta. 'buffer' es solo un
    // envoltorio por instancia sobre esos datos (ma_audio_buffer_init no copia).
    struct Pcm
    {
        void*        data       = nullptr;
        ma_uint64    frames     = 0;
        ma_format    format     = ma_format_f32;
        ma_uint32    channels   = 2;
        ma_uint32    sampleRate = 44100;
        ~Pcm() { if (data) ma_free(data, nullptr); }
    };

    ma_audio_buffer      buffer{};
    std::shared_ptr<Pcm> pcm;
    bool                 bufferLoaded     = false;
    ma_audio_buffer_ref  ref{};
    bool                 refInitialized   = false;
    ma_sound             voice{};
    bool                 voiceInitialized = false;

    float       innerRadius = 15.0f;
    float       outerRadius = 60.0f;
    int         baseVolume  = 128;
    bool        loop        = true;
    bool        isPlaying   = false;
    bool        debugDraw   = false;

    Sound3D();
    ~Sound3D() override;

    // Decodifica srcFile a PCM (44100Hz estereo f32, misma referencia que SoundPackage). Seguro
    // desde cualquier hilo: hay un bloqueo por fichero, asi que si varios Sound3D piden el mismo
    // fichero a la vez solo uno lo decodifica y los demas reutilizan el resultado.
    // Devuelve nullptr si el decode falla.
    static std::shared_ptr<Pcm> DecodeShared(const std::string& srcFile);

    // Engancha el PCM a esta instancia (barato; hilo principal). Libera el anterior si lo habia.
    bool AttachPcm(const std::shared_ptr<Pcm>& p);

    // Suelta el buffer y la referencia al PCM (la memoria se libera si nadie mas la usa).
    void ReleaseBuffer();

    void onUpdate()           override;
    void DrawPropertiesGUI()  override;
    GUIType::Sheet getIcon()  override;
    ObjectType getTypeObject() const override;

    // Libera la voz activa (ma_sound + ma_audio_buffer_ref) sin tocar el buffer PCM
    // decodificado -- se puede volver a sonar despues sin recargar el fichero.
    void stopVoice()
    {
        if (voiceInitialized) { ma_sound_uninit(&voice); voiceInitialized = false; }
        if (refInitialized)   { ma_audio_buffer_ref_uninit(&ref); refInitialized = false; }
        isPlaying = false;
    }

    void setEnabled(bool enabled) override
    {
        if (!enabled) {
            stopVoice();
        }
        Object3D::setEnabled(enabled);
    }
};

#endif //BRAKEZA3D_SOUND3D_H
