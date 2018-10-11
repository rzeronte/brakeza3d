
#include "../headers/Sprite.h"
#include "../headers/Drawable.h"

Sprite::Sprite(std::string base_name)
{
    this->base_name = base_name;
    this->billboard = new BillboardDirectional();

    this->billboard->loadSprite(base_name);
}

BillboardDirectional *Sprite::getBillboard() const {
    return billboard;
}

void Sprite::setBillboard(BillboardDirectional *billboard) {
    Sprite::billboard = billboard;
}

void Sprite::draw(Camera *cam)
{
    this->billboard->updateUnconstrainedQuad( 0.3, 0.3, this, cam->upVector(), cam->rightVector() );
    this->billboard->updateTextureFromCameraAngle(this, cam);

    Drawable::drawBillboard(this->billboard, cam );
}

