
#include <utility>
#include "../../include/Render/TextureAnimated.h"
#include <SDL_image.h>
#include "../../include/Misc/Logging.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/TextureAnimatedAnimationGUI.h"

TextureAnimated::TextureAnimated(std::string baseFile, int numFrames, int fps)
:
    baseFilename(std::move(baseFile)),
    numberFramesToLoad(numFrames),
    fps(fps)
{
    LOG_MESSAGE("Loading 2D animation: %s", baseFilename.c_str());

    for (int i = 0; i < numberFramesToLoad; i++) {
        std::string file = this->baseFilename + "_" + std::to_string(i) + ".png";
        this->frames.push_back(new Image(file));
    }
    UpdateStep();
}

TextureAnimated::TextureAnimated(const TextureAnimated *textureAnimated)
:
    baseFilename(textureAnimated->baseFilename),
    numberFramesToLoad(textureAnimated->numberFramesToLoad),
    fps(textureAnimated->fps),
    endAnimation(textureAnimated->endAnimation),
    paused(textureAnimated->paused)
{
    frames = textureAnimated->frames;
    UpdateStep();
}

TextureAnimated::TextureAnimated(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
:
    baseFilename(spriteSheetFile),
    numberFramesToLoad(numFrames),
    fps(fps),
    currentSpriteWidth(spriteWidth),
    currentspriteHeight(spriteHeight)
{
    LOG_MESSAGE("Loading sheet: %s", spriteSheetFile.c_str());
    spriteSheetSurface = Tools::SafeIMGLoad(spriteSheetFile);
}

void TextureAnimated::LoadCurrentSetup()
{
    Apply(baseFilename, currentSpriteWidth, currentspriteHeight, numberFramesToLoad, fps);
}

void TextureAnimated::Apply(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
{
    DeleteFrames();

    LOG_MESSAGE("TextureAnimated Setup: (Sprite: %s, w: %d, h: %d, nf: %d, fps: %d)", spriteSheetFile.c_str(), spriteWidth, spriteHeight, numFrames, fps);

    currentSpriteWidth = spriteWidth;
    currentspriteHeight = spriteHeight;

    // IMG_Load puede devolver null (archivo corrupto, formato no soportado, ruta incorrecta) sin
    // que el constructor lo compruebe. Sin este guard, spriteSheetSurface->h de la línea de abajo
    // es un null pointer dereference — crash nativo 0xC0000005 en vez de un error controlado
    // (mismo caso que ThreadJobLoadImage::fnCallback ya cubre para el otro camino de carga).
    if (spriteSheetSurface == nullptr) {
        LOG_ERROR("[TextureAnimated] Apply: spriteSheetSurface is null for '%s', skipping", spriteSheetFile.c_str());
        // Mantener el invariante numberFramesToLoad == frames.size(): frames queda vacío (por el
        // DeleteFrames() de arriba) así que numberFramesToLoad también debe quedar en 0. Sin esto,
        // getNumFrames() seguía devolviendo el valor pedido (p.ej. 24) con frames vacío, y
        // nextFrame()/getCurrentFrame() indexaban un vector vacío más adelante — segundo crash.
        numberFramesToLoad = 0;
        return;
    }

    const int numRows = spriteSheetSurface->h / spriteHeight;
    const int numColumns = spriteSheetSurface->w / spriteWidth;

    const auto renderer = Components::get()->Window()->getRenderer();

    for (int row = 0; row < numRows; ++row) {
        for (int column = 0; column < numColumns; ++column) {
            if ((int) frames.size() >= numFrames) continue;

            SDL_Rect spriteRect = { column * spriteWidth, row * spriteHeight, spriteWidth, spriteHeight };

            SDL_Surface* destinySurface = SDL_CreateRGBSurfaceWithFormat(0, spriteWidth, spriteHeight, 32, spriteSheetSurface->format->format);

            SDL_BlitSurface(spriteSheetSurface, &spriteRect, destinySurface, nullptr);

            SDL_Texture* spriteTexture = SDL_CreateTextureFromSurface(renderer, destinySurface);
            if (!spriteTexture) {
                LOG_MESSAGE("Failed to create texture: %s", SDL_GetError());
                SDL_FreeSurface(spriteSheetSurface);
                return;
            }

            frames.push_back(new Image(destinySurface, spriteTexture));
        }
    }

    numberFramesToLoad = (int) frames.size();
    setFps(fps);

    UpdateStep();
}

int TextureAnimated::getNumFrames() const
{
    return numberFramesToLoad;
}

Image *TextureAnimated::getCurrentFrame() const
{
    if (frames.empty()) return nullptr;
    return this->frames[currentFrame % frames.size()];
}

void TextureAnimated::nextFrame()
{
    setEndAnimation(false);

    if (!isPaused()) {
        currentFrame++;
    }

    // update frame
    if (currentFrame >= this->getNumFrames()) {
        currentFrame = 0;

        // flag for check if we are in end of animation
        setEndAnimation(true);
    }
}

bool TextureAnimated::isEndAnimation() const
{
    return endAnimation;
}

void TextureAnimated::setEndAnimation(bool value)
{
    endAnimation = value;
}

bool TextureAnimated::isPaused() const {
    return paused;
}

void TextureAnimated::setPaused(bool value)
{
    paused = value;
}

int TextureAnimated::getFps() const
{
    return fps;
}

void TextureAnimated::setFps(int value)
{
    fps = value;
}

void TextureAnimated::UpdateStep()
{
    this->counter.setStep(1.0f / (float) getFps());
    this->counter.setEnabled(true);
}

void TextureAnimated::update()
{
    counter.update();

    if (counter.isFinished()) {
        counter.setEnabled(true);
        nextFrame();
    }
}

void TextureAnimated::DeleteFrames()
{
    for (auto f: frames){
        delete f;
    }
    frames.clear();
}

const std::string &TextureAnimated::getBaseFilename() const
{
    return baseFilename;
}

void TextureAnimated::drawImGuiProperties()
{
    TextureAnimatedDrawerGUI::DrawPropertiesGUI(this);
}