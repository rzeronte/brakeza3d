//
// Created by Eduardo on 10/07/2026.
//

#ifndef BRAKEZA3D_SOUND3D_H
#define BRAKEZA3D_SOUND3D_H

#include "Object3D.h"
#include "miniaudio.h"
#include <string>

class Sound3D : public Object3D
{
public:
    static float ambienceVolumeScale;

    std::string sourceFile;

    // PCM decodificado una vez (buffer/pDecodedData) + voz propia (ref/voice), fuera del
    // pool de 32 canales de ComponentSound -- este objeto vive toda la escena y su volumen
    // se recalcula cada frame segun distancia a camara (ver onUpdate()).
    ma_audio_buffer      buffer{};
    void*                pDecodedData     = nullptr;
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

    // Decodifica srcFile a PCM en memoria y lo envuelve en outBuffer (44100Hz estereo,
    // misma referencia que SoundPackage). outData debe liberarse con ma_free() por el
    // caller cuando outBuffer deje de usarse (ma_audio_buffer_init no copia los datos).
    // Devuelve false si el decode falla (outBuffer/outData quedan sin tocar).
    static bool DecodeFile(const std::string& srcFile, ma_audio_buffer& outBuffer, void*& outData);

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
