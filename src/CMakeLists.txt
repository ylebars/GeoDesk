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

find_package(
        Boost 
        1.31.1
        REQUIRED program_options
)

find_package(Qt4 REQUIRED)

set(
  SOURCE_FILES
  main.cpp
)
set(
  QT_HEADER_FILES
)
QT4_WRAP_CPP(QT_HEADERS_MOC ${QT_HEADER_FILES})

link_directories(
        ${Boost_LIBRARY_DIRS}
)
include_directories(
        ${Boost_INCLUDE_DIRS}
)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(
  ${EXECUTABLE_NAME}
  ${SOURCE_FILES}
  ${QT_HEADERS_MOC}
)
target_link_libraries(
  ${EXECUTABLE_NAME}
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)
