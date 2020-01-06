cmake_minimum_required(VERSION 3.5)
# set the project name
project(worhp)

add_library(${PROJECT_NAME} SHARED IMPORTED)

set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext/worhp/bin/worhp.lib")