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

void Line2D::draw()
{
    this->draw(Color::white());
}

void Line2D::draw(Uint32 col)
{
    drawLineaBresenham(x1, y1, x2, y2, col);
    return;

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

void Line2D::drawLineaBresenham(int x1, int y1, int x2, int y2, Uint32 col)
{
    int pasoy;
    int pasox;
    int deltaX = (x2 - x1);
    int deltaY = (y2 - y1);

    if (deltaY < 0) {
        deltaY = -deltaY;
        pasoy = -1;
    } else {
        pasoy = 1;
    }


    if (deltaX < 0) {
        deltaX = -deltaX;
        pasox = -1;
    } else {
        pasox = 1;
    }

    int x = x1;
    int y = y1;
    //pantalla[x][y] = 'I';

    if (deltaX>deltaY){
        int p = 2 * deltaY - deltaX;
        int incE = 2 * deltaY;
        int incNE = 2 * (deltaY - deltaX);
        while (x != x2){
            x = x + pasox;
            if (p < 0){
                p = p + incE;
            } else {
                y = y + pasoy;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col);
            }
            //pantalla[x][y] = '*';
        }
    } else{
        int p = 2 * deltaX - deltaY;
        int incE = 2 * deltaX;
        int incNE = 2 * (deltaX - deltaY);
        while (y != y2){
            y = y + pasoy;
            if (p < 0) {
                p = p + incE;
            } else {
                x = x + pasox;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col);
            }
            //pantalla[x][y] = '*';

        }
    }

}
