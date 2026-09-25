#include "../../include/Cache/TextureAnimatedCache.h"
#include "../../include/Misc/Logging.h"

TextureAnimatedCache textureAnimatedCache;

TextureAnimated* TextureAnimatedCache::getOrLoadTemplate(const std::string &spriteSheetFile, int frameWidth, int frameHeight, int numFrames, int fps)
{
    auto key = normalizePath(spriteSheetFile)
        + "|" + std::to_string(frameWidth)
        + "|" + std::to_string(frameHeight)
        + "|" + std::to_string(numFrames)
        + "|" + std::to_string(fps);

    auto cached = std::static_pointer_cast<TextureAnimated>(
        getOrLoadInternal(key, [&spriteSheetFile, frameWidth, frameHeight, numFrames, fps]() -> std::shared_ptr<void> {
            LOG_MESSAGE("[TextureAnimatedCache] Loading '%s'", spriteSheetFile.c_str());
            auto *tpl = new TextureAnimated(spriteSheetFile, frameWidth, frameHeight, numFrames, fps);
            tpl->LoadCurrentSetup();
            return std::shared_ptr<TextureAnimated>(tpl);
        })
    );

    return cached.get();
}
