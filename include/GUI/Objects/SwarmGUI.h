//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_SWARMGUI_H
#define BRAKEZA3D_SWARMGUI_H

#include <functional>
#include "../../3D/Swarm.h"
#include "../../Brakeza.h"

class SwarmGUI
{
public:
    static void DrawPropertiesGUI(Swarm *o)
    {
        if (ImGui::CollapsingHeader("Swarm")) {
            const float s = 0.01f;
            const float fmin = 0.0f;
            const float fmax = 100.0f;

            float vec3f[3];
            o->size.toFloat(vec3f);
            if (ImGui::DragFloat3("Size", vec3f, s, fmin, fmax)) {
                o->size.x = vec3f[0];
                o->size.y = vec3f[1];
                o->size.z = vec3f[2];
            }
            ImGui::Checkbox("Debug AABB", &o->debug);
            ImGui::Separator();

            if (ImGui::TreeNode("Boids weights")) {
                ImGui::DragFloat("Separation",  &o->separationWeight,    s, fmin, fmax);
                ImGui::DragFloat("Alignment",   &o->alignmentWeight,     s, fmin, fmax);
                ImGui::DragFloat("Cohesion",    &o->cohesionWeight,      s, fmin, fmax);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Movement")) {
                ImGui::DragFloat("Neighbor dist",       &o->neighborDist,           s, fmin, fmax);
                ImGui::DragFloat("Max speed",           &o->maxSpeed,               s, fmin, fmax);
                ImGui::DragFloat("Velocity factor",     &o->velocityBoidsFactor,    s, fmin, fmax);
                ImGui::DragFloat("Turn factor",         &o->turnFactor,             s, fmin, fmax);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Center attraction")) {
                ImGui::DragFloat("Threshold",   &o->centerThreshold,        s, fmin, fmax);
                ImGui::DragFloat("Weight",      &o->centerAttractionWeight, s, fmin, fmax);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Predators")) {
                ImGui::DragFloat("Threshold",       &o->predatorThreshold,       s, fmin, fmax);
                ImGui::DragFloat("Avoidance weight",&o->predatorAvoidanceWeight, s, fmin, fmax);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Global bias")) {
                ImGui::DragFloat("Separation",  &o->globalBiasSeparation,   s, fmin, fmax);
                ImGui::DragFloat("Alignment",   &o->globalBiasAlignment,    s, fmin, fmax);
                ImGui::DragFloat("Cohesion",    &o->globalBiasCohesion,     s, fmin, fmax);
                ImGui::TreePop();
            }
        }

        DrawMembersGUI(o);
    }

    static void DrawMembersGUI(Swarm *o)
    {
        if (!ImGui::CollapsingHeader("Swarm Members")) return;

        auto& sceneObjects = Brakeza::get()->getSceneObjects();

        auto isAlreadyMember = [&](Object3D* candidate) {
            if (candidate == o) return true;
            for (auto* sw : o->objects)
                if (sw->object == candidate) return true;
            for (auto* sw : o->predators)
                if (sw->object == candidate) return true;
            return false;
        };

        bool hasPending = !o->pendingBoidNames.empty() || !o->pendingPredatorNames.empty();
        if (hasPending) {
            ImGui::TextColored(ImVec4(1.f, 0.8f, 0.2f, 1.f), "Pending: %d boids, %d predators",
                (int)o->pendingBoidNames.size(), (int)o->pendingPredatorNames.size());
            ImGui::SameLine();
        }
        if (ImGui::Button("Load members")) {
            o->resolvePendingMembers();
        }
        ImGui::Separator();

        DrawMemberSection(o, sceneObjects, isAlreadyMember, false);
        ImGui::Spacing();
        DrawMemberSection(o, sceneObjects, isAlreadyMember, true);
    }

