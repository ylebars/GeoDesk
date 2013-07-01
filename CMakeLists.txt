## Definition of the project GeoDesk.

cmake_minimum_required(VERSION 2.6)

project(GeoDesk CXX)

# Set version information.
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(PATCH_VERSION 1)

add_subdirectory(src)
add_subdirectory(doc)
