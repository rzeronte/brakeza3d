
#include <SDL_surface.h>
#include <cmath>
#include <list>
#include "../../headers/Objects/Triangle3D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

Triangle::Triangle() {
    texture = NULL;
    parent = NULL;
}

Triangle::Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent)
{
    this->A = A;
    this->B = B;
    this->C = C;

    this->parent = parent;

    // la textura se carga bajo demanda
    texture = NULL;
}


void Triangle::updateVertexSpaces(Camera3D *cam)
{
    Ao = Transforms::objectSpace(A, parent);
    Bo = Transforms::objectSpace(B, parent);
    Co = Transforms::objectSpace(C, parent);

    Ac = Transforms::cameraSpace(Ao, cam);
    Bc = Transforms::cameraSpace(Bo, cam);
    Cc = Transforms::cameraSpace(Co, cam);

    An = Transforms::NDCSpace(Ac, cam);
    Bn = Transforms::NDCSpace(Bc, cam);
    Cn = Transforms::NDCSpace(Cc, cam);

    As = Transforms::screenSpace(An, cam);
    Bs = Transforms::screenSpace(Bn, cam);
    Cs = Transforms::screenSpace(Cn, cam);

    updateNormal();
}

void Triangle::updateNormal()
{
    Vector3D v1 = Vector3D(this->Ao, this->Bo);
    Vector3D v2 = Vector3D(this->Ao, this->Co);

    this->normal = v1.getComponent() % v2.getComponent();
}

Vertex3D Triangle::getNormal()
{
    return this->normal;
}

void Triangle::shadowMapping(LightPoint3D *lp)
{
    this->updateVertexSpaces(lp->cam);

    if (this->isBackFaceCulling(lp->cam))  {
        return;
    }

    if ( !lp->cam->frustum->isPointInFrustum(Ao) &&
         !lp->cam->frustum->isPointInFrustum(Bo) &&
         !lp->cam->frustum->isPointInFrustum(Co)
     ) {
        return;
    }

    this->scanVerticesForShadowMapping(lp);
}

bool Triangle::draw(Camera3D *cam)
{
    this->updateVertexSpaces(cam);

    bool faceCulling = false;
    if (EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING) {
        faceCulling = this->isBackFaceCulling(cam);
    }

    // Wireframe for faceculling polygons
    if (faceCulling) {
        if (EngineSetup::getInstance()->SHOW_WIREFRAME_FOR_BFC_HIDDEN_TRIANGLES) {
            Drawable::drawLine2D(Line2D(As.x, As.y, Bs.x, Bs.y), Color::yellow());
            Drawable::drawLine2D(Line2D(Bs.x, Bs.y, Cs.x, Cs.y), Color::yellow());
            Drawable::drawLine2D(Line2D(Cs.x, Cs.y, As.x, As.y), Color::yellow());

            //Drawable::drawVector3D( Vector3D(Ao, Bo), cam, Color::yellow());
            //Drawable::drawVector3D( Vector3D(Bo, Co), cam, Color::yellow());
            //Drawable::drawVector3D( Vector3D(Co, Ao), cam, Color::yellow());
        }
        EngineBuffers::getInstance()->trianglesHidenByFaceCuling++;
        return false;
    }

    // Clipping
    if (EngineSetup::getInstance()->TRIANGLE_RENDER_CLIPPING && !isClipped()) {
        if (this->clipping(cam)) {
            return false;
        }
    }

    // Frustum Culling
    if (EngineSetup::getInstance()->TRIANGLE_FRUSTUM_CULLING && !isClipped()) {
        if ( !cam->frustum->isPointInFrustum(Ao) && !cam->frustum->isPointInFrustum(Bo) && !cam->frustum->isPointInFrustum(Co) ) {
            EngineBuffers::getInstance()->trianglesOutFrustum++;
            return false;
        }
    }

    // Pixels
    if (EngineSetup::getInstance()->TRIANGLE_MODE_PIXELS ) {
        Drawable::drawVertex(Co, cam, Color::red());
        Drawable::drawVertex(Bo, cam, Color::green());
        Drawable::drawVertex(Co, cam, Color::blue());
    }

    EngineBuffers::getInstance()->trianglesDrawed++;

    if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED || EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID) {
        this->scanVertices(cam);
    }

    if (EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME) {
        this->drawWireframe();
    }

    if (EngineSetup::getInstance()->TRIANGLE_RENDER_NORMAL) {
        this->drawNormal(cam, Color::white());
    }

    return true;
}

