
#include <SDL_surface.h>
#include <cmath>
#include <list>
#include "../../headers/Objects/Triangle3D.h"
#include "../../headers/Misc/Tools.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/ComponentsManager.h"
#include "../../headers/Brakeza3D.h"

Triangle::Triangle()
{
    texture = NULL;
    parent = NULL;
    lightmap = new Texture();
}

Triangle::Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent)
{
    this->A = A;
    this->B = B;
    this->C = C;

    this->parent = parent;

    // la textura se carga bajo demanda
    texture = NULL;
    this->lod = 1;

    lightmap = new Texture();
}

void Triangle::updateObjectSpace()
{
    Transforms::objectSpace(Ao, A, parent);
    Transforms::objectSpace(Bo, B, parent);
    Transforms::objectSpace(Co, C, parent);
}

void Triangle::updateCameraSpace(Camera3D *cam)
{
    Transforms::cameraSpace(Ac, Ao, cam);
    Transforms::cameraSpace(Bc, Bo, cam);
    Transforms::cameraSpace(Cc, Co, cam);
}

void Triangle::updateNDCSpace(Camera3D *cam)
{
    An = Transforms::NDCSpace(Ac, cam);
    Bn = Transforms::NDCSpace(Bc, cam);
    Cn = Transforms::NDCSpace(Cc, cam);
}

void Triangle::updateScreenSpace()
{
    Transforms::screenSpace(As, An);
    Transforms::screenSpace(Bs, Bn);
    Transforms::screenSpace(Cs, Cn);
}

void Triangle::updateFullVertexSpaces(Camera3D *cam)
{
    this->updateObjectSpace();
    this->updateCameraSpace(cam);
    this->updateNDCSpace(cam);
    this->updateScreenSpace();
    this->updateNormal();
}

void Triangle::updateUVCache()
{
    if (getLightmap()->isLightMapped()) {
        getLightmapCoordinatesFromUV(light_u1, light_v1, A.u, A.v);
        getLightmapCoordinatesFromUV(light_u2, light_v2, B.u, B.v);
        getLightmapCoordinatesFromUV(light_u3, light_v3, C.u, C.v);
    }

    // texture coordinates
    if (this->getTexture() != NULL) {

        tex_u1 = A.u;
        tex_v1 = A.v;

        tex_u2 = B.u;
        tex_v2 = B.v;

        tex_u3 = C.u;
        tex_v3 = C.v;

        if (parent->isDecal()) {
            Decal *decalParent = dynamic_cast<Decal*> (parent);

            float decal_As = decalParent->getSCoord(Ao);
            float decal_At = decalParent->getTCoord(Ao);
            float decal_Bs = decalParent->getSCoord(Bo);
            float decal_Bt = decalParent->getTCoord(Bo);
            float decal_Cs = decalParent->getSCoord(Co);
            float decal_Ct = decalParent->getTCoord(Co);

            this->texture = decalParent->getSprite()->getCurrentTextureAnimation()->getCurrentFrame();

            tex_u1 = decal_As;
            tex_v1 = decal_At;

            tex_u2 = decal_Bs;
            tex_v2 = decal_Bt;

            tex_u3 = decal_Cs;
            tex_v3 = decal_Ct;
        }

        if (isBSP) {
            tex_u1 = A.u / getTexture()->getSurface(1)->w;
            tex_v1 = A.v / getTexture()->getSurface(1)->h;

            tex_u2 = B.u / getTexture()->getSurface(1)->w;
            tex_v2 = B.v / getTexture()->getSurface(1)->h;

            tex_u3 = C.u / getTexture()->getSurface(1)->w;
            tex_v3 = C.v / getTexture()->getSurface(1)->h;
        }

        light_u1_Ac_z = light_u1 / Ac.z;
        light_u2_Bc_z = light_u2 / Bc.z;
        light_u3_Cc_z = light_u3 / Cc.z;

        light_v1_Ac_z = light_v1 / Ac.z;
        light_v2_Bc_z = light_v2 / Bc.z;
        light_v3_Cc_z = light_v3 / Cc.z;

        tex_u1_Ac_z = tex_u1 / Ac.z;
        tex_u2_Bc_z = tex_u2 / Bc.z;
        tex_u3_Cc_z = tex_u3 / Cc.z;

        tex_v1_Ac_z = tex_v1 / Ac.z;
        tex_v2_Bc_z = tex_v2 / Bc.z;
        tex_v3_Cc_z = tex_v3 / Cc.z;

        persp_correct_Az = 1/Ac.z;
        persp_correct_Bz = 1/Bc.z;
        persp_correct_Cz = 1/Cc.z;
    }
}

