
#include "../../include/Objects/LightPoint3D.h"
#include "../../include/Render/Billboard.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

LightPoint3D::LightPoint3D(
    float power,
    float kc,
    float kl,
    float kq,
    float specularComponent,
    Color c,
    Color specularityColor
) :
    color(c),
    specularColor(specularityColor),
    kc(kc),
    kl(kl),
    kq(kq),
    p(power),
    specularComponent(specularComponent)
{
    frustum = new Frustum();
    frustum->setParent(this);
    frustum->setup(
        getPosition(),
        this->AxisForward(),
        this->AxisUp(),
        this->AxisRight(),
        180,
        ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
        100000
    );

    this->showDeepMapping = false;
    this->showFrustum = false;

    this->sizeBuffer = EngineSetup::get()->screenWidth * EngineSetup::get()->screenHeight;
    shadowMappingBuffer = new float[sizeBuffer];
}

void LightPoint3D::syncFrustum()
{
    frustum->setPosition( this->getPosition() );
    frustum->setRotation( this->getRotation() );

    frustum->direction = this->getRotation().getTranspose() * EngineSetup::get()->forward;
    frustum->up = this->getRotation().getTranspose() * EngineSetup::get()->up;
    frustum->right = this->getRotation().getTranspose() * EngineSetup::get()->right;

    frustum->updateFrustum();
}

void LightPoint3D::clearShadowMappingBuffer() const {
    std::fill(shadowMappingBuffer, shadowMappingBuffer + sizeBuffer, NULL);
}

float LightPoint3D::getShadowMappingBuffer(int x, int y) const {
    return shadowMappingBuffer[(y * EngineSetup::get()->screenWidth) + x];
}

void LightPoint3D::setShadowMappingBuffer(int x, int y, float value) const {
    shadowMappingBuffer[(y * EngineSetup::get()->screenWidth) + x] = value;
}

void LightPoint3D::setColor(Color value) {
    this->color = value;
}

void LightPoint3D::setColorSpecularity(Color value) {
    LightPoint3D::specularColor = value;
}

Color LightPoint3D::mixColor(Color colorTexture, Vertex3D Q, Fragment *fragment)
{
    const float distance = Maths::distanceBetweenVertices(this->getPosition(), Q);

    Vertex3D P = this->getPosition();
    Vertex3D R = this->AxisForward();

    Vector3D Light = Vector3D(Q, P);
    Vertex3D L = Light.normal();

    const float C = powf(fmaxf(R * L, 0), this->p) / (this->kc + this->kl * distance + this->kq * (distance * distance));

    Color diffuseColor;
    if (C >= 1) {
        diffuseColor = this->color;
    } else {
        diffuseColor = Color::mixColor(colorTexture, this->color, C);;
    }

    if (!EngineSetup::get()->ENABLE_LIGHTS_SPECULAR) {
        return diffuseColor;
    }

    // specular
    Vector3D Viewer = Vector3D(Q, ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());

    Vertex3D Ls = Light.getComponent().getInverse();
    Vertex3D V = Viewer.getComponent();
    Vertex3D N = Vertex3D(fragment->normalX, fragment->normalY, fragment->normalZ);
    Vertex3D H = this->getHalfWay(Vector3D(Q, P).getComponent() + V);

    float booleanCondition = 1;
    if (N * Ls > 0) {
        booleanCondition = 0;
    }

    const float K = powf(fmaxf(N * H, 0), this->specularComponent) * booleanCondition;

    Color specularColor = Color::mixColor(colorTexture, this->specularColor, K);

    return Color::mixColor(specularColor, diffuseColor, 0.5);
}

void LightPoint3D::onDrawHostBuffer()
{
    Object3D::onDrawHostBuffer();

    if (EngineSetup::get()->DRAW_LIGHTS_DIRECTION) {
        Drawable::drawLinePoints(
            getPosition(),
            getPosition() + AxisForward().getInverse().getNormalize().getScaled(EngineSetup::get()->LIGHTS_DIRECTION_SIZE),
            Color::white()
        );
    }
}

void LightPoint3D::onUpdate() {
    Object3D::onUpdate();
}

void LightPoint3D::setPower(float pNew) {
    this->p = pNew;
}

Vertex3D LightPoint3D::getHalfWay(Vertex3D v) {
    const float module  = v.getModule();

    return Vertex3D(
        v.x / module,
        v.y / module,
        v.z / module
    );
}

void LightPoint3D::setSpecularComponent(float m)
{
    this->specularComponent = m;
}

void LightPoint3D::drawDeepMap(int pos_x, int pos_y) const {
    int width = EngineSetup::get()->screenWidth;
    int height = EngineSetup::get()->screenHeight;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto pixelColor = getShadowMappingBuffer(j, i);
            EngineBuffers::get()->setVideoBuffer(j + pos_x, i + pos_y, Color(pixelColor, pixelColor, pixelColor).getColor());
        }
    }
}

void LightPoint3D::setConstant(float value) {
    this->kc = value;
}

void LightPoint3D::setLinear(float value) {
    this->kl = value;
}

void LightPoint3D::setCuadratic(float value) {
    this->kq = value;
}

const char *LightPoint3D::getTypeObject() {
    return "LightPoint3D";
}

const char *LightPoint3D::getTypeIcon() {
    return "lightIcon";
}

