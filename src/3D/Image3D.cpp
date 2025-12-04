#include "../../include/3D/Image3D.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image3DGUI.h"

Image3D::Image3D(const Vertex3D &position, float width, float height, Image* image)
:
    width(width),
    height(height),
    image(image)
{
    setPosition(position);
    setSize(width, height);
}

void Image3D::setSize(float width, float height)
{
    setWidth(width);
    setHeight(height);

    vertices.clear(); uvs.clear(); normals.clear();

    float hw = width / 2.0f;
    float hh = height / 2.0f;

    Q1.x =  hw; Q1.y =  hh; Q1.z = 0.0f; // top-right
    Q2.x = -hw; Q2.y =  hh; Q2.z = 0.0f; // top-left
    Q3.x = -hw; Q3.y = -hh; Q3.z = 0.0f; // bottom-left
    Q4.x =  hw; Q4.y = -hh; Q4.z = 0.0f; // bottom-right

    Q1.u = 1.0f; Q1.v = 1.0f;
    Q2.u = 0.0f; Q2.v = 1.0f;
    Q3.u = 0.0f; Q3.v = 0.0f;
    Q4.u = 1.0f; Q4.v = 0.0f;

    // Triangle 1: Q1 -> Q2 -> Q3
    vertices.emplace_back(Q1.x, Q1.y, Q1.z, 1.0f);
    vertices.emplace_back(Q2.x, Q2.y, Q2.z, 1.0f);
    vertices.emplace_back(Q3.x, Q3.y, Q3.z, 1.0f);

    uvs.emplace_back(Q1.u, Q1.v);
    uvs.emplace_back(Q2.u, Q2.v);
    uvs.emplace_back(Q3.u, Q3.v);

    // Triangle 2: Q1 -> Q3 -> Q4
    vertices.emplace_back(Q1.x, Q1.y, Q1.z, 1.0f);
    vertices.emplace_back(Q3.x, Q3.y, Q3.z, 1.0f);
    vertices.emplace_back(Q4.x, Q4.y, Q4.z, 1.0f);

    uvs.emplace_back(Q1.u, Q1.v);
    uvs.emplace_back(Q3.u, Q3.v);
    uvs.emplace_back(Q4.u, Q4.v);

    for (int i = 0; i < 6; ++i)
        normals.emplace_back(0.0f, 0.0f, 1.0f);

    fillBuffers();
}

void Image3D::onUpdate()
{
    Object3D::onUpdate();
    if (!image->isLoaded() || !isEnabled()) return;

    if (towardsCamera) {
        lookAtBillboard(ComponentsManager::get()->getComponentCamera()->getCamera());
    }

    auto render = ComponentsManager::get()->getComponentRender();
    auto window = ComponentsManager::get()->getComponentWindow();

    if (isGUISelected()) {
        render->getShaderOGLOutline()->drawOutlineImage3D(
            this,
            Color::green(),
            0.1f,
            window->getUIFramebuffer()
        );
    }

    if (!backFaceCulling)
        glDisable(GL_CULL_FACE);


    if (Config::get()->TRIANGLE_MODE_TEXTURIZED) {
        if (Config::get()->ENABLE_LIGHTS) {
            render->getShaderOGLRenderDeferred()->render(
                this,
                image->getOGLTextureID(),
                image->getOGLTextureID(),
                vertexBuffer,
                uvBuffer,
                normalBuffer,
                static_cast<int>(vertices.size()),
                alpha,
                window->getGBuffer().FBO
            );
            if (Config::get()->ENABLE_SHADOW_MAPPING) {
                shadowMappingPass();
            }
        } else {
            render->getShaderOGLRenderForward()->render(
                this,
                image->getOGLTextureID(),
                image->getOGLTextureID(),
                vertexBuffer,
                uvBuffer,
                normalBuffer,
                static_cast<int>(vertices.size()),
                alpha,
                window->getGBuffer().FBO
            );
        }
    }

    if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
        render->getShaderOGLWireframe()->render(
            getModelMatrix(),
            vertexBuffer,
            uvBuffer,
            normalBuffer,
            static_cast<int>(vertices.size()),
            Color::gray(),
            window->getSceneFramebuffer()
        );
    }

    if (Config::get()->TRIANGLE_MODE_PIXELS) {
        render->getShaderOGLPoints()->render(
            getModelMatrix(),
            vertexBuffer,
            vertices.size(),
            Color::green(),
            window->getSceneFramebuffer()
        );
    }

    if (Config::get()->TRIANGLE_MODE_SHADING) {
        render->getShaderOGLShading()->render(
            getModelMatrix(),
            vertexBuffer,
            uvBuffer,
            normalBuffer,
            false,
            window->getSceneFramebuffer()
        );
    }

    if (Config::get()->MOUSE_CLICK_SELECT_OBJECT3D)  {
        render->getShaderOGLColor()->renderColor(
            getModelMatrix(),
            vertexBuffer,
            uvBuffer,
            normalBuffer,
            static_cast<int>(vertices.size()),
            getPickingColor(),
            false,
            window->getPickingColorFramebuffer().FBO
        );
    }

    glEnable(GL_CULL_FACE);
}