void Triangle::getLightmapCoordinatesFromUV(float &lu, float &lv, float tex_u, float tex_v)
{
    float cu = tex_u;
    float cv = tex_v;

    float intpart;
    if (
        modf( getLightmap()->minu[0], &intpart) == 0 &&
        modf( getLightmap()->maxv[0], &intpart) == 0 &&
        modf( getLightmap()->minu[1], &intpart) == 0 &&
        modf( getLightmap()->maxv[1], &intpart) == 0
    ) {
        cu = cu / getLightmap()->extents[1];
        cv = cv / getLightmap()->extents[0];
    } else {
        cu = cu - getLightmap()->mins[1];
        cu = cu / getLightmap()->extents[1];

        cv = cv - getLightmap()->mins[0];
        cv = cv / getLightmap()->extents[0];
    }

    lu = cu;
    lv = cv;
}

void Triangle::updateTextureAnimated()
{
    if (this->getTexture() == NULL) return;

    if (this->getTexture()->animated) {
        this->timerTextureAnimatedFrameControl += Brakeza3D::get()->deltaTime / 1000;

        int maxFrames = this->getTexture()->numAnimatedFrames;

        if (currentBSPTextureAnimatedFrame <= maxFrames) {
            std::string baseName = this->getTexture()->getFilename().substr(2, this->getTexture()->getFilename().length());

            std::string n = "+" + std::to_string(currentBSPTextureAnimatedFrame) + baseName;

            float stepTime = (float) 1 / (float) maxFrames;

            if (this->timerTextureAnimatedFrameControl >= stepTime) {
                this->setTexture(ComponentsManager::get()->getComponentBSP()->getBSP()->getTexture(n));
                this->currentBSPTextureAnimatedFrame++;
                this->timerTextureAnimatedFrameControl = 0;
            }

            if ( currentBSPTextureAnimatedFrame >= maxFrames ) {
                currentBSPTextureAnimatedFrame = 0;
            }
        }
    }
}

void Triangle::updateNormal()
{

    this->normal = Vector3D(this->Ao, this->Bo).getComponent() % Vector3D(this->Ao, this->Co).getComponent();
}

Vertex3D Triangle::getNormal()
{
    return this->normal;
}

/*void Triangle::shadowMapping(LightPoint3D *lp)
{
    this->updateFullVertexSpaces(lp->cam);

    if (this->isBackFaceCulling(lp->cam->getPosition()))  {
        return;
    }

    if ( !lp->cam->frustum->isPointInFrustum(Ao) &&
         !lp->cam->frustum->isPointInFrustum(Bo) &&
         !lp->cam->frustum->isPointInFrustum(Co)
    ) {
        return;
    }

    this->scanVerticesForShadowMapping(lp);
}*/

void Triangle::clipping(Camera3D *cam, Plane *planes, int startPlaneIndex, int endPlaneIndex, Object3D *newTrianglesParent, std::vector<Triangle*> &triangles, bool isBSP)
{
    Vertex3D output_vertices[10] ; int num_outvertices   = 0;
    Vertex3D input_vertices[10]  ; int num_inputvertices = 0;

    input_vertices[0] = this->Ao; num_inputvertices++;
    input_vertices[1] = this->Bo; num_inputvertices++;
    input_vertices[2] = this->Co; num_inputvertices++;

    // clip against planes
    for (int i = startPlaneIndex ; i <= endPlaneIndex ; i++) {
        Maths::ClippingPolygon(input_vertices, num_inputvertices, output_vertices, num_outvertices, i, planes);

        memcpy (&input_vertices, &output_vertices, sizeof(output_vertices));
        /*for (int j = 0; j < num_outvertices; j++) { input_vertices[j] = output_vertices[j]; }*/

        num_inputvertices = num_outvertices;
        num_outvertices = 0;
    }

    if (num_inputvertices != 0) {
        int oldNumTriangles = triangles.size();

        Maths::TriangulatePolygon(
                num_inputvertices, input_vertices,
                this->getNormal(),
                triangles,
                newTrianglesParent,
                this->getTexture(),
                this->getLightmap(),
                true,
                isBSP,
                this->typelight
        );

        // update cache for clipped triangles (they are out from hide removal surface updating)
        for (int i = oldNumTriangles; i < triangles.size(); i++) {
            triangles[i]->lightmapIndexPattern  = this->lightmapIndexPattern;
            triangles[i]->lightmapIndexPattern2 = this->lightmapIndexPattern2;
            triangles[i]->lightmapIndexPattern3 = this->lightmapIndexPattern3;
            triangles[i]->lightmapIndexPattern4 = this->lightmapIndexPattern4;
            triangles[i]->updateFullVertexSpaces(cam);
            triangles[i]->updateUVCache();
            triangles[i]->updateBoundingBox();
            triangles[i]->updateFullArea();

            if (EngineSetup::getInstance()->RASTERIZER_OPENCL) {
                EngineBuffers::getInstance()->addOCLTriangle(triangles[i]->getOpenCL());
            }
        }
    }
}

