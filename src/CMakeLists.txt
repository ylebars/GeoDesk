## Project compilation.

# Path for the executable.
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
# Name of the executable.
set(EXECUTABLE_NAME geodesk)

configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
  "${PROJECT_BINARY_DIR}/config.hpp"
)
include_directories(${PROJECT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

find_package(
        Boost 
        1.31.1
        REQUIRED program_options
)

find_package(Eigen3 REQUIRED)

find_package(Qt4 REQUIRED)

set(
  SOURCE_FILES
  main.cpp
  mainboard.cpp
  projection.hpp
)
set(
  QT_HEADER_FILES
  mainboard.hpp
)
QT4_WRAP_CPP(QT_HEADERS_MOC ${QT_HEADER_FILES})

set (
  QT_FORM_FILES
  mainboard.ui
)
QT4_WRAP_UI(QT_FORM_HEADERS ${QT_FORM_FILES})

link_directories(
        ${Boost_LIBRARY_DIRS}
)
include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Boost_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIR}
)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(
  ${EXECUTABLE_NAME}
  ${SOURCE_FILES}
  ${QT_HEADERS_MOC}
  ${QT_FORM_HEADERS}
)
target_link_libraries(
  ${EXECUTABLE_NAME}
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)
