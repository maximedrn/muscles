#include "MuscleViewer.h"
#include <QApplication>
#include <QCommandLineParser>
#include <cstdlib>
#include <iostream>

int main(int argc, char** const argv) {
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName("muscles");

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Muscle practice scaffold: constant-volume ellipsoid"
    );
    parser.addHelpOption();
    parser.process(application);
    if (!parser.positionalArguments().isEmpty()) {
        std::cerr << "Usage: muscles [--help]\n";
        return EXIT_FAILURE;
    }

    MuscleViewer viewer;
    viewer.setWindowTitle("Muscles - Practice 6");
    viewer.resize(900, 700);
    viewer.show();

    return application.exec();
}
