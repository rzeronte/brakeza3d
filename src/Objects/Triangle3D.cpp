
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

void Triangle::updateVertexObjectSpace()
{
    Ao = Transforms::objectSpace(A, parent);
    Bo = Transforms::objectSpace(B, parent);
    Co = Transforms::objectSpace(C, parent);
}

void Triangle::updateVertexCameraSpace(Camera3D *cam)
{
    Ac = Transforms::cameraSpace(Ao, cam);
    Bc = Transforms::cameraSpace(Bo, cam);
    Cc = Transforms::cameraSpace(Co, cam);
}

void Triangle::shadowMapping(LightPoint3D *lp)
{
    this->updateVertexObjectSpace();
    this->updateVertexCameraSpace(lp->cam);

    if (this->faceCulling(lp))  {
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
    this->updateVertexObjectSpace();
    this->updateVertexCameraSpace(cam);

    bool faceCulling = false;
    if (EngineSetup::getInstance()->TRIANGLE_FACECULLING)  {
        //faceCulling = this->faceCulling(cam);
    }

    if (faceCulling) {
        EngineBuffers::getInstance()->trianglesHidenByFaceCuling++;
        return false;
    }

    // Frustum Culling
    if (EngineSetup::getInstance()->TRIANGLE_FRUSTUM_CULLING) {
        if ( !cam->frustum->isPointInFrustum(Ao) && !cam->frustum->isPointInFrustum(Bo) && !cam->frustum->isPointInFrustum(Co) ) {
            EngineBuffers::getInstance()->trianglesOutFrustum++;
            return false;
        }
    }

    // Clipping
    if (EngineSetup::getInstance()->TRIANGLE_RENDER_CLIPPING ) {
        if (!faceCulling) {
            if (this->clipping(cam)) {
                return false;
            }
        }
    }

    // Pixels
    if (EngineSetup::getInstance()->TRIANGLE_MODE_PIXELS ) {
        if (!faceCulling) {
            Drawable::drawVertex(Co, cam, Color::red());
            Drawable::drawVertex(Bo, cam, Color::green());
            Drawable::drawVertex(Co, cam, Color::blue());
        }
    }

    if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED || EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID) {
        if (!faceCulling) {
            if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
                if (this->getTexture() != NULL) {
                    this->scanVertices(cam);
                } else {
                    if (EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID) {
                        this->scanVertices(cam);
                    }
                    EngineBuffers::getInstance()->trianglesNoTexture++;
                }
            }
        }
    }

    if (EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME) {
        if (!faceCulling) {
            this->drawWireframe(cam);
        }
    }

    if (EngineSetup::getInstance()->TRIANGLE_RENDER_NORMAL) {
        this->drawNormal(cam, Color::white());
    }

    return true;
}

