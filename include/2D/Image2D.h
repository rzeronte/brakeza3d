//
// Created by Eduardo on 12/01/2025.
//

#ifndef BRAKEZA3D_IMAGE2D_H
#define BRAKEZA3D_IMAGE2D_H

#include "../Render/Image.h"
#include "../3D/Object3D.h"

class VideoPlayer;

class Image2D : public Object3D
{
    int x = 0;
    int y = 0;

    float widthScale = 1.0f;
    float heightScale = 1.0f;

    Image *image = nullptr;
    std::string filepath;
    std::string videoPath;

    VideoPlayer *videoPlayer = nullptr;

public:
    Image2D() = default;
    explicit Image2D(const std::string &file, float widthScale = 1.0f, float heightScale = 1.0f);
    ~Image2D() override;

    void DrawPropertiesGUI() override;
    void onUpdate() override;
    void setSize(float w, float h);
    void setScreenPosition(int x, int y);
    void setFilePath(const std::string &filepath);
    void setImage(Image *value);
    void setVideoPlayer(VideoPlayer *vp);
    void loadVideo(const std::string &path);
    void stopVideo();
    [[nodiscard]] bool isVideoFinished() const;
    [[nodiscard]] bool hasVideo() const;

    [[nodiscard]] VideoPlayer   *getVideoPlayer() const { return videoPlayer; }
    [[nodiscard]] const std::string &getVideoPath() const { return videoPath; }

    [[nodiscard]] ObjectType getTypeObject() const override     { return ObjectType::Image2D; }
    GUIType::Sheet getIcon() override                           { return IconObject::IMAGE_2D; }
    [[nodiscard]] Image* getImage() const                       { return image; }

    friend class Image2DSerializer;
    friend class Image2DGUI;
};


#endif //BRAKEZA3D_IMAGE2D_H
