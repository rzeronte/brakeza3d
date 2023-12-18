
#include <utility>

#include "../../include/2D/TextureAnimated.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

TextureAnimated::TextureAnimated(std::string baseFile, int numFrames, int fps) :
    base_file(std::move(baseFile)),
    numberFramesToLoad(numFrames),
    currentFrame(0),
    fps(fps),
    endAnimation(false),
    paused(false)
{
    Logging::Message("Loading 2D animation: %s", base_file.c_str());

    for (int i = 0; i < numberFramesToLoad; i++) {
        std::string file = this->base_file + "_" + std::to_string(i) + ".png";
        this->frames.push_back(new Image(file));
    }
    updateStep();
}

TextureAnimated::TextureAnimated(TextureAnimated *textureAnimated)
:
    base_file(textureAnimated->base_file),
    numberFramesToLoad(textureAnimated->numberFramesToLoad),
    currentFrame(0),
    fps(textureAnimated->fps),
    endAnimation(textureAnimated->endAnimation),
    paused(textureAnimated->paused)
{
    for (auto texture : textureAnimated->frames) {
        frames.push_back(texture);
    }
    updateStep();
}

TextureAnimated::TextureAnimated(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
:
    fps(fps),
    paused(false),
    currentFrame(0),
    endAnimation(false)
{
    Logging::Message("Loading sheet: %s", spriteSheetFile.c_str());

    SDL_Surface* spriteSheetSurface = IMG_Load(spriteSheetFile.c_str());
    if (!spriteSheetSurface) {
        Logging::Log("Failed to load sprite sheet: %s", SDL_GetError());
        return;
    }

    const int numRows = spriteSheetSurface->h / spriteHeight;
    const int numColumns = spriteSheetSurface->w / spriteWidth;

    const auto renderer = ComponentsManager::get()->getComponentWindow()->getRenderer();

    for (int row = 0; row < numRows; ++row) {
        for (int column = 0; column < numColumns; ++column) {
            if ((int)frames.size() >= numFrames) continue;

            SDL_Rect spriteRect = { column * spriteWidth, row * spriteHeight, spriteWidth, spriteHeight };

            SDL_Surface* destinySurface = SDL_CreateRGBSurfaceWithFormat(0, spriteWidth, spriteHeight, 32, spriteSheetSurface->format->format);

            SDL_BlitSurface(spriteSheetSurface, &spriteRect, destinySurface, nullptr);

            SDL_Texture* spriteTexture = SDL_CreateTextureFromSurface(renderer, destinySurface);
            if (!spriteTexture) {
                Logging::Log("Failed to create texture: %s", SDL_GetError());
                SDL_FreeSurface(spriteSheetSurface);
                return;
            }
            Logging::Message("Loading frame: %d", frames.size());

            frames.push_back(new Image(destinySurface, spriteTexture));
        }
    }

    SDL_FreeSurface(spriteSheetSurface);

    numberFramesToLoad = (int) frames.size();
    updateStep();
}

int TextureAnimated::getNumFrames() const {
    return numberFramesToLoad;
}

Image *TextureAnimated::getCurrentFrame() {
    return this->frames[currentFrame];
}

void TextureAnimated::nextFrame() {
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

bool TextureAnimated::isEndAnimation() const {
    return endAnimation;
}

void TextureAnimated::setEndAnimation(bool value) {
    TextureAnimated::endAnimation = value;
}

bool TextureAnimated::isPaused() const {
    return paused;
}

void TextureAnimated::setPaused(bool value) {
    TextureAnimated::paused = value;
}

int TextureAnimated::getFps() const {
    return fps;
}

void TextureAnimated::setFps(int value) {
    TextureAnimated::fps = value;
}

void TextureAnimated::updateStep()
{
    float step = (float) 1 / (float) getFps();
    this->counter.setStep(step);
}

void TextureAnimated::update()
{
    counter.update();

    if (counter.isFinished()) {
        counter.setEnabled(true);
        nextFrame();

    }
}
