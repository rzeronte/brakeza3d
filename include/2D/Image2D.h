//
// Created by Eduardo on 12/01/2025.
//

#ifndef BRAKEZA3D_IMAGE2D_H
#define BRAKEZA3D_IMAGE2D_H

#include "../Render/Image.h"
#include "../3D/Object3D.h"

class Image2D : public Object3D
{
    int x = 0;
    int y = 0;

    int width = 0;
    int height = 0;

    Image *image = nullptr;
    std::string filepath;

public:
    Image2D() = default;
    Image2D(const std::string &file, int width, int height);

    void DrawPropertiesGUI() override;
    void onUpdate() override;
    void setSize(int x, int y);
    void setScreenPosition(int x, int y);
    void setFilePath(const std::string &filepath);
    void setImage(Image *value);

    ObjectType getTypeObject() const override           { return ObjectType::Image2D; }
    GUIType::Sheet getIcon() override                   { return IconObject::IMAGE_2D; }
    [[nodiscard]] Image* getImage() const               { return image; }

    friend class Image2DSerializer;
    friend class Image2DGUI;
};


#endif //BRAKEZA3D_IMAGE2D_H
