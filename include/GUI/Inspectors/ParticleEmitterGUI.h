//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_PARTICLEEMITTERGUI_H
#define BRAKEZA3D_PARTICLEEMITTERGUI_H

#include "../../3D/ParticleEmitter.h"
#include "../../Brakeza.h"
#include "../../Misc/Logging.h"
#include "../Widgets/GUIFields.h"
#include "../Widgets/GUIHelpers.h"

class ParticleEmitterGUI
{
public:
    static void DrawPropertiesGUI(ParticleEmitter *o)
    {
        if (ImGui::CollapsingHeader("ParticleEmitter")) {

            if (ImGui::TreeNode("Image")) {
                if (o->texture != nullptr) {
                    ImGui::Image(o->texture->getOGLImTexture(),ImVec2(32, 32));
                } else {
                    ImGui::Text("Empty texture. Drag one here!");
                }

                GUIHelpers::ImageDropTarget([&](const char *selection) {
                    delete o->texture;
                    o->texture = new Image(std::string(selection));
                    LOG_MESSAGE("File %s", selection);
                });
                ImGui::TreePop();
            }
            ImGui::Separator();

            // Propiedades en tablas de 2 columnas (etiqueta de ancho fijo + control que llena el resto),
            // agrupadas por secciones; pares mín/máx en una sola fila; ayuda al pasar sobre la etiqueta.
            auto& c = o->context;
            using namespace GUIFields;

            if (BeginSection("EMISSION")) {
                bool emitting = !o->stopAdd;
                if (Check("Emitting", &emitting, "Unchecked = stops spawning new particles (the alive ones finish their life).")) o->stopAdd = !emitting;
                Float("Rate", &c.PARTICLES_BY_SECOND, 1.0f, 0.0f, 5000.0f, "%.1f /s", "Particles spawned per second (decimals allowed: 0.5 = one every 2 s).");
                Float("Lifespan", &c.PARTICLE_LIFESPAN, 0.01f, 0.0f, 100.0f, "%.2f s", "Seconds each particle lives. Color, opacity and size interpolate over this time.");
                Check("Depth test", &o->depthTest, "On: scene geometry (buildings...) hides the particles behind it. Off: drawn on top of everything.");
                EndSection();
            }

            if (BeginSection("MOVEMENT")) {
                Int("Spread angle", &c.SMOKE_ANGLE_RANGE, 1.0f, 0, 360, "%d deg", "Random deviation from the emitter's UP direction (0 = straight line, 360 = all directions).");
                FloatRange("Speed", &c.MIN_VELOCITY, &c.MAX_VELOCITY, 0.05f, 0.0f, 200.0f, "min %.2f u/s", "max %.2f u/s", "Launch speed, units/second. Each particle picks a value between min and max.");
                Float("Gravity", &c.GRAVITY, 0.01f, -100.0f, 100.0f, "%.2f u/s2", "Vertical acceleration, u/s2. Positive = rises (smoke, fire), negative = falls (sparks, debris).");
                Float("Deceleration", &c.DECELERATION_FACTOR, 0.001f, 0.0f, 1.0f, "%.3f", "Speed kept per frame at 60 fps (1 = no braking, 0.9 = brakes fast). Frame-rate independent.");
                Float("Direction noise", &c.VELOCITY_NOISE, 0.02f, 0.0f, 90.0f, "%.2f deg", "Random turn of the direction each frame (degrees at 60 fps). Makes paths wobble; keeps the speed.");
                Float("Spawn area", &c.POSITION_NOISE, 0.02f, 0.0f, 100.0f, "%.2f u", "Particles are born scattered in a cube of this size (units) around the emitter.");
                EndSection();
            }

            if (BeginSection("LOOK")) {
                IntRange("Opacity at birth", &c.MIN_ALPHA, &c.MAX_ALPHA, 1.0f, 0, 255, "min %d", "max %d", "Opacity when born (0-255), random between min and max. Fades to 0 at the end of its life.");
                Float("Size", &c.PARTICLE_SIZE_SCALE, 0.05f, 0.01f, 100.0f, "x %.2f", "Base size multiplier of every particle (base ~0.1-0.6 u).");
                float sz[2] = { c.SIZE_START, c.SIZE_END };
                if (Float2("Size over life", sz, 0.01f, 0.0f, 20.0f, "%.2f", "Size multiplier at birth -> at death (1 / 1 = constant). Start < end = grows (smoke), start > end = shrinks (sparks).")) {
                    c.SIZE_START = sz[0]; c.SIZE_END = sz[1];
                }
                EndSection();
            }

            if (BeginSection("ROTATION")) {
                Check("Random start", &c.ROTATION_RANDOM_START, "Each particle is born rotated at a random angle (breaks the 'stamp' look of smoke).");
                FloatRange("Spin speed", &c.ROTATION_SPEED_MIN, &c.ROTATION_SPEED_MAX, 0.25f, -1440.0f, 1440.0f, "min %.0f deg/s", "max %.0f deg/s", "Rotation speed in degrees/second, random between min and max (0 / 0 = no spin).");
                Check("Random direction", &c.ROTATION_RANDOM_DIR, "Each particle spins clockwise or counter-clockwise at random.");
                EndSection();
            }

            if (BeginSection("SHAPE")) {
                static const char *shapeNames[] = { "Point", "Box", "Sphere", "Cone", "Cylinder", "Torus", "Mesh" };
                int shape = static_cast<int>(c.SHAPE);
                if (Combo("Shape", &shape, shapeNames, IM_ARRAYSIZE(shapeNames),
                          "Where the particles are born. Point = a single point (original behaviour). The shape's axis is the emitter's local up.")) {
                    c.SHAPE = static_cast<EmitterShape>(shape);
                }

                switch (c.SHAPE) {
                    case EmitterShape::BOX: {
                        float sz[3] = { c.SHAPE_SIZE_X, c.SHAPE_SIZE_Y, c.SHAPE_SIZE_Z };
                        if (Float3("Size", sz, 0.05f, 0.0f, 1000.0f, "%.2f", "Full side lengths of the box (units), centred on the emitter.")) {
                            c.SHAPE_SIZE_X = sz[0]; c.SHAPE_SIZE_Y = sz[1]; c.SHAPE_SIZE_Z = sz[2];
                        }
                        break;
                    }
                    case EmitterShape::SPHERE:
                        Float("Radius", &c.SHAPE_RADIUS, 0.05f, 0.0f, 1000.0f, "%.2f u", "Sphere radius (units).");
                        break;
                    case EmitterShape::CONE:
                        Float("Base radius", &c.SHAPE_RADIUS, 0.05f, 0.0f, 1000.0f, "%.2f u", "Radius of the cone at its far end (units). The apex is at the emitter and it opens towards its up axis.");
                        Float("Height", &c.SHAPE_HEIGHT, 0.05f, 0.01f, 1000.0f, "%.2f u", "Distance from the apex to the base (units).");
                        break;
                    case EmitterShape::CYLINDER:
                        Float("Radius", &c.SHAPE_RADIUS, 0.05f, 0.0f, 1000.0f, "%.2f u", "Cylinder radius (units).");
                        Float("Height", &c.SHAPE_HEIGHT, 0.05f, 0.0f, 1000.0f, "%.2f u", "Cylinder height (units), centred on the emitter.");
                        break;
                    case EmitterShape::TORUS:
                        Float("Ring radius", &c.SHAPE_RADIUS, 0.05f, 0.0f, 1000.0f, "%.2f u", "Distance from the centre to the middle of the tube (units). The ring lies in the emitter's XZ plane.");
                        Float("Tube radius", &c.SHAPE_TUBE_RADIUS, 0.02f, 0.0f, 1000.0f, "%.2f u", "Radius of the tube (units).");
                        break;
                    case EmitterShape::MESH: {
                        Row("Mesh", "Scene mesh whose surface emits the particles (it follows the mesh when it moves). Static geometry; skinned animation is not followed.");
                        if (ImGui::BeginCombo("##v", c.SHAPE_MESH.empty() ? "(none)" : c.SHAPE_MESH.c_str())) {
                            for (auto *so : Brakeza::get()->copySceneObjects()) {
                                if (so->getTypeObject() != ObjectType::Mesh3D || so->isRemoved()) continue;
                                const bool selected = so->getName() == c.SHAPE_MESH;
                                if (ImGui::Selectable(so->getName().c_str(), selected)) c.SHAPE_MESH = so->getName();
                            }
                            ImGui::EndCombo();
                        }
                        EndRow();
                        break;
                    }
                    default:
                        break;
                }

                if (c.SHAPE != EmitterShape::POINT && c.SHAPE != EmitterShape::MESH) {
                    Check("Surface only", &c.SHAPE_SURFACE_ONLY, "On: born on the surface of the shape. Off: born anywhere inside its volume.");
                }

                static const char *dirNames[] = { "Along the emitter axis", "Outward from the shape" };
                int dirMode = static_cast<int>(c.SHAPE_DIRECTION);
                if (Combo("Direction", &dirMode, dirNames, IM_ARRAYSIZE(dirNames),
                          "Along the axis = original behaviour. Outward = away from the shape's surface (a sphere bursts, a ring spreads). Point + Outward = random direction.")) {
                    c.SHAPE_DIRECTION = static_cast<EmitterShapeDirection>(dirMode);
                }
                EndSection();
            }

            if (BeginSection("SPIRAL / VORTEX")) {
                Float("Swirl", &c.SWIRL_SPEED, 1.0f, -3600.0f, 3600.0f, "%.0f deg/s", "Particles orbit the emitter's axis (its local up), in degrees/second. Sign = direction. Needs some distance from the axis (cone angle, spawn area or radial speed) to draw a curve.");
                Float("Pull to axis", &c.SWIRL_PULL, 0.01f, 0.0f, 100.0f, "%.2f", "Spring towards the axis, proportional to the distance (1/s²). With swirl it makes tight helices; 0 = none.");
                Float("Radial speed", &c.SPIRAL_RADIAL_SPEED, 0.05f, -50.0f, 50.0f, "%.2f u/s", "Extra launch speed away from the axis (units/second), in the direction chosen by the arms below.");
                Int("Arms", &c.SPIRAL_ARMS, 0.1f, 0, 16, "%d", "0 = launch in a random direction around the axis (ring/disc). N = N evenly spaced arms.");
                Float("Arms spin", &c.SPIRAL_SPIN, 1.0f, -3600.0f, 3600.0f, "%.0f deg/s", "How fast the launch direction rotates around the axis (deg/s). Arms + spin = spiral galaxy / sprinkler.");
                EndSection();
            }

            if (BeginSection("COLORS")) {
                float from[3] = { o->colorFrom.r, o->colorFrom.g, o->colorFrom.b };
                if (Color3("At birth", from, "Color when the particle is born.")) o->setColorFrom(Color(from[0], from[1], from[2]));
                float to[3] = { o->colorTo.r, o->colorTo.g, o->colorTo.b };
                if (Color3("At death", to, "Color it fades to at the end of its life.")) o->setColorTo(Color(to[0], to[1], to[2]));
                EndSection();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("Attached Light")) {
                if (o->attachedLight != nullptr) {
                    ImGui::Text("Light: %s", o->attachedLight->getName().c_str());

                    ImVec4 color = {o->attachedLight->diffuse.r, o->attachedLight->diffuse.g, o->attachedLight->diffuse.b, 1};
                    bool changed = ImGui::ColorEdit4("Diffuse##", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    if (changed) {
                        o->attachedLight->diffuse = glm::vec4(color.x, color.y, color.z, 0);
                    }

                    color = {o->attachedLight->ambient.r, o->attachedLight->ambient.g, o->attachedLight->ambient.b, 1};
                    changed = ImGui::ColorEdit4("Ambient##", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    if (changed) {
                        o->attachedLight->ambient = glm::vec4(color.x, color.y, color.z, 0);
                    }

                    color = {o->attachedLight->specular.r, o->attachedLight->specular.g, o->attachedLight->specular.b, 1};
                    changed = ImGui::ColorEdit4("Specular##", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    if (changed) {
                        o->attachedLight->specular = glm::vec4(color.x, color.y, color.z, 0);
                    }

                    ImGui::DragFloat("Constant", &o->attachedLight->constant, 0.01f, 0.0f, 10.0f, "%.4f");
                    ImGui::DragFloat("Linear", &o->attachedLight->linear, 0.001f, 0.0f, 10.0f, "%.4f");
                    ImGui::DragFloat("Quadratic", &o->attachedLight->quadratic, 0.001f, 0.0f, 10.0f, "%.4f");

                    if (ImGui::Button("Remove Light")) {
                        o->removeAttachedLight();
                    }
                } else {
                    ImGui::Text("No attached light.");

                    static float range = 15.0f;
                    static ImVec4 color = {1, 0, 0, 1};
                    ImGui::ColorEdit4("Color##new", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    ImGui::DragFloat("Range", &range, 0.5f, 1.0f, 100.0f, "%.1f");

                    if (ImGui::Button("Create Light")) {
                        o->createAttachedLight(
                            Color(color.x, color.y, color.z),
                            Color(color.x * 0.05f, color.y * 0.05f, color.z * 0.05f),
                            Color(color.x, color.y, color.z),
                            range
                        );
                    }
                }
                ImGui::TreePop();
            }
        }
    }
};

#endif //BRAKEZA3D_PARTICLEEMITTERGUI_H