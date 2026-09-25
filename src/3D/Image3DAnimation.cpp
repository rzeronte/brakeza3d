
#include "../../include/3D/Image3DAnimation.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image3DAnimationGUI.h"
#include "../../include/Cache/TextureAnimatedCache.h"

Image3DAnimation::Image3DAnimation(const Vertex3D &position, float w, float h)
:
    width(w),
    height(h),
    currentAnimationIndex(0),
    billboard(new Image3D(position, w, h, nullptr))
{
    setPosition(position);
}

void Image3DAnimation::onUpdate()
{
    Object3D::onUpdate();

    // Aplicado sobre `this`, no sobre `billboard` -- ver nota en Image3DAnimation.h. `this` es
    // el Object3D cuya rotación/getModelMatrix() realmente consume el render() de postUpdate().
    if (towardsCamera) {
        LookAt(Components::get()->Camera()->getCamera()->getPosition());
    }

    this->UpdateTrianglesCoordinatesAndTexture();
}

// El dibujado real vive en postUpdate() (no en onUpdate()) a propósito: postUpdate() corre
// DESPUÉS de que toda la geometría opaca (vehículos, edificios) ya haya escrito su profundidad
// real en el G-Buffer este mismo frame -- es la fase que ComponentRender.cpp ya reserva para
// "objetos transparentes". Si renderFB=="scene", esto permite que el test de profundidad de
// OpenGL oculte el sprite correctamente detrás de geometría, en vez de dibujarlo siempre encima
// (comportamiento por defecto, renderFB=="foreground", sin buffer de profundidad -- ver
// ComponentWindow.cpp, foregroundFBO se limpia solo con GL_COLOR_BUFFER_BIT).
void Image3DAnimation::postUpdate()
{
    Object3D::postUpdate();

    // Sin animations o sin frames cargados (p.ej. IMG_Load falló para el sprite sheet, ver
    // TextureAnimated::Apply) no hay nada que dibujar este frame — evita el null pointer
    // dereference de getCurrentFrame()->getOGLTextureID().
    if (animations.empty()) return;
    Image *frame = getCurrentTextureAnimation()->getCurrentFrame();
    if (frame == nullptr) return;

    GLuint fbo = (renderFB == "scene")
        ? Components::get()->Window()->getSceneFramebuffer()
        : Components::get()->Window()->getForegroundFramebuffer();

    Components::get()->Render()->getShaders()->shaderOGLRender->render(
        this,
        frame->getOGLTextureID(),
        frame->getOGLTextureID(),
        billboard->getVertexBuffer(),
        billboard->getUVBuffer(),
        billboard->getNormalBuffer(),
        billboard->getVertices().size(),
        fbo
    );

    if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
        Components::get()->Render()->getShaders()->shaderOGLWireframe->render(
            getModelMatrix(),
            billboard->getVertexBuffer(),
            billboard->getUVBuffer(),
            billboard->getNormalBuffer(),
            billboard->getVertices().size(),
            Color::gray(),
            Components::get()->Window()->getSceneFramebuffer()
        );
        // Toca GL_BLEND directamente, fuera de la caché compartida de Fase 1.1.1 (ver Mesh3D::onUpdate()).
        Components::get()->Render()->InvalidateRenderStateCache();
    }
}

void Image3DAnimation::setRenderFB(const std::string &fb)
{
    renderFB = fb;
    // No se escribe profundidad propia en NINGÚN caso: si dos sprites semitransparentes se
    // solapan (p.ej. dos charcos de sangre cercanos), no queremos que el primero en dibujarse
    // tape al segundo por el test de profundidad -- solo la geometría opaca real debe ocluir.
    getRenderSettings().writeDepth = false;
    getRenderSettings().depthTest  = (renderFB == "scene");
    // "scene" se usa para decals planos en el suelo orientados una sola vez con LookAt (p.ej. el
    // charco de sangre), no con towardsCamera. Object3D::LookAt fija el eje Z local (= la normal
    // del quad, ver Image3D::ResetBuffersToSize) según su propia convención de signo, que no
    // garantiza que la cara visible quede orientada hacia una cámara que mira desde arriba --
    // con culling activo (el default), eso descarta el quad entero como cara trasera y el sprite
    // desaparece por completo (bug reportado: "el sprite de sangre ahora no se ve"). Un decal
    // tumbado en el suelo nunca se ve desde abajo, así que desactivar culling aquí es seguro y
    // evita depender de acertar el signo exacto de LookAt.
    getRenderSettings().culling = (renderFB != "scene");
}