void Triangle::updateBoundingBox()
{
    maxX = std::max(As.x, std::max(Bs.x, Cs.x));
    minX = std::min(As.x, std::min(Bs.x, Cs.x));
    maxY = std::max(As.y, std::max(Bs.y, Cs.y));
    minY = std::min(As.y, std::min(Bs.y, Cs.y));
}

int Triangle::updateLightmapFrame()
{
    int style;
    int length;
    for (int nl = 0; nl < getLightmap()->numLightmaps; nl++) {
        style = typelight[nl];
        float timeIncrement = Brakeza3D::get()->deltaTime / 100.0f;
        if ( style > 10) continue;
        switch(nl) {
            default:
            case 0:
                lightmapIndexPattern += timeIncrement;
                length = strlen(EngineSetup::getInstance()->LIGHT_PATTERNS[style]);
                if ((int) lightmapIndexPattern >= length) {
                    lightmapIndexPattern = 0;
                }
                break;
            case 1:
                lightmapIndexPattern2 += timeIncrement;
                length = strlen(EngineSetup::getInstance()->LIGHT_PATTERNS[style]);
                if ((int) lightmapIndexPattern2 >= length) {
                    lightmapIndexPattern2 = 0;
                }
                break;
            case 2:
                lightmapIndexPattern3 += timeIncrement;
                length = strlen(EngineSetup::getInstance()->LIGHT_PATTERNS[style]);
                if ((int) lightmapIndexPattern3 >= length) {
                    lightmapIndexPattern3 = 0;
                }
                break;
            case 3:
                lightmapIndexPattern4 += timeIncrement;
                length = strlen(EngineSetup::getInstance()->LIGHT_PATTERNS[style]);
                if ((int) lightmapIndexPattern4 >= length) {
                    lightmapIndexPattern4 = 0;
                }
                break;
        }
    }
}

float Triangle::updateFullArea()
{
    this->fullArea = Maths::orient2d(Bs, Cs, Point2D((int) As.x, (int) As.y));
    this->reciprocalFullArea = 1 / this->fullArea;
}

// (v0 - P) . N
bool Triangle::isBackFaceCulling(Vertex3D *position)
{
    // Camera-triangle vector
    Vertex3D v = this->Ao - *position;

    return (v * this->getNormal()) >= 0;
}

Vertex3D Triangle::getCenterOfMass()
{
    Vertex3D A;

    Vertex3D At = this->Ao;
    Vertex3D Bt = this->Bo;
    Vertex3D Ct = this->Co;

    A.x = (At.x+Bt.x+Ct.x)/3;
    A.y = (At.y+Bt.y+Ct.y)/3;
    A.z = (At.z+Bt.z+Ct.z)/3;

    return A;
}

void Triangle::drawNormal(Camera3D *cam, Uint32 color)
{
    Drawable::drawVector3D( Vector3D( this->Ao, this->getNormal() ), cam, color );
}

