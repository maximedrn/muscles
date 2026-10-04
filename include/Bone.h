#pragma once

#include <QGLViewer/frame.h>

using namespace qglviewer;

class Bone {
  public:
    explicit Bone(
        double boneLength,
        const Vec& boneColor,
        const Frame* parentFrame = nullptr
    );

    Bone(const Bone&) = delete;
    Bone& operator=(const Bone&) = delete;

    Vec startPosition() const;
    Vec endPosition() const;
    void draw() const;
    void drawAttachment(const Vec& localPosition) const;

    // The bone extends from (0, 0, 0) to (0, 0, length) in this frame.
    Frame frame;
    const double length;

  private:
    static constexpr double radius = 0.055;
    const Vec color;
};
