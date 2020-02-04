# CMakeList.txt: CMake-Projekt für "robi". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)

project ("robot")

# Schließen Sie Unterprojekte ein.
#add_subdirectory ("robi")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

file(GLOB robiSources *.cpp)
file(GLOB robiIncludes *.h *.hpp)

# Declare the executable target built from your sources
add_executable(robot ${robiSources} ${robiIncludes})

# Link your application with OpenCV libraries
target_link_libraries(robot ${OpenCV_LIBS}  pthread JetsonGPIO)


# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
