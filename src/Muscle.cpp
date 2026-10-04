#include "Muscle.h"
#include <cmath>
#include <numbers>

Muscle::Muscle(const Bone& originBone, const Bone& insertionBone)
    : originBone(originBone), insertionBone(insertionBone),
      originLocalPosition(-0.12, 0.0, 0.55 * originBone.length),
      insertionLocalPosition(-0.12, 0.0, 0.35 * insertionBone.length) {}

void Muscle::reset() {
    this->originPosition = Vec();
    this->insertionPosition = Vec();
    this->center = Vec();
    this->orientation = Quaternion();
    this->semiAxes = Vec(this->referenceRadius, this->referenceRadius, 0.0);
    this->referenceVolume = 0.0;

    this->updateAttachments();
    this->initializeReferenceVolume();
    this->updatePose();
    this->updateRadii();
}

void Muscle::update() {
    this->updateAttachments();
    this->updatePose();
    this->updateRadii();
}

void Muscle::updateAttachments() {
    this->originPosition =
        this->originBone.frame.inverseCoordinatesOf(this->originLocalPosition);
    this->insertionPosition = this->insertionBone.frame.inverseCoordinatesOf(
        this->insertionLocalPosition
    );
}

void Muscle::initializeReferenceVolume() {
    const double L0 = (this->insertionPosition - this->originPosition).norm();
    const double c0 = L0 / 2.0;
    this->referenceVolume = (4.0 / 3.0) * std::numbers::pi *
        this->referenceRadius * this->referenceRadius * c0;
}

void Muscle::updatePose() {
    this->center = (this->originPosition + this->insertionPosition) / 2.0;
    const Vec direction = this->insertionPosition - this->originPosition;
    const double length = direction.norm();
    if (length > 0.0) {
        this->orientation = Quaternion(Vec(0.0, 0.0, 1.0), direction);
    }
    this->semiAxes.z = length / 2.0;
}

void Muscle::updateRadii() {
    this->semiAxes.x = 0.0;
    this->semiAxes.y = 0.0;
    if (this->semiAxes.z > 0.0 && this->referenceVolume > 0.0) {
        this->semiAxes.x = std::sqrt(
            3.0 * this->referenceVolume /
            (4.0 * std::numbers::pi * this->semiAxes.z)
        );
        this->semiAxes.y = this->semiAxes.x;
    }
}

void Muscle::draw() const {
    if (this->semiAxes.x > 0.0 && this->semiAxes.y > 0.0 &&
        this->semiAxes.z > 0.0) {
        glPushMatrix();
        glTranslated(this->center.x, this->center.y, this->center.z);
        GLdouble rotation[16];
        this->orientation.getMatrix(rotation);
        glMultMatrixd(rotation);
        glScaled(this->semiAxes.x, this->semiAxes.y, this->semiAxes.z);
        static GLUquadric* quadric = gluNewQuadric();
        gluSphere(quadric, 1.0, 20, 20);
        glPopMatrix();
    }
}

void Muscle::drawAttachments() const {
    // These local markers work before the muscle exercises are completed.
    this->originBone.drawAttachment(this->originLocalPosition);
    this->insertionBone.drawAttachment(this->insertionLocalPosition);
}
