cmake_minimum_required(VERSION 3.15)
project(node-addon)
include_directories(${CMAKE_JS_INC})
file(GLOB SOURCE_FILES "test.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_UseIntelMKL "Parallel")
set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_OpenMPSupport "Yes")