bool Triangle::clipping(Camera3D *cam)
{
    Vertex3D output_vertices[10] ; int num_outvertices   = 0;
    Vertex3D input_vertices[10]  ; int num_inputvertices = 0;

    input_vertices[0] = this->Ao; num_inputvertices++;
    input_vertices[1] = this->Bo; num_inputvertices++;
    input_vertices[2] = this->Co; num_inputvertices++;

    bool any_new_vertex = false;

    const int plane_init = EngineSetup::getInstance()->LEFT_PLANE;
    const int plane_end  = EngineSetup::getInstance()->BOTTOM_PLANE;

    // clip against planes
    for (int i = plane_init ; i <= plane_end ; i++) {
        bool isClip = Maths::ClippingPolygon(input_vertices, num_inputvertices, output_vertices, num_outvertices, i, cam);

        if (isClip) any_new_vertex = true;
        memcpy (&input_vertices, &output_vertices, sizeof(output_vertices));
        /*for (int j = 0; j < num_outvertices; j++) { input_vertices[j] = output_vertices[j]; }*/

        num_inputvertices = num_outvertices;
        num_outvertices = 0;
    }

    if ( any_new_vertex && num_inputvertices != 0) {
        Triangle new_triangles[10];
        int num_new_triangles = 0;

        Maths::TriangulatePolygon(num_inputvertices, input_vertices, this->getNormal(), new_triangles, num_new_triangles, parent, this->getTexture(), true);
        //this->triangulate(num_inputvertices, input_vertices,  this->getNormal(), new_triangles, num_new_triangles, parent, this->getTexture(),true);

        for (int i = 0; i < num_new_triangles; i++) {
            EngineBuffers::getInstance()->trianglesClippingCreated++;
            new_triangles[i].draw(cam);
        }

        return true;
    }

    return false;
}

void Triangle::drawWireframe()
{
    Drawable::drawLine2D(Line2D(As.x, As.y, Bs.x, Bs.y), Color::red());
    Drawable::drawLine2D(Line2D(Bs.x, Bs.y, Cs.x, Cs.y), Color::green());
    Drawable::drawLine2D(Line2D(Cs.x, Cs.y, As.x, As.y), Color::blue());
}

// (v0 - P) . N
bool Triangle::isBackFaceCulling(Camera3D *cam)
{
    // Camera-triangle vector
    Vertex3D v = this->Ao - *cam->getPosition();

    return (v * this->getNormal()) >= 0;
}

Vertex3D Triangle::getCenter()
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

void Triangle::scanVertices(Camera3D *cam)
{
    // y obtenemos los puntos en la proyección 2d
    Point2D v1 = this->As;
    Point2D v2 = this->Bs;
    Point2D v3 = this->Cs;

    // Ordenamos los vertices y puntos por su valor en 'y'
    Maths::sortPointsByY(v1, v2, v3);

    if (v2.y == v3.y) {
        this->scanBottomFlatTriangle(v1, v2, v3);
    } else if (v1.y == v2.y) {
        this->scanTopFlatTriangle(v1, v2, v3);
    } else {
        // En este caso vamos a dividir el triángulo en dos
        // para tener uno que cumpla 'bottomFlat' y el otro 'TopFlat' y necesitamos un punto extra para separar ambos
        const int x = (int) (v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.x - v1.x));
        const int y = (int) v2.y;

        const Point2D v4(x, y);

        this->scanBottomFlatTriangle(v1, v2, v4);
        this->scanTopFlatTriangle(v4, v2, v3);

        if (EngineSetup::getInstance()->TRIANGLE_DEMO_EXTRALINE_ENABLED) {
            const Line2D extraLineDemo = Line2D(v4.x, v4.y, v2.x, v2.y);
            Drawable::drawLine2D(extraLineDemo, EngineSetup::getInstance()->TRIANGLE_DEMO_EXTRALINE_COLOR);
        }
    }
}

