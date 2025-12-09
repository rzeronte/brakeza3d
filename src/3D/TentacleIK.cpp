//
// Created by Eduardo on 09/09/2023.
//

#include "../../include/3D/TentacleIK.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Transforms.h"
#include <random>  // Para generación de números aleatorios

#include "../../include/Misc/ToolsMaths.h"

TentacleIK::TentacleIK(Vertex3D position, Object3D *parent, Object3D * target, float maxLength)
:
    target(target->getPosition()),
    velocity(Vertex3D()),
    maxLength(maxLength),
    cadence(Counter(0.03f))
{
    setParent(parent);
    setPosition(position);
}

void TentacleIK::onUpdate()
{
    Object3D::onUpdate();
}

void TentacleIK::postUpdate()
{
    Object3D::postUpdate();

    cadence.update();

    IKForwardSolver();
}

void TentacleIK::IKForwardSolver()
{
    if (joints.empty()) return;

    setRootPosition(parent->getPosition());
    moveFinalToTargetPosition();
    transformJoints();

    if ((target - joints.front()->startWorld).getModule() > maxLength) {
        fadeOut();
        hide();
    } else {
        fadeIn();
        show();
    }

    if (cadence.isFinished()) cadence.setEnabled(true);

    draw();
    drawTargetPosition();
}

void TentacleIK::drawTargetPosition()
{
    Drawable::drawVertex(target, Color::green());
}

void TentacleIK::addJoint(TentacleSegment *segment)
{
    segment->endWorld = segment->end;
    segment->startWorld = segment->start;

    joints.push_back(segment);
}

void TentacleIK::updateRotations()
{
    int size = (int)joints.size();

    auto accumulatedRotation = M3::getMatrixIdentity();
    auto accumulatedPosition = joints[0]->start;

    for (int i = 0; i < size; i++) {
        auto originalVector = joints[i]->end - joints[i]->start;
        accumulatedRotation =  accumulatedRotation * joints[i]->rotation;

        auto newEnd = accumulatedPosition + (accumulatedRotation * originalVector);

        joints[i]->startWorld = accumulatedPosition;
        joints[i]->endWorld = newEnd;

        accumulatedPosition = newEnd;
    }
}

void TentacleIK::transformJoints()
{
    for (int i = (int) joints.size() -1 ; i >= 0 ; i--) {
        transformJoint(i);
        applySinusoidalMovement(Config::get()->TESTING_INT3, Config::get()->TESTING_INT2, Brakeza::get()->getExecutionTime());
    }
}

void TentacleIK::transformJoint(int i)
{
    Vertex3D toEnd = joints.back()->endWorld - joints[i]->startWorld;
    Vertex3D toTarget = target - joints[i]->startWorld;

    const float toEndModule = toEnd.getModule();
    const float toTargetModule = toTarget.getModule();

    const float factor = toEndModule * toTargetModule;
    const float cosTheta = (toEnd * toTarget) / factor;
    float angle = acos(std::clamp(cosTheta, -1.0f, 1.0f));

    Vertex3D rotationAxis = (toEnd % toTarget).getNormalize();

    if (!Tools::isValidVector(rotationAxis)) {
        rotationAxis = Vertex3D(0, 0, 0);
    }

    angle = std::clamp(angle, ToolsMaths::DegreesToRadians(0.1), ToolsMaths::DegreesToRadians(1));

    joints[i]->rotation = M3::arbitraryAxis(rotationAxis, angle) * joints[i]->rotation;

    updateRotations();
}

void TentacleIK::moveFinalToTargetPosition()
{
    Vertex3D v = followPosition - target;
    target = target + v.getScaled(0.02);
}

void TentacleIK::moveFinalToRandomNear()
{
    Vertex3D toBase = joints.front()->startWorld - target;
    target = target + toBase.getScaled(0.3);
}

void TentacleIK::setTarget(Object3D *o)
{
    followPosition = o->getPosition();
}

void TentacleIK::draw()
{
    auto camera = Components::get()->Camera()->getCamera();

    int cont = 0;
    for (auto & joint : joints) {
        if (!joint->active) continue;

        Point2D startScreenPoint = Transforms::WorldToPoint(joint->startWorld);
        Point2D endScreenPoint = Transforms::WorldToPoint(joint->endWorld);

        const float intensity = joint->intensity * Config::get()->TESTING_INT1;

        /*shaderLasers->addLaser(
            startScreenPoint.x, startScreenPoint.y,
            endScreenPoint.x, endScreenPoint.y,
            PaletteColors::getStamina(),
            intensity,
            false,
            false
        );*/
        cont++;
    }
}

void TentacleIK::hide()
{
    if (!cadence.isFinished()) return;

    for (int i = (int) joints.size() - 1; i > 5 ; i--) {
        if (joints[i]->active) {
            joints[i]->active = false;
            return;
        }
    }
}

void TentacleIK::show()
{
    if (!cadence.isFinished()) return;

    for (int i = 0; i < (int) joints.size(); i++) {
        if (!joints[i]->active) {
            joints[i]->active = true;
            return;
        }
    }
}


void TentacleIK::fadeIn()
{
    for (int i = (int) joints.size() - 1; i >= 0; i--) {
        joints[i]->intensity *= 1.3f;
        if (joints[i]->intensity > 1) joints[i]->intensity = 1;
    }
}

void TentacleIK::fadeOut()
{
    for (int i = (int) joints.size() - 1; i >= 0; i--) {
        joints[i]->intensity *= 0.99;
        if (joints[i]->intensity < 0.1) joints[i]->intensity = 0.1f;
    }
}

void TentacleIK::applySinusoidalMovement(float amplitude, float frequency, float time)
{
    float accumulatedPhase = time;

    float waveLength = 2 * M_PI / (joints.size() - 1);

    for (int i = 1; i < joints.size(); i++) {
        float phase = accumulatedPhase + i * waveLength + frequency;

        float displacementY = amplitude * std::cos(phase) * 0.005f;

        auto r = M3::getMatrixRotationForEulerAngles(0, 0, displacementY);

        joints[i]->rotation = r * joints[i]->rotation;

        accumulatedPhase += frequency;
        updateRotations();
    }
}

void TentacleIK::setRootPosition(Vertex3D position)
{
    Vertex3D distance = position - joints.front()->start;

    Vector3D v(position, joints.front()->start);
    Drawable::drawVector3D(v, Color::red());

    joints[0]->start = joints[0]->start + distance.getScaled(0.2);
    joints[0]->end = joints[0]->end + distance.getScaled(0.2);
    updateRotations();
}