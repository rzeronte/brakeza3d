#include "../../include/Cache/ImageCache.h"
#include "../../include/Misc/Logging.h"

ImageCache imageCache;

Image* ImageCache::getOrLoad(const FilePath::ImageFile& path)
{
    auto normalized = normalizePath(path);
    auto cached = std::static_pointer_cast<Image>(
        getOrLoadInternal(normalized, [&normalized]() -> std::shared_ptr<void> {
            LOG_MESSAGE("[ImageCache] Loading '%s'", normalized.c_str());
            return std::make_shared<Image>(normalized);
        })
    );
    return cached.get();
}

void ImageCache::release(const FilePath::ImageFile& path)
{
    removeInternal(normalizePath(path));
}

void ImageCache::visit(std::function<void(const FilePath::ImageFile&, Image*)> visitor) const
{
    forEach([&visitor](const std::string& key, std::shared_ptr<void> ptr) {
        auto* img = static_cast<Image*>(ptr.get());
        if (img) {
            visitor(key, img);
        }
    });
}

size_t ImageCache::getMemoryUsage() const
{
    size_t total = 0;
    forEach([&total](const std::string&, std::shared_ptr<void> ptr) {
        auto* img = static_cast<Image*>(ptr.get());
        if (img && img->isLoaded() && img->getSurface()) {
            auto* surface = img->getSurface();
            total += static_cast<size_t>(surface->format->BytesPerPixel) *
                     static_cast<size_t>(surface->w) *
                     static_cast<size_t>(surface->h);
        }
    });
    return total;
}
