//
// Created by Eduardo on 12/01/2025.
//

#include "../../include/2D/Image2D.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/Image2DGUI.h"
#include "../../include/Misc/VideoPlayer.h"

Image2D::Image2D(const std::string &file, float widthScale, float heightScale)
:
    widthScale(widthScale),
    heightScale(heightScale),
    filepath(file)
{
    featuresGUI.position = false;
    featuresGUI.rotation = false;
    featuresGUI.scale = false;
    featuresGUI.collider = false;
}

Image2D::~Image2D()
{
    if (videoPlayer) {
        videoPlayer->stop();
        delete videoPlayer;
    }
}

void Image2D::onUpdate()
{
    Object3D::onUpdate();

    if (videoPlayer && !videoPlayer->isFinished())
        videoPlayer->onUpdate();

    if (!image || !image->isLoaded()) return;

    const int drawW = (int)(widthScale  * (float)Config::get()->screenWidth);
    const int drawH = (int)(heightScale * (float)Config::get()->screenHeight);
    image->DrawFlatAlpha(x, y, drawW, drawH, alpha, Components::get()->Window()->getBackgroundFramebuffer());
}

void Image2D::setVideoPlayer(VideoPlayer *vp)
{
    if (videoPlayer) {
        videoPlayer->stop();
        if (image == videoPlayer->getImage()) image = nullptr;
        delete videoPlayer;
    }
    videoPlayer = vp;
    if (vp) {
        image = vp->getImage();
    } else {
        videoPath.clear();
    }
}

void Image2D::loadVideo(const std::string &path)
{
    videoPath = path;
    auto *vp = new VideoPlayer(path);
    vp->start();
    setVideoPlayer(vp);
}

void Image2D::setSize(float w, float h)
{
    widthScale  = w;
    heightScale = h;
}

void Image2D::setScreenPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Image2D::setFilePath(const std::string &filepath)
{
    this->filepath = filepath;
}

void Image2D::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image2DGUI::DrawPropertiesGUI(this);
}

void Image2D::setImage(Image *value)
{
    image = value;
}