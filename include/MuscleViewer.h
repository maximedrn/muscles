#pragma once

#include "Bone.h"
#include "Muscle.h"
#include <QGLViewer/qglviewer.h>

class MuscleViewer : public QGLViewer {
    Q_OBJECT

  public:
    MuscleViewer();
    ~MuscleViewer() override;

  protected:
    void init() override;
    void draw() override;
    void animate() override;
    void keyPressEvent(QKeyEvent* event) override;
    QString helpString() const override;

  private:
    static constexpr double timeStep = 0.01;
    static constexpr double minimumAngle = 25.0;
    static constexpr double maximumAngle = 115.0;
    static constexpr double cycleDuration = 4.0;

    void resetScene();
    void updateBonePose();
    void drawJoints() const;

    // Parent frames must outlive their children and the muscle references.
    Bone upperArm;
    Bone forearm;
    Muscle muscle;
    double simulationTime = 0.0;
    double elbowAngle = minimumAngle;
};