void LightPoint3D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("LightPoint")) {
        static ImVec4 imguiColor;
        imguiColor.x = color.r/255;
        imguiColor.y = color.g/255;
        imguiColor.z = color.b/255;
        static ImVec4 imguiColorSpecularity;
        imguiColorSpecularity.x = specularColor.r;
        imguiColorSpecularity.y = specularColor.g;
        imguiColorSpecularity.z = specularColor.b;

        bool changed_color_specular = false;
        std::string attenuation_text = "Attenuation##";
        std::string colorpicker_text = "RGB##";
        std::string colorpicker_text_specularity = "Specularity##";
        std::string show_deep_map_text = "Show ZBuffer##";
        std::string show_frustum_map_text = "Show Frustum##";
        std::string specularity_RGB_text = "RGB##";
        std::string color_text = "Color##";

        const float range_potence_min = 0;
        const float range_potence_max = 1000;
        const float range_potence_sensibility = 0.1f;

        const float range_min = -90000;
        const float range_max = 90000;

        if (EngineSetup::get()->CREATE_LIGHT_ZBUFFER) {
            ImGui::Checkbox(show_deep_map_text.c_str(), &showDeepMapping);
            ImGui::Checkbox(show_frustum_map_text.c_str(), &showFrustum);
        }

        // Color
        if (ImGui::TreeNode(color_text.c_str())) {
            bool changed_color = ImGui::ColorEdit4(colorpicker_text.c_str(), (float *) &imguiColor, ImGuiColorEditFlags_NoOptions);
            if (changed_color) {
                setColor(
                        Color(
                                (int) (imguiColor.x * 256),
                                (int) (imguiColor.y * 256),
                                (int) (imguiColor.z * 256)
                        )
                );
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(attenuation_text.c_str())) {
            ImGui::DragScalar("P", ImGuiDataType_Float, &p, range_potence_sensibility,&range_potence_min, &range_potence_max, "%f", 1.0f);
            ImGui::DragScalar("Constant", ImGuiDataType_Float, &kc, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Linear", ImGuiDataType_Float, &kl, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Quadratic", ImGuiDataType_Float, &kq, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(colorpicker_text_specularity.c_str())) {
            ImGui::DragScalar("m", ImGuiDataType_Float, &specularComponent, range_potence_sensibility,&range_potence_min, &range_potence_max, "%f", 1.0f);
            changed_color_specular = ImGui::ColorEdit4(specularity_RGB_text.c_str(), (float *) &imguiColorSpecularity, ImGuiColorEditFlags_NoOptions);
            if (changed_color_specular) {
                setColorSpecularity(
                        Color(
                                (int) (imguiColorSpecularity.x * 256),
                                (int) (imguiColorSpecularity.y * 256),
                                (int) (imguiColorSpecularity.z * 256)
                        )
                );
            }
            ImGui::TreePop();
        }
    }
}

cJSON *LightPoint3D::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON_AddNumberToObject(root, "kc", this->kc);
    cJSON_AddNumberToObject(root, "kl", this->kl);
    cJSON_AddNumberToObject(root, "kq", this->kq);
    cJSON_AddNumberToObject(root, "p", this->p);

    cJSON *colorJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorJSON, "r", color.r);
    cJSON_AddNumberToObject(colorJSON, "g", color.g);
    cJSON_AddNumberToObject(colorJSON, "b", color.b);
    cJSON_AddItemToObject(root, "color", colorJSON);

    cJSON *colorSpecularJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorSpecularJSON, "r", specularColor.r);
    cJSON_AddNumberToObject(colorSpecularJSON, "g", specularColor.g);
    cJSON_AddNumberToObject(colorSpecularJSON, "b", specularColor.b);
    cJSON_AddItemToObject(root, "specularColor", colorSpecularJSON);

    cJSON_AddNumberToObject(root, "specularComponent", this->specularComponent);

    return root;
}

void LightPoint3D::setPropertiesFromJSON(cJSON *object, LightPoint3D *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    o->setConstant((float)cJSON_GetObjectItemCaseSensitive(object, "kc")->valuedouble);
    o->setLinear((float)cJSON_GetObjectItemCaseSensitive(object, "kl")->valuedouble);
    o->setCuadratic((float)cJSON_GetObjectItemCaseSensitive(object, "kq")->valuedouble);
    o->setPower((float)cJSON_GetObjectItemCaseSensitive(object, "p")->valuedouble);

    auto color = cJSON_GetObjectItemCaseSensitive(object, "color");
    o->setColor(Color(
        cJSON_GetObjectItemCaseSensitive(color, "r")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "g")->valueint,
        cJSON_GetObjectItemCaseSensitive(color, "b")->valueint
    ));

    auto specularColor = cJSON_GetObjectItemCaseSensitive(object, "specularColor");
    o->setColorSpecularity(Color(
        cJSON_GetObjectItemCaseSensitive(specularColor, "r")->valueint,
        cJSON_GetObjectItemCaseSensitive(specularColor, "g")->valueint,
        cJSON_GetObjectItemCaseSensitive(specularColor, "b")->valueint
    ));

    o->setSpecularComponent((float) cJSON_GetObjectItemCaseSensitive(object, "specularComponent")->valuedouble);
}

void LightPoint3D::createFromJSON(cJSON *object)
{
    auto o = new LightPoint3D(10, 1, 0, 0, 0, Color::red(), Color::green());

    LightPoint3D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}