void Image3D::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image3DGUI::DrawPropertiesGUI(this);
}

Image3D::~Image3D()
{
}

ObjectTypes Image3D::getTypeObject() const
{
    return ObjectTypes::Image3D;
}

GUISheet Image3D::getIcon()
{
    return IconObject::IMAGE_3D;
}

void Image3D::setWidth(float value)
{
    width = value;
}

void Image3D::setHeight(float value)
{
    height = value;
}

void Image3D::fillBuffers()
{
    GLuint buffersToDelete[3] = {vertexBuffer, uvBuffer, normalBuffer};
    glDeleteBuffers(3, buffersToDelete);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(vertices.size() * sizeof(glm::vec4)), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(uvs.size() * sizeof(glm::vec2)), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(normals.size() * sizeof(glm::vec3)), &normals[0], GL_STATIC_DRAW);
}

Image3D *Image3D::create(const Vertex3D &p, float w, float h, const std::string &file)
{
    return new Image3D(p, w, h, new Image(file));
}

GLuint Image3D::getVertexBuffer() const
{
    return vertexBuffer;
}

GLuint Image3D::getNormalBuffer() const
{
    return normalBuffer;
}

GLuint Image3D::getUVBuffer() const
{
    return uvBuffer;
}

std::vector<glm::vec4> Image3D::getVertices() const
{
    return vertices;
}

Image* Image3D::getImage() const
{
    return image;
}

void Image3D::setImage(Image* value)
{
    image = value;
}

void Image3D::shadowMappingPass()
{
    auto render = ComponentsManager::get()->getComponentRender();
    auto shaderShadowPass = render->getShaderOGLShadowPass();
    auto shaderRender = render->getShaderOGLRenderForward();

    // Directional Light
    shaderShadowPass->renderIntoDirectionalLightTexture(
        this,
        shaderRender->getDirectionalLight(),
        vertexBuffer,
        uvBuffer,
        normalBuffer,
        static_cast<int>(vertices.size()),
        shaderShadowPass->getDirectionalLightDepthMapFBO()
    );

    // SpotLights
    const auto shadowSpotLights = shaderRender->getShadowMappingSpotLights();
    const auto numSpotLights = static_cast<int>(shadowSpotLights.size());

    for (int i = 0; i < numSpotLights; i++) {
        const auto l = shadowSpotLights[i];
        shaderShadowPass->renderIntoArrayDepthTextures(
            this,
            l,
            vertexBuffer,
            uvBuffer,
            normalBuffer,
            static_cast<int>(vertices.size()),
            shaderShadowPass->getSpotLightsDepthMapsFBO(),
            i,
            shaderShadowPass->getSpotLightsDepthMapsFBO()
        );
    }
}

void Image3D::lookAtBillboard(Object3D *o)
{
    Vertex3D direction = (o->getPosition() - position).getNormalize();

    Vertex3D horizontalDirection = Vertex3D(direction.x, direction.y, 0).getNormalize();
    Vertex3D upVector = Vertex3D(0, 0, 1);
    Vertex3D rightVector = upVector % horizontalDirection;

    Vertex3D correctedForward = rightVector % upVector;

    setRotation(M3::getFromVectors(correctedForward, upVector));
}