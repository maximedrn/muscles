cmake_minimum_required(VERSION 3.20)
project(muscles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.cmake")

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_QT5 QUIET Qt5Widgets)
endif()

find_package(Qt5 5.9 REQUIRED COMPONENTS Widgets OpenGL Xml
    HINTS "${PC_QT5_PREFIX}/lib/cmake/Qt5")
find_package(OpenGL REQUIRED)
include(QGLViewer)

add_executable(muscles
    src/main.cpp
    src/Bone.cpp
    include/Bone.h
    src/Muscle.cpp
    include/Muscle.h
    src/MuscleViewer.cpp
    include/MuscleViewer.h
)
target_include_directories(muscles PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(muscles PRIVATE
    QT_NO_KEYWORDS
    $<$<PLATFORM_ID:Windows>:WIN32>
)
target_link_libraries(muscles PRIVATE QGLViewer::QGLViewer)

if(MSVC)
    target_compile_options(muscles PRIVATE /W4)
else()
    target_compile_options(muscles PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(muscles PROPERTIES
    AUTOMOC ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    get_target_property(MUSCLES_SOURCES muscles SOURCES)
    add_custom_target(format
        COMMAND "${CLANG_FORMAT_EXECUTABLE}" --style=file -i ${MUSCLES_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Formatting C++ sources"
        VERBATIM
    )
    add_custom_target(format-check
        COMMAND "${CLANG_FORMAT_EXECUTABLE}" --style=file --dry-run --Werror ${MUSCLES_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Checking C++ formatting"
        VERBATIM
    )
else()
    message(STATUS "clang-format not found; format and format-check targets are unavailable")
endif()

add_custom_target(run
    COMMAND "$<TARGET_FILE:muscles>"
    DEPENDS muscles
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)
