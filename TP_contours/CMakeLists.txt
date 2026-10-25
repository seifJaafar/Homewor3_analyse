cmake_minimum_required(VERSION 3.10)
project(TP_contours LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # build/compile_commands.json, pour l'IntelliSense de VS Code

find_package(OpenCV REQUIRED)

add_executable(tp_contours
    src/main.cpp
    src/detecteurs.cpp
    src/evaluation.cpp)
target_include_directories(tp_contours PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tp_contours PRIVATE ${OpenCV_LIBS})

# Dossier du projet, pour retrouver data/ et results/ quel que soit le dossier de lancement
target_compile_definitions(tp_contours PRIVATE "DOSSIER_PROJET=\"${CMAKE_SOURCE_DIR}\"")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(tp_contours PRIVATE -Wall -Wextra)
endif()