/*void Triangle::scanVerticesForShadowMapping(LightPoint3D *lp)
{
    Vertex3D Aos = this->Ao;
    Vertex3D Bos = this->Bo;
    Vertex3D Cos = this->Co;

    // Pasamos por la cámara
    Vertex3D A = Ac;
    Vertex3D B = Bc;
    Vertex3D C = Cc;

    A = Transforms::NDCSpace(A, lp->cam);
    B = Transforms::NDCSpace(B, lp->cam);
    C = Transforms::NDCSpace(C, lp->cam);

    // y obtenemos los puntos en la proyección 2d
    Point2D v1 = Transforms::screenSpace(A, lp->cam);
    Point2D v2 = Transforms::screenSpace(B, lp->cam);
    Point2D v3 = Transforms::screenSpace(C, lp->cam);

    // Ordenamos los vertices y puntos por su valor en 'y'
    Maths::sortPointsByY(v1, v2, v3);
    Maths::sortVerticesByY(A, B, C);
    Maths::sortVerticesByY(Aos, Bos, Cos);

    if (v2.y == v3.y) {
        this->scanShadowMappingBottomFlatTriangle(v1, v2, v3, A, B, C, lp);
    } else if (v1.y == v2.y) {
        this->scanShadowMappingTopFlatTriangle( v1, v2, v3, A, B, C, lp);
    } else {
        // En este caso tenemos vamos a dividir los triángulos
        // para tener uno que cumpla 'bottomFlat' y el otro 'TopFlat'
        // y necesitamos un punto extra para separar ambos.
        const int x = (v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.x - v1.x));
        const int y = v2.y;

        const Point2D v4(x, y);

        // Hayamos las coordenadas baricéntricas del punto v4 respecto al triángulo v1, v2, v3
        float alpha, theta, gamma;
        Maths::getBarycentricCoordinates(alpha, theta, gamma, v4.x, v4.y, v1, v2, v3);

        const float u = alpha * A.u + theta * B.u + gamma * C.u;
        const float v = alpha * A.v + theta * B.v + gamma * C.v;

        // Creamos un nuevo vértice que representa v4 (el nuevo punto creado) en el triángulo original
        Vertex3D D = Vertex3D(
                alpha * A.x + theta * B.x + gamma * C.x,
                alpha * A.y + theta * B.y + gamma * C.y,
                alpha * A.z + theta * B.z + gamma * C.z
        );

        D.u = u; D.v = v;

        this->scanShadowMappingBottomFlatTriangle(v1, v2, v4, A, B, D, lp);
        this->scanShadowMappingTopFlatTriangle(v2, v4, v3, B, D, C, lp);
    }
}*/

/*void Triangle::scanShadowMappingTopFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float invslope1 = (pc.x - pa.x) / (pc.y - pa.y);
    float invslope2 = (pc.x - pb.x) / (pc.y - pb.y);

    float curx1 = pc.x;
    float curx2 = pc.x;

    for (int scanlineY = pc.y; scanlineY > pa.y; scanlineY--) {
        this->scanShadowMappingLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, lp);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}*/

/*void Triangle::scanShadowMappingBottomFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float invslope1 = (pb.x - pa.x) / (pb.y - pa.y);
    float invslope2 = (pc.x - pa.x) / (pc.y - pa.y);

    float curx1 = pa.x;
    float curx2 = pa.x;

    for (int scanlineY = pa.y; scanlineY <= pb.y; scanlineY++) {
        this->scanShadowMappingLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, lp);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}*/

void Triangle::processPixelTexture(Uint32 &pixelColor, float tex_u, float tex_v)
{
    float ignorablePartInt;

    // Check for inversion U
    if (!std::signbit(tex_u)) {
        tex_u = modf(tex_u , &ignorablePartInt);
    } else {
        tex_u = 1 - modf(abs(tex_u) , &ignorablePartInt);
    }

    // Check for inversion V
    if (!std::signbit(tex_v)) {
        tex_v = modf(tex_v , &ignorablePartInt);
    } else {
        tex_v = 1 - modf(abs(tex_v) , &ignorablePartInt);
    }

    if (EngineSetup::getInstance()->TEXTURES_BILINEAR_INTERPOLATION) {
        pixelColor = Tools::readSurfacePixelFromBilinearUV(getTexture()->getSurface(lod), tex_u, tex_v);
        return;
    }

    pixelColor = Tools::readSurfacePixelFromUV(getTexture()->getSurface(lod), tex_u, tex_v);
}