bool Triangle::clipping(Camera3D *cam)
{

    Vertex3D At = this->Ao;
    Vertex3D Bt = this->Bo;
    Vertex3D Ct = this->Co;

    Vector3D AB = Vector3D(At, Bt);
    Vector3D AC = Vector3D(At, Ct);
    Vector3D BC = Vector3D(Bt, Ct);

    int plane_init = EngineSetup::getInstance()->FAR_PLANE; int plane_end = EngineSetup::getInstance()->BOTTOM_PLANE;

    // un triángulo como máximo tendrá 9 puntos de intersección en un frustum
    Vertex3D new_vertexes[10]; int num_new_vertexes = 0;
    Vertex3D temp_vertex[10];  int num_tmp_vertex = 0;

    int num_vertex_against_frustum = 0;

    num_tmp_vertex = 0;
    // AB
    if ( Maths::isVector3DClippingPlane( cam->frustum->planes[ EngineSetup::getInstance()->NEAR_PLANE ], AB ) ) {
        Vertex3D newVertex = cam->frustum->planes[EngineSetup::getInstance()->NEAR_PLANE].getPointIntersection(AB.vertex1, AB.vertex2);
        if (cam->frustum->isPointInFrustum(newVertex)) {
            new_vertexes[num_new_vertexes] = newVertex; num_new_vertexes++;
            num_vertex_against_frustum++;
        }
        temp_vertex[num_tmp_vertex] = newVertex; num_tmp_vertex++;
    }

    // AC
    if ( Maths::isVector3DClippingPlane( cam->frustum->planes[ EngineSetup::getInstance()->NEAR_PLANE ], AC )) {
        Vertex3D newVertex = cam->frustum->planes[EngineSetup::getInstance()->NEAR_PLANE].getPointIntersection(AC.vertex1, AC.vertex2);
        if (cam->frustum->isPointInFrustum(newVertex)) {
            new_vertexes[num_new_vertexes] = newVertex; num_new_vertexes++;
            num_vertex_against_frustum++;
        }
        temp_vertex[num_tmp_vertex] = newVertex; num_tmp_vertex++;
    }

    // BC
    if ( Maths::isVector3DClippingPlane( cam->frustum->planes[ EngineSetup::getInstance()->NEAR_PLANE ], BC ) ) {
        Vertex3D newVertex = cam->frustum->planes[EngineSetup::getInstance()->NEAR_PLANE].getPointIntersection(BC.vertex1, BC.vertex2);
        if (cam->frustum->isPointInFrustum(newVertex)) {
            new_vertexes[num_new_vertexes] = newVertex; num_new_vertexes++;
            num_vertex_against_frustum++;
        }
        temp_vertex[num_tmp_vertex] = newVertex; num_tmp_vertex++;
    }

    if (num_vertex_against_frustum > 0) {
        if (cam->frustum->isPointInFrustum(At)) {
            new_vertexes[num_new_vertexes] = At; num_new_vertexes++;
        }

        if (cam->frustum->isPointInFrustum(Bt)) {
            new_vertexes[num_new_vertexes] = Bt; num_new_vertexes++;
        }

        if (cam->frustum->isPointInFrustum(Ct)) {
            new_vertexes[num_new_vertexes] = Ct; num_new_vertexes++;
        }

        Vertex3D final_vertex[100]; int num_final_vertex = 0;
        for (int j = 0; j < num_new_vertexes; j++) {
            final_vertex[num_final_vertex] = new_vertexes[j]; num_final_vertex++;
            //Drawable::drawVertex(screen, new_vertexes[j], cam, Color::red());
        }

        Maths::triangulate(final_vertex, num_final_vertex, parent, cam, A, B, C, this->getTexture() );
        return true;
    }

    return false;
}

void Triangle::drawWireframe(Camera3D *cam)
{
    Vector3D vAB(Ao, Bo);
    Vector3D vBC(Bo, Co);
    Vector3D vCA(Co, Ao);

    Drawable::drawVector3D( vAB, cam, Color::red());
    Drawable::drawVector3D( vBC, cam, Color::green());
    Drawable::drawVector3D( vCA, cam, Color::blue());
}

