#include "../../include/Render/PhysicsDebugDraw.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Logging.h"

PhysicsDebugDraw::PhysicsDebugDraw() {
}

void PhysicsDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    Vertex3D a = Vertex3D(from.x(), from.y(), from.z());
    Vertex3D b = Vertex3D(to.x(), to.y(), to.z());

    Drawable::drawVector3D(Vector3D(a, b), Color::red());

    glColor3f(color.x(), color.y(), color.z());
    glBegin(GL_LINES);
    glVertex3f(from.x(), from.y(), from.z());
    glVertex3f(to.x(), to.y(), to.z());
    glEnd();
}

void PhysicsDebugDraw::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance,
                                        int lifeTime, const btVector3 &color) {

}

void PhysicsDebugDraw::reportErrorWarning(const char *warningString) {

}

void PhysicsDebugDraw::draw3dText(const btVector3 &location, const char *textString) {

}

void PhysicsDebugDraw::setDebugMode(int debugMode) {

}

int PhysicsDebugDraw::getDebugMode() const {
    return btIDebugDraw::DBG_DrawWireframe;
}