void Triangle::processPixelLightmap(Uint32 &pixelColor, float light_u, float light_v)
{
    EngineSetup *engineSetup = EngineSetup::getInstance();

    float intpart;
    // Check for inversion U
    if (!std::signbit(light_u)) {
        light_u = modf(light_u , &intpart);
    } else {
        light_u = 1 - modf(abs(light_u) , &intpart);
    }

    // Check for inversion V
    if (!std::signbit(light_v)) {
        light_v = modf(light_v , &intpart);
    } else {
        light_v = 1 - modf(abs(light_v) , &intpart);
    }

    Uint32 lightmap_color;
    Uint8  lightmap_intensity = 0;
    char c = 10;

    for (int nl = 0; nl < getLightmap()->numLightmaps; nl++) {
        int indexPattern;
        int style = typelight[nl];
        if (style > 11) continue;
        switch(nl) {
            case 0:
                if (!engineSetup->LIGHTMAPS_BILINEAR_INTERPOLATION) {
                    lightmap_color = Tools::readSurfacePixelFromUV(getLightmap()->lightmap, light_v, light_u);
                } else {
                    lightmap_color = Tools::readSurfacePixelFromBilinearUV(getLightmap()->lightmap, light_v, light_u);
                }
                indexPattern = (int) lightmapIndexPattern;
                c = engineSetup->LIGHT_PATTERNS[style][indexPattern];
                break;
            case 1:
                if (!engineSetup->LIGHTMAPS_BILINEAR_INTERPOLATION) {
                    lightmap_color = Tools::readSurfacePixelFromUV(getLightmap()->lightmap2, light_v, light_u);
                } else {
                    lightmap_color = Tools::readSurfacePixelFromBilinearUV(getLightmap()->lightmap2, light_v, light_u);
                }
                indexPattern = (int) lightmapIndexPattern2;
                c = engineSetup->LIGHT_PATTERNS[style][indexPattern];
                break;
            case 2:
                if (!engineSetup->LIGHTMAPS_BILINEAR_INTERPOLATION) {
                    lightmap_color = Tools::readSurfacePixelFromUV(getLightmap()->lightmap3, light_v, light_u);
                } else {
                    lightmap_color = Tools::readSurfacePixelFromBilinearUV(getLightmap()->lightmap3, light_v, light_u);
                }
                indexPattern = (int) lightmapIndexPattern3;
                c = engineSetup->LIGHT_PATTERNS[style][indexPattern];
                break;
            case 3:
                if (!engineSetup->LIGHTMAPS_BILINEAR_INTERPOLATION) {
                    lightmap_color = Tools::readSurfacePixelFromUV(getLightmap()->lightmap4, light_v, light_u);
                } else {
                    lightmap_color = Tools::readSurfacePixelFromBilinearUV(getLightmap()->lightmap4, light_v, light_u);
                }
                indexPattern = (int) lightmapIndexPattern4;
                c = engineSetup->LIGHT_PATTERNS[style][indexPattern];
                break;
            default:
                lightmap_color = Tools::readSurfacePixelFromUV(getLightmap()->lightmap4, light_v, light_u);

        }
        lightmap_intensity = lightmap_intensity + Tools::getRedValueFromColor(lightmap_color)  * (c/10  * engineSetup->LIGHTMAPPING_INTENSITY); // RGB son iguales en un gris
    }

    Uint8 pred, pgreen, pblue, palpha;
    SDL_GetRGBA(pixelColor, texture->getSurface(lod)->format, &pred, &pgreen, &pblue, &palpha);

    pixelColor = (Uint32) Tools::createRGB(
        std::min(int((pred * engineSetup->TEXTURE_INTENSITY) * lightmap_intensity), (int) c),
        std::min(int((pgreen * engineSetup->TEXTURE_INTENSITY) * lightmap_intensity), (int) c),
        std::min(int((pblue * engineSetup->TEXTURE_INTENSITY) * lightmap_intensity), (int) c)
    );

    if (EngineSetup::getInstance()->SHOW_LIGHTMAPPING) {
        SDL_GetRGBA(lightmap_color, texture->getSurface(lod)->format, &pred, &pgreen, &pblue, &palpha);

        pixelColor = (Uint32) Tools::createRGB(
                std::min(int((pred) * lightmap_intensity ), (int) c),
                std::min(int((pgreen) * lightmap_intensity), (int) c),
                std::min(int((pblue) * lightmap_intensity), (int) c)
        );
    }
}

