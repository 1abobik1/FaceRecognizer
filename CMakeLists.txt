cmake_minimum_required(VERSION 3.16)
project(FaceRecognizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui videoio objdetect face)

add_executable(facerec
    src/main.cpp
    src/FaceModelTrainer.cpp
    src/FaceRecognition.cpp
    src/FileOperations.cpp
    src/Preprocess.cpp
)

target_include_directories(facerec PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(facerec PRIVATE ${OpenCV_LIBS})

if(MSVC)
    target_compile_options(facerec PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(facerec PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy the Haar cascade next to the binary so the program finds it without absolute paths.
add_custom_command(TARGET facerec POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:facerec>/data
)
