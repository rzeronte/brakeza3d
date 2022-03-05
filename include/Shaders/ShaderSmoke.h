//
// Created by eduardo on 4/3/22.
//

#ifndef BRAKEDA3D_SHADERSMOKE_H
#define BRAKEDA3D_SHADERSMOKE_H

#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../EngineBuffers.h"

#define MAX_PALETTE_VAR 1
#define MAX_PALETTE 256
#define MAX_FUME_HEIGHT 1
#define MAX_BLAZE_TYPE 2
#define MAX_FIRE_DIRECTION 4

enum ShaderSmokeDirections{
    FIRST = 0,
    SECOND = 1,
    THIRD = 2,
    FOURTH = 3,
};

class ShaderSmoke: public Shader {
public:
    Object3D *object;

    Uint32* sp;

    float wheel = 0.5;
    int pi = 0, bi = 0, direction = 0;
    Uint8** fireImg = new Uint8*[this->h];
    Uint32 firePalette[MAX_PALETTE_VAR][MAX_PALETTE];

    ShaderSmoke() {
        setDirection(ShaderSmokeDirections::FIRST);
        setPalette(firePalette);
        int x, y;

        for (y = 0; y < this->h; ++y)
            fireImg[y] = new Uint8[this->w];

        for (y = 0; y < this->h; ++y)
            for (x = 0; x < this->w; ++x)
                fireImg[y][x] = 0;
    }

    void clearSmokeBuffer() {
        int x, y;
        for (y = 0; y < this->h; ++y) {
            for (x = 0; x < this->w; ++x) {
                fireImg[y][x] = 0;
            }
        }
    }

    void onUpdate() override {
        if (!isEnabled()) return;
        int x, y;

        sp = (Uint32*)EngineBuffers::getInstance()->videoBuffer;
        if (object != nullptr && object->isStencilBufferEnabled()) {
            for (y = 0; y < this->h; ++y) {
                for (x = 0; x < this->w; ++x) {
                    if (object->getStencilBufferValue(x, y)) {
                        fireImg[y][x] = rand() % 256;
                    }
                }
            }
        }

        // Fire Direction
        sp = EngineBuffers::getInstance()->videoBuffer;
        if (direction == ShaderSmokeDirections::FIRST) {
            for (y = 0; y < this->h; ++y) {
                for (x = 0; x < this->w; ++x) {
                    fireImg[y][x] = (
                        getFireImg(y + 1, (x - 1 + this->w) % this->w) +
                        getFireImg(y + 1, x) +
                        getFireImg(y + 1, (x + 1) % this->w) +
                        getFireImg((y + 2) % this->h, x)
                    ) / (4 + wheel);


                    if (x == 0  || x == this->w - 1) {
                        fireImg[y][x] /= (4 + wheel);
                    }
                    if (y == 0  || y == this->h - 1) {
                        fireImg[y][x] /= (4 + wheel);
                    }

                    *sp++ = firePalette[pi][fireImg[y][x]];
                }
            }
        }
    }

    Uint8 getFireImg(int y, int x) {
        if (Tools::isPixelInWindow(x, y)) {
            return fireImg[y][x];
        }

        return 0;
    }

    bool setPalette(Uint32(*firePalette)[MAX_PALETTE])
    {
        //
        // Set Fire Palette #1
        //

        int i, H, S, L;
        Uint8* fireR, * fireG, * fireB, * fireA;
        for (i = 0; i < 256; ++i)
        {
            H = i / 3; S = 100; L = i < 128 ? (i / 1.28) : 100;

            fireB = (Uint8*)&firePalette[0][i];
            fireG = fireB + 1;
            fireR = fireB + 2;
            fireA = fireB + 3;
            *fireA = 0;

            HSLtoRGB(H, S, L, fireR, fireG, fireB);

        }
        return true;
    }