/*void Triangle::scanShadowMappingLine(float start_x, float end_x, int y,
                                     Point2D pa, Point2D pb, Point2D pc,
                                     Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float offset_self_shadow = 0.25f;

    if (start_x == end_x) return;

    // left to right
    if (start_x > end_x) {
        int tmp = (int) start_x;

        start_x = end_x;
        end_x = tmp;
    }

    float alpha, theta, gamma;

    for (int x = (int) start_x; x < end_x; x++) {
        const Point2D pointFinal(x, y);

        if (Tools::isPixelInWindow(pointFinal.x, pointFinal.y)) {
            // Hayamos las coordenadas baricéntricas del punto v4 respecto al triángulo pa, pb, pc
            Maths::getBarycentricCoordinates(alpha, theta, gamma, x, y, pa, pb, pc);

            float z = alpha * A.z + theta * B.z + gamma * C.z; // Homogeneous clipspace

            float buffer_shadowmap_z = lp->getShadowMappingBuffer(pointFinal.x, pointFinal.y);
            if (buffer_shadowmap_z != NULL) {
                if ( z < buffer_shadowmap_z ) {
                    lp->setShadowMappingBuffer(pointFinal.x, pointFinal.y, z + offset_self_shadow);
                }
            }  else {
                lp->setShadowMappingBuffer(pointFinal.x, pointFinal.y, z + offset_self_shadow);
            }
        }
    }
}*/

Texture *Triangle::getTexture() const
{
    return texture;
}

void Triangle::setTexture(Texture *t)
{
    texture = t;
}

Texture *Triangle::getLightmap() const
{
    return lightmap;
}

void Triangle::setLightmap(Texture *t)
{
    lightmap = t;
}

/*void Triangle::setLightPoints(std::vector<LightPoint3D *> &lightPoints)
{
    this->lightPoints = lightPoints;
}*/

void Triangle::setClipped(bool value)
{
    this->clipped = value;
}

bool Triangle::isClipped()
{
    return this->clipped;
}

bool Triangle::isPointInside(Vertex3D v)
{
    return Maths::PointInTriangle(v, Ao, Bo, Co);
}

int Triangle::processLOD()
{
    int lod = EngineSetup::getInstance()->LOAD_OF_DETAIL;

    if (getTexture() == NULL) return 0;

    if (getTexture()->isMipMapped() && EngineSetup::getInstance()->ENABLE_MIPMAPPING) {
        float area_screen  = Maths::TriangleArea(As.x, As.y, Bs.x, Bs.y, Cs.x, Cs.y);
        float area_texture = getTexture()->getAreaForVertices(A, B, C, 1);

        float r = area_texture / area_screen;

        int triangle_lod = Tools::int_floor(r);
        int clamped_lod = 1;

        // Range LOD selection
        if (triangle_lod < 10) {
            clamped_lod = 1;
        } else if (triangle_lod >= 10 && triangle_lod  < 15) {
            clamped_lod = 2;
        } else if (triangle_lod >= 15 && triangle_lod  < 25) {
            clamped_lod = 4;
        } else if (triangle_lod > 25) {
            clamped_lod = 8;
        }

        lod = clamped_lod;
    }

    return lod;
}

bool Triangle::testForClipping(Plane *planes, int startPlaneIndex, int endPlaneIndex)
{
    // clip against planes
    Vector3D edges[3];
    edges[0] = Vector3D(Ao, Bo);
    edges[1] = Vector3D(Bo, Co);
    edges[2] = Vector3D(Co, Ao);

    for (int i = startPlaneIndex ; i <= endPlaneIndex ; i++) {
        for (int e = 0 ; e < 3 ; e++) {
            if ( Maths::isVector3DClippingPlane(planes[i], edges[e]) > 1 ) {
                return true;
            }
        }
    }

    return false;
}

int Triangle::getId() const {
    return id;
}

void Triangle::setId(int id) {
    Triangle::id = id;
}

Plane Triangle::plane()
{
    return Plane(this->Ao, this->Bo, this->Co);
}

OCLTriangle Triangle::getOpenCL()
{
    OCLTriangle ot;

    ot.id = 0;

    ot.o_x = parent->getPosition()->x;
    ot.o_y = parent->getPosition()->y;
    ot.o_z = parent->getPosition()->z;

    ot.o_scale = parent->scale;

    M3 objRot = parent->getRotation();
    ot.oPitch = objRot.getPitch();
    ot.oYaw   = objRot.getYaw();
    ot.oRoll  = objRot.getRoll();

    ot.A_x = A.x; ot.A_y = A.y; ot.A_z = A.z;
    ot.B_x = B.x; ot.B_y = B.y; ot.B_z = B.z;
    ot.C_x = C.x; ot.C_y = C.y; ot.C_z = C.z;

    ot.As_x = As.x; ot.As_y = As.y;
    ot.Bs_x = Bs.x; ot.Bs_y = Bs.y;
    ot.Cs_x = Cs.x; ot.Cs_y = Cs.y;

    ot.An_z = An.z; ot.Bn_z = Bn.z; ot.Cn_z = Cn.z;

    return ot;
}