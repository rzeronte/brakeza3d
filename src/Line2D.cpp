#include <string>
#include "../headers/Line2D.h"
#include "../headers/Color.h"
#include "../headers/Tools.h"
#include "../headers/EngineBuffers.h"

Line2D::Line2D()
{
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
}

Line2D::Line2D(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

void Line2D::setup(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

void Line2D::draw(SDL_Surface *screen)
{
    this->draw(screen, Color::white());
}

void Line2D::draw(SDL_Surface *screen, Uint32 col)
{
    int x1 = this->x1;
    int y1 = this->y1;

    int x2 = this->x2;
    int y2 = this->y2;

    float y_diff = y2-y1;
    float x_diff = x2-x1;

    // Vamos a dibujar la línea de izquierda a derecha
    // por tanto, si el primer punto a dibujar está a la derecha
    // invertimos los puntos para que se pueda realizar de esta manera
    if (x1 > x2) {
        x1 = this->x2;
        y1 = this->y2;

        x2 = this->x1;
        y2 = this->y1;
    }

    // m = pendiente
    float m = (y_diff/x_diff);

    // De izquierda a derecha
    double x_rect, y_rect;
    int i;

    for (i = x1; i < x2; i++) {
        x_rect = i;
        y_rect = m * (x_rect-x1) + y1;
        if (Tools::isPixelInWindow(x_rect, y_rect)) {
            EngineBuffers::getInstance()->setVideoBuffer(x_rect, y_rect, col);
        }
    }

    // Reseteamos los valores originales
    x1 = this->x1; y1 = this->y1;
    x2 = this->x2; y2 = this->y2;

    if (y1 > y2) {
        x1 = this->x2; y1 = this->y2;
        x2 = this->x1; y2 = this->y1;
    }

    // de arriba a abajo
    for (i = y1; i < y2; i++) {
        y_rect = i;
        x_rect =  (y_rect-y1) / m  + x1;
        if (Tools::isPixelInWindow(x_rect, y_rect)) {
            EngineBuffers::getInstance()->setVideoBuffer(x_rect, y_rect, col);
        }
    }
}