    bool HSLtoRGB(double H, double S, double L, Uint8* r, Uint8* g, Uint8* b)
    {
        if (H < 0 || H > 360) return false;
        else if (S < 0 || S > 100) return false;
        else if (L < 0 || L > 100) return false;

        double t1, t2, tr, tg, tb;
        double R, G, B;

        S /= 100; L /= 100;

        // Black and White
        if (S == 0)
        {
            R = G = B = (L * 255);
        }
        else
        {
            t1 = L < 0.5 ? L * (1 + S) : L + S - (L * S);
            t2 = 2 * L - t1;
            H = H / 360;

            tr = H + 0.333; tg = H; tb = H - 0.333;

            if (tr < 0) tr += 1; if (tb < 0) tb += 1; if (tg < 0) tg += 1;
            if (tr > 1) tr -= 1; if (tb > 1) tb -= 1; if (tg > 1) tg -= 1;

            // RED
            if (6 * tr < 1) R = t2 + (t1 - t2) * 6 * tr;
            else if (2 * tr < 1) R = t1;
            else if (3 * tr < 2) R = t2 + (t1 - t2) * (0.666 - tr) * 6;
            else R = t2;

            // GREEN
            if (6 * tg < 1) G = t2 + (t1 - t2) * 6 * tg;
            else if (2 * tg < 1) G = t1;
            else if (3 * tg < 2) G = t2 + (t1 - t2) * (0.666 - tg) * 6;
            else G = t2;

            // BLUE
            if (6 * tb < 1) B = t2 + (t1 - t2) * 6 * tb;
            else if (2 * tb < 1) B = t1;
            else if (3 * tb < 2) B = t2 + (t1 - t2) * (0.666 - tb) * 6;
            else B = t2;

            *r = round(R * 255);
            *g = round(G * 255);
            *b = round(B * 255);
        }

        return true;
    }

    bool addPalette(Uint32* rgba, int maxPals, Uint8 r1, Uint8 g1, Uint8 b1, ...)
    {

        Uint8 r0, g0, b0;
        Uint8* b, * g, * r, * a;
        Uint32 color;
        double rn, gn, bn, rf, gf, bf;

        int range, i, j, ri;

        // Compute for the range Range of each palette
        if (maxPals <= 1)
        {
            b = (Uint8*)&color; *b = b1;
            g = b + 1; *g = g1;
            r = b + 2; *r = r1;
            a = b + 3; *a = 0;

            for (i = 0; i < MAX_PALETTE; ++i)
                rgba[i] = color;
        }
        else
        {
            range = MAX_PALETTE / (maxPals - 1);

            // Preferred over than under
            if ((MAX_PALETTE / (maxPals - 1)) % 2 != 0) ++range;

            // Place the pointer to their respective positions
            b = (Uint8*)rgba;
            g = b + 1;
            r = b + 2;
            a = b + 3;

            va_list argPtr;
            va_start(argPtr, b1);

            j = 0;

            for (i = 1; i < maxPals; ++i)
            {
                rf = r0 = r1; gf = g0 = g1; bf = b0 = b1;

                r1 = va_arg(argPtr, Uint8);
                g1 = va_arg(argPtr, Uint8);
                b1 = va_arg(argPtr, Uint8);

                rn = (r1 - r0) / (double)range;
                gn = (g1 - g0) / (double)range;
                bn = (b1 - b0) / (double)range;

                ri = 0;
                do
                {
                    *r = rf; *g = gf; *b = bf; *a = 0;
                    rf += rn; gf += gn; bf += bn;
                    r += 4; g += 4; b += 4; a += 4;
                } while (++ri < range && j < MAX_PALETTE);
            }

            va_end(argPtr);

        }

        return true;
    }

    void setObject(Object3D *object) {
        clearSmokeBuffer();
        ShaderSmoke::object = object;
    }

    Object3D *getObject() const {
        return object;
    }

    void setDirection(int d) {
        if (d >= MAX_FIRE_DIRECTION) this->direction = 0;
        this->direction = d;
    }

    int getDirection() {
        return this->direction;
    }
};
#endif //BRAKEDA3D_SHADERSMOKE_H