    static void DrawMemberSection(
        Swarm* o,
        const std::vector<Object3D*>& sceneObjects,
        const std::function<bool(Object3D*)>& isAlreadyMember,
        bool isPredator)
    {
        auto& list = isPredator ? o->predators : o->objects;
        const char* sectionLabel = isPredator ? "Predators" : "Boids";
        const char* newLabel     = isPredator ? "New predator" : "New boid";
        const char* objPrefix    = isPredator ? "Predator"     : "Boid";

        std::string header = std::string(sectionLabel) + " (" + std::to_string(list.size()) + ")";
        if (!ImGui::TreeNode(header.c_str())) return;

        // New button
        if (ImGui::SmallButton(newLabel)) {
            auto* newObj = new Object3D();
            newObj->setName(Brakeza::UniqueObjectLabel(objPrefix));
            newObj->setPosition(o->randomVertexInsideAABB());
            Brakeza::get()->AddObject3D(newObj, newObj->getName());
            if (isPredator) {
                auto* sw = new SwarmObject(newObj);
                o->addPredator(sw);
            } else {
                o->createBoid(newObj);
            }
        }

        ImGui::Spacing();

        // Table
        SwarmObject* toRemove = nullptr;
        const ImGuiTableFlags tflags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg   |
            ImGuiTableFlags_SizingStretchProp;

        if (!list.empty() && ImGui::BeginTable(isPredator ? "tbl_pred" : "tbl_boid", 2, tflags)) {
            ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 54.0f);
            ImGui::TableHeadersRow();

            for (auto* sw : list) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(sw->object->getName().c_str());
                ImGui::TableSetColumnIndex(1);
                std::string rid = "Remove##" + std::string(isPredator ? "p" : "b") + sw->object->getName();
                if (ImGui::SmallButton(rid.c_str()))
                    toRemove = sw;
            }
            ImGui::EndTable();
        } else if (list.empty()) {
            ImGui::TextDisabled("(empty)");
        }

        if (toRemove) {
            if (isPredator) o->removePredator(toRemove);
            else            o->removeBoid(toRemove);
        }

        // Assign from scene
        ImGui::Spacing();
        ImGui::TextDisabled("Assign from scene:");
        DrawAddMemberCombo(o, sceneObjects, isAlreadyMember, isPredator);

        ImGui::TreePop();
    }

    static void DrawAddMemberCombo(
        Swarm* o,
        const std::vector<Object3D*>& sceneObjects,
        const std::function<bool(Object3D*)>& isAlreadyMember,
        bool asPredator)
    {
        std::vector<Object3D*> candidates;
        for (auto* obj : sceneObjects) {
            if (!obj->isRemoved() && !isAlreadyMember(obj))
                candidates.push_back(obj);
        }

        static int selectedBoid = 0;
        static int selectedPred = 0;
        int& selected = asPredator ? selectedPred : selectedBoid;

        if (candidates.empty()) {
            ImGui::TextDisabled("(no free scene objects)");
            return;
        }

        if (selected >= (int)candidates.size()) selected = 0;

        const char* comboId = asPredator ? "##predcombo" : "##boidcombo";
        const char* btnId   = asPredator ? "Assign##pred" : "Assign##boid";

        float availW   = ImGui::GetContentRegionAvail().x;
        float btnW     = ImGui::CalcTextSize("Assign").x + ImGui::GetStyle().FramePadding.x * 2.0f + 8.0f;
        float comboW   = availW - btnW - ImGui::GetStyle().ItemSpacing.x;

        ImGui::SetNextItemWidth(comboW);
        if (ImGui::BeginCombo(comboId, candidates[selected]->getName().c_str())) {
            for (int i = 0; i < (int)candidates.size(); i++) {
                bool isSel = (selected == i);
                if (ImGui::Selectable(candidates[i]->getName().c_str(), isSel))
                    selected = i;
                if (isSel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button(btnId)) {
            auto* sw = new SwarmObject(candidates[selected]);
            sw->velocity = Vertex3D::randomVertex().getNormalize();
            if (asPredator) o->predators.emplace_back(sw);
            else            o->objects.emplace_back(sw);
            selected = 0;
        }
    }

};

#endif //BRAKEZA3D_SWARMGUI_H