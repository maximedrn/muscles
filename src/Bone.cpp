#include "Bone.h"
#include <cmath>
#include <memory>
#include <stdexcept>

Bone::Bone(
    const double boneLength,
    const Vec& boneColor,
    const Frame* const parentFrame
)
    : length(boneLength), color(boneColor) {
    if (!std::isfinite(this->length) || this->length <= 0.0) {
        throw std::invalid_argument("Bone length must be positive and finite");
    }
    this->frame.setReferenceFrame(parentFrame);
}

Vec Bone::startPosition() const {
    return this->frame.position();
}

Vec Bone::endPosition() const {
    return this->frame.inverseCoordinatesOf(Vec(0.0, 0.0, this->length));
}

void Bone::draw() const {
    static const std::unique_ptr<GLUquadric, decltype(&gluDeleteQuadric)> quad(
        gluNewQuadric(), &gluDeleteQuadric
    );
    if (!quad) {
        return;
    }

    glPushMatrix();
    // worldMatrix includes the parent frame's transform.
    glMultMatrixd(this->frame.worldMatrix());
    glColor3fv(this->color);
    gluCylinder(quad.get(), Bone::radius, Bone::radius, this->length, 20, 1);
    glPushMatrix();
    glRotated(180.0, 1.0, 0.0, 0.0);
    gluDisk(quad.get(), 0.0, Bone::radius, 20, 1);
    glPopMatrix();
    glTranslated(0.0, 0.0, this->length);
    gluDisk(quad.get(), 0.0, Bone::radius, 20, 1);
    glPopMatrix();
}

void Bone::drawAttachment(const Vec& localPosition) const {
    glPushMatrix();
    glMultMatrixd(this->frame.worldMatrix());
    glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT);
    glColor3f(1.0f, 0.45f, 0.15f);
    glPointSize(9.0f);
    glBegin(GL_POINTS);
    glVertex3fv(localPosition);
    glEnd();
    glPopAttrib();
    glPopMatrix();
}