void Image3DAnimation::CreateAnimation(const std::string& sprite, int w, int h, int numFrames, int fps)
{
    this->animations.emplace_back(new TextureAnimated(sprite, w, h, numFrames, fps));
}

// A diferencia de CreateAnimation, esto no toca disco salvo la primera vez que
// se pide este (sprite, w, h, numFrames, fps): TextureAnimatedCache guarda la
// plantilla ya cargada y recortada en frames (Image*/SDL_Texture), compartida
// entre todas las instancias. Aquí solo se copia (constructor de copia de
// TextureAnimated: comparte el vector de frames, resetea currentFrame/counter
// propios) para que cada Image3DAnimation reproduzca su animación de forma
// independiente sin recargar ni volver a recortar el sprite sheet.
void Image3DAnimation::CreateAnimationFromCache(const std::string& sprite, int w, int h, int numFrames, int fps)
{
    TextureAnimated *tpl = textureAnimatedCache.getOrLoadTemplate(sprite, w, h, numFrames, fps);
    this->animations.emplace_back(new TextureAnimated(tpl));
}

void Image3DAnimation::UpdateTexture()
{
    if (animations.empty()) return;

    getCurrentTextureAnimation()->counter.update();

    if (getCurrentTextureAnimation()->counter.isFinished()) {
        getCurrentTextureAnimation()->counter.setEnabled(true);
        getCurrentTextureAnimation()->nextFrame();
        if (this->isAutoRemoveAfterAnimation() && getCurrentTextureAnimation()->isEndAnimation()) {
            this->setRemoved(true);
        }
    }

    // getCurrentFrame() puede devolver null si el sprite sheet no cargó (ver
    // TextureAnimated::Apply). billboard->setImage acepta null sin crashear, pero mejor no
    // pisar la imagen previa con null innecesariamente.
    Image *frame = this->animations[currentAnimationIndex]->getCurrentFrame();
    if (frame != nullptr) {
        billboard->setImage(frame);
    }
}

void Image3DAnimation::UpdateTrianglesCoordinatesAndTexture()
{
    billboard->ResetBuffersToSize(width, height);
    UpdateTexture();
}

void Image3DAnimation::LoadAnimationFiles() const
{
    for (auto &a : animations) {
        a->LoadCurrentSetup();
    }
}

void Image3DAnimation::LinkTextureIntoAnotherImage3DAnimation(const Image3DAnimation *to)
{
    animations.clear();

    for (auto animation : to->animations) {
        animations.push_back(animation);
    }

    sharedTextures = true;
}

void Image3DAnimation::UpdateBillboardSize() const
{
    billboard->setWidth(width);
    billboard->setHeight(height);
}

void Image3DAnimation::setTowardsCamera(bool value)
{
    towardsCamera = value;
}

// Redimensiona el quad en caliente (p.ej. efecto "charco creciendo"). No hace falta llamar a
// UpdateBillboardSize() aparte: UpdateTrianglesCoordinatesAndTexture() reconstruye el buffer
// desde width/height cada frame en onUpdate().
void Image3DAnimation::setSize(float w, float h)
{
    width = w;
    height = h;
}

void Image3DAnimation::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image3DAnimationGUI::DrawPropertiesGUI(this);
}

void Image3DAnimation::setAnimation(int value)
{
    this->currentAnimationIndex = value;
}

void Image3DAnimation::setAutoRemoveAfterAnimation(bool value)
{
    autoRemoveAfterAnimation = value;
}

Image3DAnimation::~Image3DAnimation()
{
    delete billboard;

    if (!sharedTextures) {
        for (auto animation : animations) {
            delete animation;
        }
    }
}
