#pragma once

#include "Bone.h"
#include <QGLViewer/quaternion.h>

class Muscle {
  public:
    explicit Muscle(const Bone& originBone, const Bone& insertionBone);

    void reset();
    void update();
    void draw() const;
    void drawAttachments() const;

  private:
    void updateAttachments();
    void initializeReferenceVolume();
    void updatePose();
    void updateRadii();

    const Bone& originBone;
    const Bone& insertionBone;
    const Vec originLocalPosition;
    const Vec insertionLocalPosition;
    const double referenceRadius = 0.16;

    Vec originPosition;
    Vec insertionPosition;
    Vec center;
    Quaternion orientation;
    // a and b are radial semi-axes; c is the longitudinal semi-axis.
    Vec semiAxes;
    double referenceVolume = 0.0;
};
