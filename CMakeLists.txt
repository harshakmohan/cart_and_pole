cmake_minimum_required(VERSION 3.16)
project(cartpole_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(glfw3 REQUIRED)

# Find MuJoCo on macOS
if(APPLE)
    set(MUJOCO_FRAMEWORK "/Applications/MuJoCo.app/Contents/Frameworks/mujoco.framework")
    set(MUJOCO_LIB "${MUJOCO_FRAMEWORK}/Versions/A/libmujoco.3.3.6.dylib")
    if(NOT EXISTS ${MUJOCO_LIB})
        message(FATAL_ERROR "MuJoCo not found at ${MUJOCO_LIB}. Please install MuJoCo.app")
    endif()
    
    # Create a temporary include directory with proper structure
    set(TEMP_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")
    file(MAKE_DIRECTORY "${TEMP_INCLUDE_DIR}/mujoco")
    file(GLOB MUJOCO_HEADERS "${MUJOCO_FRAMEWORK}/Headers/*.h")
    foreach(header ${MUJOCO_HEADERS})
        get_filename_component(header_name ${header} NAME)
        configure_file(${header} "${TEMP_INCLUDE_DIR}/mujoco/${header_name}" COPYONLY)
    endforeach()
    
    set(MUJOCO_INCLUDE_PATH "${TEMP_INCLUDE_DIR}")
endif()

# Main executable
add_executable(cartpole src/main.cpp)
target_include_directories(cartpole PRIVATE include ${MUJOCO_INCLUDE_PATH})
target_link_libraries(cartpole PRIVATE ${MUJOCO_LIB} glfw)

# Test environment executable (original)
add_executable(test_env src/test_env.cpp src/cartpole_env.cpp src/environment_factory.cpp src/config.cpp)
target_include_directories(test_env PRIVATE include ${MUJOCO_INCLUDE_PATH})
target_link_libraries(test_env PRIVATE ${MUJOCO_LIB} glfw)

# Framework test executable
add_executable(test_framework 
    src/test_framework.cpp
    src/cartpole_env.cpp
    src/environment_factory.cpp
    src/agent_factory.cpp
    src/experiment_runner.cpp
    src/config.cpp
    src/agents/random_agent.cpp
    src/agents/rule_based_agent.cpp
)
target_include_directories(test_framework PRIVATE include ${MUJOCO_INCLUDE_PATH})
target_link_libraries(test_framework PRIVATE ${MUJOCO_LIB} glfw)
