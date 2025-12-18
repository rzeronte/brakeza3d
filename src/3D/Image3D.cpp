#include "../../include/3D/Image3D.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image3DGUI.h"

Image3D::Image3D(const Vertex3D &position, float width, float height, const std::string &filePath)
:
    width(width),
    height(height),
    source(filePath)
{
    setPosition(position);
}

Image3D::Image3D(const Vertex3D &position, float width, float height, Image* image)
:
    width(width),
    height(height),
    image(image)
{
    if (image != nullptr) {
        source = image->getFileName();
    }
    setPosition(position);
}

void Image3D::ResetBuffersToSize(float width, float height)
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

    FillBuffers();
}

void Image3D::onUpdate()
{
    Object3D::onUpdate();
    if (!image->isLoaded() || !isEnabled()) return;

    if (towardsCamera) {
        LookAtBillboard();
    }

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();

    if (isGUISelected()) {
        render->getShaders()->shaderOGLOutline->drawOutlineImage3D(
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
                ShadowMappingPass();
            }
        } else {
            render->getShaders()->shaderOGLRender->render(
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
        render->getShaders()->shaderOGLWireframe->render(
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
        render->getShaders()->shaderOGLPoints->render(
            getModelMatrix(),
            vertexBuffer,
            (int) vertices.size(),
            Color::green(),
            window->getSceneFramebuffer()
        );
    }

    if (Config::get()->TRIANGLE_MODE_SHADING) {
        render->getShaders()->shaderOGLShading->render(
            getModelMatrix(),
            vertexBuffer,
            uvBuffer,
            normalBuffer,
            false,
            window->getSceneFramebuffer()
        );
    }

    if (Config::get()->MOUSE_CLICK_SELECT_OBJECT3D)  {
        render->getShaders()->shaderOGLColor->RenderColor(
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

void Image3D::FillBuffers()
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

void Image3D::ShadowMappingPass()
{
    auto render = Components::get()->Render();
    auto shaderShadowPass = render->getShaders()->shaderShadowPass;
    auto shaderRender = render->getShaders()->shaderOGLRender;

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

void Image3D::LookAtBillboard()
{
    auto o = Components::get()->Camera()->getCamera();

    // Dirección de la imagen hacia la cámara
    auto direction = (o->getPosition() - position).getNormalize();

    // Proyectar la dirección en el plano horizontal (XY)
    auto forward = Vertex3D(direction.x, direction.y, 0).getNormalize();

    // Vector arriba fijo (siempre apuntando en Z positivo)
    auto upVector = Vertex3D(0, 1, 0);

    // Vector derecha (perpendicular a forward y up)
    Vertex3D rightVector = forward % upVector;

    // Establecer la rotación con los vectores corregidos
    setRotation(M3::getFromVectors(forward, upVector));
}

void Image3D::setWidth(float value)
{
    width = value;
}

void Image3D::setSource(const std::string &source)
{
    this->source = source;
}

void Image3D::setHeight(float value)
{
    height = value;
}

void Image3D::setImage(Image* value)
{
    image = value;
}

Image3D::~Image3D()
{
}
