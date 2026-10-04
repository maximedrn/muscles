#include "MuscleViewer.h"
#include <QKeyEvent>
#include <cmath>
#include <memory>
#include <numbers>

MuscleViewer::MuscleViewer()
    : upperArm(1.4, Vec(0.82, 0.85, 0.9)),
      forearm(1.2, Vec(0.65, 0.73, 0.85), &this->upperArm.frame),
      muscle(this->upperArm, this->forearm) {
    this->resetScene();
}

MuscleViewer::~MuscleViewer() {
    if (this->animationIsStarted()) {
        this->stopAnimation();
    }
}

void MuscleViewer::init() {
    glDisable(GL_LIGHTING);
    this->setBackgroundColor(QColor(32, 35, 41));
    this->setSceneCenter(Vec(-0.15, 0.0, 0.7));
    this->setSceneRadius(1.8);
    this->camera()->setViewDirection(Vec(0.0, 1.0, 0.0));
    this->camera()->setUpVector(Vec(0.0, 0.0, 1.0));
    this->showEntireScene();

    this->setShortcut(CAMERA_MODE, 0);
    this->setShortcut(ANIMATION, 0);
    this->setShortcut(EXIT_VIEWER, Qt::Key_Escape);
    this->setKeyDescription(Qt::Key_Space, "Start or pause animation");
    this->setKeyDescription(Qt::Key_R, "Reset pose and pause");
    this->setAnimationPeriod(10);
    this->resetScene();
}

void MuscleViewer::resetScene() {
    if (this->animationIsStarted()) {
        this->stopAnimation();
    }
    this->simulationTime = 0.0;
    this->elbowAngle = MuscleViewer::minimumAngle;
    this->upperArm.frame.setTranslation(Vec(-this->upperArm.length, 0.0, 0.25));
    this->upperArm.frame.setRotation(
        Quaternion(Vec(0.0, 1.0, 0.0), std::numbers::pi / 2.0)
    );
    // Place the elbow at the end of the parent bone, in parent coordinates.
    this->forearm.frame.setTranslation(Vec(0.0, 0.0, this->upperArm.length));
    this->updateBonePose();
    this->muscle.reset();
}

void MuscleViewer::updateBonePose() {
    const double angle = this->elbowAngle * std::numbers::pi / 180.0;
    // Only the forearm rotation changes; the elbow translation stays fixed.
    this->forearm.frame.setRotation(Quaternion(Vec(0.0, -1.0, 0.0), angle));
}

void MuscleViewer::animate() {
    this->simulationTime += MuscleViewer::timeStep;
    const double phase = 2.0 * std::numbers::pi * this->simulationTime /
        MuscleViewer::cycleDuration;
    // Start continuously at minimumAngle, then flex and extend periodically.
    const double blend = 0.5 * (1.0 - std::cos(phase));
    this->elbowAngle = MuscleViewer::minimumAngle +
        (MuscleViewer::maximumAngle - MuscleViewer::minimumAngle) * blend;
    this->updateBonePose();
    this->muscle.update();
}

void MuscleViewer::drawJoints() const {
    static const std::unique_ptr<GLUquadric, decltype(&gluDeleteQuadric)> quad(
        gluNewQuadric(), &gluDeleteQuadric
    );
    if (!quad) {
        return;
    }

    const Vec joints[]{
        this->upperArm.startPosition(),
        this->upperArm.endPosition(),
        this->forearm.endPosition()
    };
    glColor3f(0.95f, 0.85f, 0.55f);
    for (const Vec& joint : joints) {
        glPushMatrix();
        glTranslated(joint.x, joint.y, joint.z);
        gluSphere(quad.get(), 0.085, 20, 12);
        glPopMatrix();
    }
}

void MuscleViewer::draw() {
    this->upperArm.draw();
    this->forearm.draw();
    this->drawJoints();
    this->muscle.draw();
    this->muscle.drawAttachments();

    glColor3f(1.0f, 1.0f, 1.0f);
    this->drawText(
        15,
        25,
        QStringLiteral("Muscles | %1 | t = %2 s | angle = %3 deg")
            .arg(this->animationIsStarted() ? "running" : "paused")
            .arg(this->simulationTime, 0, 'f', 2)
            .arg(this->elbowAngle, 0, 'f', 1)
    );
    this->drawText(15, 45, "Space: start/pause | R: reset | H: help");
}

void MuscleViewer::keyPressEvent(QKeyEvent* const event) {
    if (event->modifiers() != Qt::NoModifier || event->isAutoRepeat()) {
        QGLViewer::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Space) {
        if (this->animationIsStarted()) {
            this->stopAnimation();
        } else {
            this->startAnimation();
        }
    } else if (event->key() == Qt::Key_R) {
        this->resetScene();
    } else {
        QGLViewer::keyPressEvent(event);
        return;
    }

    event->accept();
    this->update();
}

QString MuscleViewer::helpString() const {
    return QStringLiteral(
        "<h2>Muscles - Practice 6</h2>"
        "<p><b>Space</b>: start or pause. <b>R</b>: reset pose and pause.</p>"
        "<p>The upper arm stays fixed. The forearm rotates at the elbow. "
        "The three spheres are the shoulder, elbow, and wrist.</p>"
        "<p>Orange points show fixed muscle attachments in each bone's "
        "local frame. Complete TODO 1-5 in Muscle.cpp to draw a "
        "constant-volume ellipsoid between them.</p>"
        "<p>Standard QGLViewer camera controls remain available.</p>"
    );
}
