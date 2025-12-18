
#include <utility>
#include "../../include/Render/TextureAnimated.h"
#include <SDL_image.h>
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/TextureAnimatedAnimationGUI.h"

TextureAnimated::TextureAnimated(std::string baseFile, int numFrames, int fps)
:
    baseFilename(std::move(baseFile)),
    numberFramesToLoad(numFrames),
    fps(fps)
{
    Logging::Message("Loading 2D animation: %s", baseFilename.c_str());

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
    Logging::Message("Loading sheet: %s", spriteSheetFile.c_str());
    spriteSheetSurface = IMG_Load(spriteSheetFile.c_str());
}

void TextureAnimated::LoadCurrentSetup()
{
    Apply(baseFilename, currentSpriteWidth, currentspriteHeight, numberFramesToLoad, fps);
}

void TextureAnimated::Apply(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
{
    DeleteFrames();

    Logging::Message("TextureAnimated Setup: (Sprite: %s, w: %d, h: %d, nf: %d, fps: %d)", spriteSheetFile.c_str(), spriteWidth, spriteHeight, numFrames, fps);

    currentSpriteWidth = spriteWidth;
    currentspriteHeight = spriteHeight;

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
                Logging::Message("Failed to create texture: %s", SDL_GetError());
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
    return this->frames[currentFrame];
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