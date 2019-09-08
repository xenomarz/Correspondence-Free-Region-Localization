cmake_minimum_required(VERSION 3.15)
project(node_addon)

# Sources
file(GLOB SOURCE_FILES "addon.cpp" "engine.cpp" "engine.h" ${CMAKE_SOURCE_DIR}/natvis/eigen.natvis)

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR)

string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

# Add Library Target
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

# Include Directories
target_include_directories(${PROJECT_NAME} 
	PRIVATE
		${CMAKE_JS_INC}
		${CMAKE_SOURCE_DIR}
		${NODE_ADDON_API_DIR})

# Link Libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        igl::core
        igl::opengl
        rds::optimization_lib
        ${CMAKE_JS_LIB})

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Custom Commands
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND call $(ProjectDir)..\\..\\scripts\\post_build.bat ARGS "$(TargetDir)$(TargetFileName)" "$(ProjectDir)latest_builds\\"
                   COMMENT "Copying .node file to latest_builds folder")