bool Triangle::faceCulling(Object3D *cam)
{
    Vector3D vecNormal(this->getCenter(), this->getNormal());

    Vertex3D componente = vecNormal.getComponent().getNormalize();
    Vertex3D t = Transforms::objectSpace(A, parent);

    Vertex3D V (
        t.x - cam->getPosition()->x,
        t.y - cam->getPosition()->y,
        t.z - cam->getPosition()->z
    );

    // -V . Normal
    float val = (-V.x*componente.x) + (-V.y*componente.y) + (-V.z*componente.z);

    return val <= 0;
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

Vertex3D Triangle::calcNormalSurface(Vector3D u, Vector3D v)
{
    // Lo llevamos al origen
    Vertex3D U = u.getComponent();
    Vertex3D V = v.getComponent();

    float Wx = (U.y * V.z) - (U.z * V.y);
    float Wy = (U.z * V.x) - (U.x * V.z);
    float Wz = (U.x * V.y) - (U.y * V.x);

    Vertex3D R;

    R = Vertex3D(Wx, Wy, Wz);
    R = R.getNormalize();

    return R;
}

Vertex3D Triangle::getNormal()
{
    Vertex3D At = this->Ao;
    Vertex3D Bt = this->Bo;
    Vertex3D Ct = this->Co;

    Vector3D VnAB(At, Bt);
    Vector3D VnAC(At, Ct);

    Vertex3D normal = this->calcNormalSurface(VnAB, VnAC);

    // Como se calcula con la componente (respecto al origen),
    // lo vuelvo a poner sobre A y aplico distancia al centro

    normal.addVertex(At);

    normal.x+=this->getCenter().x-At.x;
    normal.y+=this->getCenter().y-At.y;
    normal.z+=this->getCenter().z-At.z;

    return normal;
}

void Triangle::drawNormal(Camera3D *cam, Uint32 color)
{
    Vector3D vecNormal(this->getCenter(), this->getNormal());

    Drawable::drawVector3D( vecNormal, cam, color );
}

/**
 * Rellena un triángulo de color
 * http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
 * https://www.davrous.com/2013/06/21/tutorial-part-4-learning-how-to-write-a-3d-software-engine-in-c-ts-or-js-rasterization-z-buffering/
 */
void Triangle::scanVertices(Camera3D *cam)
{
    Vertex3D Aos = this->Ao;
    Vertex3D Bos = this->Bo;
    Vertex3D Cos = this->Co;

    // Pasamos por la cámara
    Vertex3D A = Ac;
    Vertex3D B = Bc;
    Vertex3D C = Cc;

    A = Transforms::NDCSpace(A, cam);
    B = Transforms::NDCSpace(B, cam);
    C = Transforms::NDCSpace(C, cam);

    // y obtenemos los puntos en la proyección 2d
    Point2D v1 = Transforms::screenSpace(A, cam);
    Point2D v2 = Transforms::screenSpace(B, cam);
    Point2D v3 = Transforms::screenSpace(C, cam);

    // Ordenamos los vertices y puntos por su valor en 'y'
    Maths::sortPointsByY(v1, v2, v3);

    Maths::sortVertexByY(A, B, C);
    Maths::sortVertexByY(Aos, Bos, Cos);

    if (v2.y == v3.y) {
        this->scanBottomFlatTriangle(v1, v2, v3, A, B, C, Aos, Bos, Cos);
    } else if (v1.y == v2.y) {
        this->scanTopFlatTriangle( v1, v2, v3, A, B, C, Aos, Bos, Cos);
    } else {
        // En este caso tenemos vamos a dividir los triángulos
        // para tener uno que cumpla 'bottomFlat' y el otro 'TopFlat'
        // y necesitamos un punto extra para separar ambos.
        int x = (int) (v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.x - v1.x));
        int y = (int) v2.y;

        Point2D v4(x, y);

        // Hayamos las coordenadas baricéntricas del punto v4 respecto al triángulo v1, v2, v3
        float alpha, theta, gamma;
        Maths::getBarycentricCoordinates(alpha, theta, gamma, v4.x, v4.y, v1, v2, v3);

        float u = alpha * A.u + theta * B.u + gamma * C.u;
        float v = alpha * A.v + theta * B.v + gamma * C.v;

        // Creamos un nuevo vértice que representa v4 (el nuevo punto creado) en el triángulo original
        Vertex3D D = Vertex3D(
            alpha * A.x + theta * B.x + gamma * C.x,
            alpha * A.y + theta * B.y + gamma * C.y,
            alpha * A.z + theta * B.z + gamma * C.z
        );

        Vertex3D Dos = Vertex3D(
            alpha * Aos.x + theta * Bos.x + gamma * Cos.x,
            alpha * Aos.y + theta * Bos.y + gamma * Cos.y,
            alpha * Aos.z + theta * Bos.z + gamma * Cos.z
        );

        D.u = u; D.v = v;

        this->scanBottomFlatTriangle(v1, v2, v4, A, B, D, Aos, Bos, Dos);
        this->scanTopFlatTriangle(v2, v4, v3, B, D, C, Bos, Dos, Cos);

        if (EngineSetup::getInstance()->TRIANGLE_DEMO_EXTRALINE_ENABLED) {
            Line2D extraLineDemo = Line2D();
            extraLineDemo.setup(v4.x, v4.y, v2.x, v2.y);
            extraLineDemo.draw(EngineSetup::getInstance()->TRIANGLE_DEMO_EXTRALINE);
        }
    }

    EngineBuffers::getInstance()->trianglesDrawed++;
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
    Maths::sortVertexByY(A, B, C);
    Maths::sortVertexByY(Aos, Bos, Cos);

    if (v2.y == v3.y) {
        this->scanShadowMappingBottomFlatTriangle(v1, v2, v3, A, B, C, lp);
    } else if (v1.y == v2.y) {
        this->scanShadowMappingTopFlatTriangle( v1, v2, v3, A, B, C, lp);
    } else {
        // En este caso tenemos vamos a dividir los triángulos
        // para tener uno que cumpla 'bottomFlat' y el otro 'TopFlat'
        // y necesitamos un punto extra para separar ambos.
        int x = (int) (v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.x - v1.x));
        int y = (int) v2.y;

        Point2D v4(x, y);

        // Hayamos las coordenadas baricéntricas del punto v4 respecto al triángulo v1, v2, v3
        float alpha, theta, gamma;
        Maths::getBarycentricCoordinates(alpha, theta, gamma, v4.x, v4.y, v1, v2, v3);

        float u = alpha * A.u + theta * B.u + gamma * C.u;
        float v = alpha * A.v + theta * B.v + gamma * C.v;

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

void Triangle::scanTopFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, Vertex3D Aos, Vertex3D Bos, Vertex3D Cos)
{
    float invslope1 = (pc.x - pa.x) / (pc.y - pa.y);
    float invslope2 = (pc.x - pb.x) / (pc.y - pb.y);

    float curx1 = pc.x;
    float curx2 = pc.x;

    for (int scanlineY = (int) pc.y; scanlineY > pa.y; scanlineY--) {

        this->scanLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, Aos, Bos, Cos);

        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void Triangle::scanBottomFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, Vertex3D Aos, Vertex3D Bos, Vertex3D Cos)
{
    float invslope1 = (pb.x - pa.x) / (pb.y - pa.y);
    float invslope2 = (pc.x - pa.x) / (pc.y - pa.y);

    float curx1 = pa.x;
    float curx2 = pa.x;

    for (int scanlineY = (int) pa.y; scanlineY <= pb.y; scanlineY++) {

        this->scanLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, Aos, Bos, Cos);

        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void Triangle::scanShadowMappingTopFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float invslope1 = (pc.x - pa.x) / (pc.y - pa.y);
    float invslope2 = (pc.x - pb.x) / (pc.y - pb.y);

    float curx1 = pc.x;
    float curx2 = pc.x;

    for (int scanlineY = (int) pc.y; scanlineY > pa.y; scanlineY--) {

        this->scanShadowMappingLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, lp);

        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void Triangle::scanShadowMappingBottomFlatTriangle(Point2D pa, Point2D pb, Point2D pc, Vertex3D A, Vertex3D B, Vertex3D C, LightPoint3D *lp)
{
    float invslope1 = (pb.x - pa.x) / (pb.y - pa.y);
    float invslope2 = (pc.x - pa.x) / (pc.y - pa.y);

    float curx1 = pa.x;
    float curx2 = pa.x;

    for (int scanlineY = (int) pa.y; scanlineY <= pb.y; scanlineY++) {

        this->scanShadowMappingLine(curx1, curx2, scanlineY, pa, pb, pc, A, B, C, lp);

        curx1 += invslope1;
        curx2 += invslope2;
    }
}

/**
 * Escanea una línea horizontal entre x1 y x2 en una coordenada y indicada.
 */
void Triangle::scanLine(float start_x, float end_x, int y,
                        Point2D pa, Point2D pb, Point2D pc,
                        Vertex3D A, Vertex3D B, Vertex3D C,
                        Vertex3D Aos, Vertex3D Bos, Vertex3D Cos
                        )
{

    if (start_x == end_x) return;

    // forzamos de izquierda a derecha
    if (start_x > end_x) {
        int tmp = (int) start_x;

        start_x = end_x;
        end_x = tmp;
    }


    Uint32 pixelColor = EngineSetup::getInstance()->TRIANGLE_SOLID_COLOR;

    for (int x = (int) start_x; x < end_x; x++) {
        Point2D pointFinal(x, y);

        if (Tools::isPixelInWindow(pointFinal.x, pointFinal.y)) {

            // barycentric coordinates (from screen coordinates)
            // Hayamos las coordenadas baricéntricas del punto v4 respecto al triángulo pa, pb, pc
            float alpha, theta, gamma;
            Maths::getBarycentricCoordinates(alpha, theta, gamma, x, y, pa, pb, pc);

            float z = alpha * A.z + theta * B.z + gamma * C.z; // Homogeneous clipspace

            // EngineBuffers
            if (EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER) {
                float buffer_z = EngineBuffers::getInstance()->getDepthBuffer(pointFinal.x, pointFinal.y);

                if (buffer_z != NULL) {
                    if ( z < buffer_z ) {
                        EngineBuffers::getInstance()->setDepthBuffer(pointFinal.x, pointFinal.y, z);
                    } else {
                        continue;
                    }
                }  else {
                    EngineBuffers::getInstance()->setDepthBuffer(pointFinal.x, pointFinal.y, z);
                }
            }

            // Texture
            if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
                if (this->getTexture() != NULL) {
                    // UV texture coordinates
                    float u = alpha * A.u + theta * B.u + gamma * C.u;
                    float v = alpha * A.v + theta * B.v + gamma * C.v;

                    if (u < 0 || v < 0) continue;

                    // Check for repeat U coordinate
                    float ignorablePartInt;
                    if (u > 1) { u = modf(u , &ignorablePartInt); }
                    if (v > 1) { v = modf(v , &ignorablePartInt); }


                    pixelColor = Tools::readSurfacePixelFromUV(texture->texture_surface, u, v);
                    Uint8 red, green, blue, alpha;
                    SDL_GetRGBA(pixelColor, texture->texture_surface->format, &red, &green, &blue, &alpha);

                    if (alpha == 0) {
                        continue;
                    }
                }
            }

            if (EngineSetup::getInstance()->ENABLE_LIGHTS) {
                Vertex3D D;

                if (this->numberLightPoints > 0) {
                    // Coordenadas del punto que estamos procesando en el mundo (object space)
                    float x3d = alpha * Aos.x + theta * Bos.x + gamma * Cos.x;
                    float y3d = alpha * Aos.y + theta * Bos.y + gamma * Cos.y;
                    float z3d = alpha * Aos.z + theta * Bos.z + gamma * Cos.z;

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
                            Point2D DP = Transforms::screenSpace(Dl, this->lightPoints[i]->cam);

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

            EngineBuffers::getInstance()->setVideoBuffer( (int) pointFinal.x, (int) pointFinal.y, pixelColor);
        }
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
        Point2D pointFinal(x, y);

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

void Triangle::consoleInfo(const char *text)
{
    printf("%s Triangle consoleInfo: (%f, %f, %f) (u: %f, v: %f) | (%f, %f, %f) (u: %f, v: %f) | (%f, %f, %f) (u: %f, v: %f)\r\n",
           text,
           this->A.x, this->A.y, this->A.z, A.u, A.v,
           this->B.x, this->B.y, this->B.z, B.u, B.v,
           this->C.x, this->C.y, this->C.z, C.u, C.v
    );
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

