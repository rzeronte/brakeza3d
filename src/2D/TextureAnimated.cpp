
#include <utility>

#include "../../include/2D/TextureAnimated.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

TextureAnimated::TextureAnimated(std::string baseFile, int numFrames, int fps) :
        currentspriteHeight(0),
        currentSpriteWidth(0),
        baseFilename(std::move(baseFile)),
        numberFramesToLoad(numFrames),
        currentFrame(0),
        fps(fps),
        endAnimation(false),
        paused(false)
{
    Logging::Message("Loading 2D animation: %s", baseFilename.c_str());

    for (int i = 0; i < numberFramesToLoad; i++) {
        std::string file = this->baseFilename + "_" + std::to_string(i) + ".png";
        this->frames.push_back(new Image(file));
    }
    updateStep();
}

TextureAnimated::TextureAnimated(TextureAnimated *textureAnimated)
        :
        currentspriteHeight(0),
        currentSpriteWidth(0),
        baseFilename(textureAnimated->baseFilename),
        numberFramesToLoad(textureAnimated->numberFramesToLoad),
        currentFrame(0),
        fps(textureAnimated->fps),
        endAnimation(textureAnimated->endAnimation),
        paused(textureAnimated->paused)
{
    frames = textureAnimated->frames;
    updateStep();
}

TextureAnimated::TextureAnimated(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
        :
        baseFilename(spriteSheetFile),
        numberFramesToLoad(numFrames),
        fps(fps),
        paused(false),
        currentFrame(0),
        endAnimation(false),
        currentSpriteWidth(spriteWidth),
        currentspriteHeight(spriteHeight)
{
    Logging::Message("Loading sheet: %s", spriteSheetFile.c_str());

    setup(spriteSheetFile, spriteWidth, spriteHeight, numFrames, fps);
}

void TextureAnimated::setup(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
{
    deleteFrames();

    Logging::Message("TextureAnimated Setup: (Sprite: %s, w: %d, h: %d, nf: %d, fps: %d)", spriteSheetFile.c_str(), spriteWidth, spriteHeight, numFrames, fps);

    SDL_Surface* spriteSheetSurface = IMG_Load(spriteSheetFile.c_str());
    if (!spriteSheetSurface) {
        Logging::Log("Failed to load sprite sheet: %s", SDL_GetError());
        return;
    }

    currentSpriteWidth = spriteWidth;
    currentspriteHeight = spriteHeight;

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

            frames.push_back(new Image(destinySurface, spriteTexture));
        }
    }

    SDL_FreeSurface(spriteSheetSurface);

    numberFramesToLoad = (int) frames.size();
    setFps(fps);

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

void TextureAnimated::deleteFrames()
{
    for (auto f: frames){
        delete f;
    }
    frames.clear();
}

const std::string &TextureAnimated::getBaseFilename() const {
    return baseFilename;
}

void TextureAnimated::drawImGuiProperties()
{
    if (ImGui::TreeNode("Current Animation Setup")) {
        static int width = currentSpriteWidth;
        static int height = currentspriteHeight;
        static int numFrames = numberFramesToLoad;
        static int fps = this->fps;

        const int range_min_int = 1;
        const int range_max_int = 1280;

        ImGui::DragScalar("Sprite Width", ImGuiDataType_S32, &width,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
        ImGui::DragScalar("Sprite Height", ImGuiDataType_S32, &height,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
        ImGui::DragScalar("Number frames", ImGuiDataType_S32, &numFrames,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
        ImGui::DragScalar("FPS", ImGuiDataType_S32, &fps,1.f, &range_min_int, &range_max_int, "%d", 1.0f);

        if (ImGui::Button("Apply")) {
            auto spriteFile = getBaseFilename();
            setup(spriteFile, width, height, numFrames, fps);
            updateStep();
        }
        ImGui::TreePop();
    }
}
