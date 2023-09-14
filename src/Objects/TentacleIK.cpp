//
// Created by Eduardo on 09/09/2023.
//

#include "../../include/Objects/TentacleIK.h"
#include "../../include/Render/Drawable.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include <random>  // Para generación de números aleatorios

TentacleIK::TentacleIK(Vertex3D position, Object3D * target)
:
    target(target),
    velocity(Vertex3D(0, 0, 0))
{
    setPosition(position);
}

void TentacleIK::onUpdate() {
    Object3D::onUpdate();
}

void TentacleIK::postUpdate() {
    Object3D::postUpdate();
}

void TentacleIK::drawOnUpdateSecondPass() {
    Object3D::drawOnUpdateSecondPass();
}

void TentacleIK::onDrawHostBuffer()
{
    Object3D::onDrawHostBuffer();

    IKForwardSolver();
}

void TentacleIK::IKForwardSolver()
{
    if ((int) joints.size() <= 0) return;

    //attractRootToTarget();
    transformJoints();
    draw();
}

void TentacleIK::attractRootToTarget()
{
    const float distance = joints.back()->endWorld.distance(target->getPosition());
    const float speed = 0.05;

    if (distance > 100) {
        Vertex3D velocityDirection = target->getPosition() - joints.back()->endWorld;
        velocity = velocityDirection.getScaled(speed);

        joints[0]->start = joints[0]->start + velocity;
        joints[0]->end = joints[0]->end + velocity;

        Vertex3D targetDirection = joints[0]->startWorld - joints.back()->endWorld;
        Vertex3D currentDirection = joints[0]->endWorld - joints.back()->endWorld;;

        const float toEndModule = targetDirection.getModule();
        const float toCurrentModule = currentDirection.getModule();

        const float factor = toEndModule * toCurrentModule;
        const float cosTheta = (targetDirection * currentDirection) / factor;
        float angle = acos(std::clamp(cosTheta, -1.0f, 1.0f));

        if (Maths::radiansToDegrees(angle) > 30) {
            return;
        }
        Vertex3D rotationAxis = (targetDirection % currentDirection).getNormalize();

        angle = std::clamp(angle, Maths::degreesToRadians(0.1), Maths::degreesToRadians(45));

        joints[0]->rotation = M3::arbitraryAxis(rotationAxis, angle) * joints[0]->rotation;
    }
}

void TentacleIK::updateInertiaRoot()
{
    Vertex3D targetDirection = joints.front()->startWorld  - joints.front()->endWorld;
    Vertex3D currentDirection = joints.back()->startWorld - joints.back()->endWorld;

    const float toDirectionModule = targetDirection.getModule();
    const float toCurrentModule = currentDirection.getModule();

    const float factor = toDirectionModule * toCurrentModule;
    const float cosTheta = (targetDirection * currentDirection) / factor;
    float angle = acos(std::clamp(cosTheta, -1.0f, 1.0f));

    Vertex3D rotationAxis = (targetDirection % currentDirection).getNormalize();

    angle = std::clamp(angle, Maths::degreesToRadians(0.1), Maths::degreesToRadians(1));

    joints[0]->rotation = M3::arbitraryAxis(rotationAxis, angle) * joints[0]->rotation;
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
        //transformJointSpiral(i);
        applySinusoidalMovement(1, 0.01, Brakeza3D::get()->getExecutionTime());
    }
}

void TentacleIK::transformJoint(int i)
{
    Vertex3D targetPosition = target->getPosition();

    Vertex3D toEnd = joints.back()->endWorld - joints[i]->startWorld;
    Vertex3D toTarget = targetPosition - joints[i]->startWorld;

    const float toEndModule = toEnd.getModule();
    const float toTargetModule = toTarget.getModule();

    const float factor = toEndModule * toTargetModule;
    const float cosTheta = (toEnd * toTarget) / factor;
    float angle = acos(std::clamp(cosTheta, -1.0f, 1.0f));

    Vertex3D rotationAxis = (toEnd % toTarget).getNormalize();

    // Singularity skip
    if (!Tools::isValidVector(rotationAxis)) {
        rotationAxis = Vertex3D(0, 0, 0);
    }

    // Limitar el ángulo a un rango específico
    angle = std::clamp(angle, Maths::degreesToRadians(0.1), Maths::degreesToRadians(1));

    joints[i]->rotation = M3::arbitraryAxis(rotationAxis, angle) * joints[i]->rotation;

    updateRotations();
}

void TentacleIK::transformJointSpiral(int i)
{

    Vertex3D centerPoint = joints.front()->start; // Puedes ajustar este punto según tus necesidades


    // Usamos el producto cruz para encontrar el eje de rotación
    Vertex3D rotationAxis = Vertex3D(0, 0, 1);

    // Ahora, en lugar de utilizar un ángulo basado en la posición del objetivo,
    // usamos un ángulo que mantendrá el segmento en una trayectoria circular.
    float angle = 2 * M_PI / joints.size() * i;

    // Aplicamos la rotación
    joints[i]->rotation = M3::arbitraryAxis(rotationAxis, sin(Brakeza3D::get()->getExecutionTime()));

    //updateRotations();
}

void TentacleIK::setTarget(Object3D *target) {
    TentacleIK::target = target;
}

void TentacleIK::draw()
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto shaderLasers = ComponentsManager::get()->getComponentGame()->getShaderLasers();

    for (auto & joint : joints) {

        Point2D startScreenPoint = Transforms::WorldToPoint(joint->startWorld, camera);
        Point2D endScreenPoint = Transforms::WorldToPoint(joint->endWorld, camera);

        shaderLasers->addLaser(
            startScreenPoint.x, startScreenPoint.y,
            endScreenPoint.x, endScreenPoint.y,
            PaletteColors::getStamina(),
            0.25f,
            false,
            false
        );
    }
}

void TentacleIK::applySinusoidalMovement(float amplitude, float frequency, float time)
{
    float accumulatedPhase = time;

    float waveLength = 2 * M_PI / (joints.size() - 1);

    for (int i = 1; i < joints.size(); i++) {
        float phase = accumulatedPhase + i * waveLength + frequency;

        float displacementY = amplitude * std::cos(phase) * 0.05f;

        // Aplicar la rotación resultante
        auto r = M3::getMatrixRotationForEulerAngles(0, 0, displacementY);
        joints[i]->rotation = r * joints[i]->rotation;

        accumulatedPhase += frequency;
        updateRotations();
    }
}