void Triangle::scanTopFlatTriangle(Point2D pa, Point2D pb, Point2D pc)
{
    const float invslope1 = (pc.x - pa.x) / (pc.y - pa.y);
    const float invslope2 = (pc.x - pb.x) / (pc.y - pb.y);

    float curx1 = pc.x;
    float curx2 = pc.x;

    for (int scanlineY = (int) pc.y; scanlineY > pa.y; scanlineY--) {
        this->scanLine(curx1, curx2, scanlineY);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void Triangle::scanBottomFlatTriangle(Point2D pa, Point2D pb, Point2D pc)
{
    const float invslope1 = (pb.x - pa.x) / (pb.y - pa.y);
    const float invslope2 = (pc.x - pa.x) / (pc.y - pa.y);

    float curx1 = pa.x;
    float curx2 = pa.x;

    for (int scanlineY = (int) pa.y; scanlineY <= pb.y; scanlineY++) {
        this->scanLine(curx1, curx2, scanlineY);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void Triangle::scanVerticesForShadowMapping(LightPoint3D *lp)
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
}

void Triangle::scanShadowMappingTopFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float invslope1 = (float) (pc.x - pa.x) / (pc.y - pa.y);
    float invslope2 = (float) (pc.x - pb.x) / (pc.y - pb.y);

    float curx1 = pc.x;
    float curx2 = pc.x;

    for (int scanlineY = pc.y; scanlineY > pa.y; scanlineY--) {
        this->scanShadowMappingLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, lp);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void Triangle::scanShadowMappingBottomFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float invslope1 = (float) (pb.x - pa.x) / (pb.y - pa.y);
    float invslope2 = (float) (pc.x - pa.x) / (pc.y - pa.y);

    float curx1 = pa.x;
    float curx2 = pa.x;

    for (int scanlineY = pa.y; scanlineY <= pb.y; scanlineY++) {
        this->scanShadowMappingLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, lp);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void Triangle::scanLine(float start_x, float end_x, const int y)
{
    if (start_x == end_x) return;

    if (start_x > end_x) {
        const int tmp = (int) start_x;
        start_x = end_x;
        end_x = tmp;
    }

    for (int x = (int) start_x ; x < end_x; x++) {
        const Point2D p = Point2D(x, y);
        processPixel(p);
    }
}

void Triangle::processPixel(const Point2D &pointFinal)
{
    if (Tools::isPixelInWindow((int) pointFinal.x, (int) pointFinal.y)) {
        // Hayamos las coordenadas baricéntricas del punto (x,y) respecto al triángulo As, Bs, Cs
        float alpha, theta, gamma;
        Maths::getBarycentricCoordinates(alpha, theta, gamma, pointFinal.x, pointFinal.y, As, Bs, Cs);
        Uint32 pixelColor = (Uint32) Tools::createRGB(alpha * 255, theta * 255, gamma * 255);

        // EngineBuffers
        if (EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER) {
            const float z = alpha * An.z + theta * Bn.z + gamma * Cn.z;               // Homogeneous clipspace
            const float buffer_z = EngineBuffers::getInstance()->getDepthBuffer(pointFinal.x, pointFinal.y);

            if (buffer_z != NULL) {
                if ( z < buffer_z ) {
                    EngineBuffers::getInstance()->setDepthBuffer(pointFinal.x, pointFinal.y, z);
                } else {
                    return;
                }
            }  else {
                EngineBuffers::getInstance()->setDepthBuffer(pointFinal.x, pointFinal.y, z);
            }
        }

        // Texture
        if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
            if (this->getTexture() != NULL) {
                // UV texture coordinates
                float u = alpha * An.u + theta * Bn.u + gamma * Cn.u;
                float v = alpha * An.v + theta * Bn.v + gamma * Cn.v;

                float ignorablePartInt;
                if (!std::signbit(u)) {
                    u = modf(abs(u) , &ignorablePartInt);
                } else {
                    u = 1 - modf(abs(u) , &ignorablePartInt);
                }

                // Check for inversion U
                if (!std::signbit(v)) {
                    v = modf(abs(v) , &ignorablePartInt);
                } else {
                    v = 1 - modf(abs(v) , &ignorablePartInt);
                }

                pixelColor = Tools::readSurfacePixelFromUV(texture->getSurface(), u, v);
                Uint8 red, green, blue, alpha;
                SDL_GetRGBA(pixelColor, texture->getSurface()->format, &red, &green, &blue, &alpha);

                if (alpha == 0) {
                    return;
                }
            }
        }

        if (EngineSetup::getInstance()->ENABLE_LIGHTS) {
            Vertex3D D;

            if (this->numberLightPoints > 0) {
                // Coordenadas del punto que estamos procesando en el mundo (object space)
                float x3d = alpha * Ao.x + theta * Bo.x + gamma * Co.x;
                float y3d = alpha * Ao.y + theta * Bo.y + gamma * Co.y;
                float z3d = alpha * Ao.z + theta * Bo.z + gamma * Co.z;

                D = Vertex3D( x3d, y3d, z3d ); // Object space
            }

            for (int i = 0; i < this->numberLightPoints; i++) {
                if (!this->lightPoints[i]->isEnabled()) {
                    continue;
                }

                // Color light apply
                float d = Maths::distanteBetweenpoints( *this->lightPoints[i]->getPosition(), D );
                pixelColor = Maths::mixColor(pixelColor, d, this->lightPoints[i], D);

                if (EngineSetup::getInstance()->ENABLE_SHADOW_CASTING) {
                    Mesh3D *isMesh = dynamic_cast<Mesh3D*> (parent);

                    if (isMesh != NULL && isMesh->isShadowCaster()) {
                        // Shadow test
                        Vertex3D Dl = Transforms::cameraSpace(D, this->lightPoints[i]->cam);
                        Dl = Transforms::NDCSpace(Dl, this->lightPoints[i]->cam);
                        const Point2D DP = Transforms::screenSpace(Dl, this->lightPoints[i]->cam);

                        if (Tools::isPixelInWindow(DP.x, DP.y)) {
                            float buffer_shadowmapping_z = this->lightPoints[i]->getShadowMappingBuffer(DP.x, DP.y);
                            if ( Dl.z > buffer_shadowmapping_z) {
                                pixelColor = Color::red();
                            }
                        }
                    }
                }
            }
        }

        EngineBuffers::getInstance()->pixelesDrawed++;
        EngineBuffers::getInstance()->setVideoBuffer(pointFinal.x, pointFinal.y, pixelColor);
    } else {
        EngineBuffers::getInstance()->pixelesOutOfWindow++;
    }
}

void Triangle::scanShadowMappingLine(float start_x, float end_x, int y,
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
}

Texture *Triangle::getTexture() const
{
    return texture;
}

void Triangle::setTexture(Texture *t)
{
    texture = t;
}

void Triangle::setLightPoints(LightPoint3D **lightPoints, int number)
{
    this->lightPoints = lightPoints;
    this->numberLightPoints = number;
}

void Triangle::setClipped(bool value)
{
    this->is_clipped = value;
}

bool Triangle::isClipped()
{
    return this->is_clipped;
}

void Triangle::setBSP(bool value)
{
    this->is_bsp = value;
}

bool Triangle::isBSP()
{
    return this->is_bsp;
}

int Triangle::triangulate(int num_vertex, Vertex3D *vertices, Vertex3D normal, Triangle *triangle, int &ntriangles, Object3D *parent, Texture *texture, bool clipped)
{
    Vertex3D middle = Maths::getCenterVertices(vertices, num_vertex);

    for (int i = 0; i < num_vertex ; i++) {
        Vertex3D tv1, tv2, tv3;
        // Vertex new triangles
        int current = i;
        int next = i+1;

        if (next < num_vertex){
            tv1 = Transforms::objectToLocal(middle, parent);
            tv2 = Transforms::objectToLocal(vertices[current], parent);
            tv3 = Transforms::objectToLocal(vertices[next], parent);
        } else {
            tv1 = Transforms::objectToLocal(middle, parent);
            tv2 = Transforms::objectToLocal(vertices[current], parent);
            tv3 = Transforms::objectToLocal(vertices[0], parent);
        }

        Triangle t = Triangle(tv1, tv2, tv3, parent);
        t.setTexture( texture );
        t.setClipped(clipped);
        EngineBuffers::getInstance()->trianglesClippingCreated++;

        triangle[ntriangles] = t;
        ntriangles++;
    }
}


