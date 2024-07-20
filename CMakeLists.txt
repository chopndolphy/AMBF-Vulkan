cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -Wall -Wextra -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")


project(ambf_engine VERSION 1.0)

set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/)

## IMGUI 
file(GLOB_RECURSE IMGUI_SRC_FILES ${LIB}/imgui/*.cpp)

## MY FILES
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(ambf_engine ${CMAKE_CURRENT_SOURCE_DIR}/src/ambf_engine.cpp ${SRC_FILES} ${IMGUI_SRC_FILES})
target_include_directories(ambf_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIB}/imgui)


## INCLUDE LIBRARIES
add_subdirectory(${LIB}/fastgltf)
set(FASTGLTF_INCLUDE_DIR ${LIB}/fastgltf/include)
set(FASTGLTF_SIMDJSON_INCLUDE_DIR ${LIB}/fastgltf/deps/simdjson)
target_include_directories(ambf_engine PUBLIC ${FASTGLTF_INCLUDE_DIR})
target_include_directories(ambf_engine PUBLIC ${FASTGLTF_SIMDJSON_INCLUDE_DIR})

add_subdirectory(${LIB}/fmt)
set(FMT_INCLUDE_DIR ${LIB}/fmt/include)
target_include_directories(ambf_engine PUBLIC ${FMT_INCLUDE_DIR})

set(GLM_INCLUDE_DIR ${LIB}/glm/glm)
target_include_directories(ambf_engine PUBLIC ${GLM_INCLUDE_DIR})
add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)

set(STB_IMAGE_INCLUDE_DIR ${LIB}/stb_image)
target_include_directories(ambf_engine PUBLIC ${STB_IMAGE_INCLUDE_DIR})

add_subdirectory(${LIB}/vkbootstrap)
set(VKBOOTSTRAP_INCLUDE_DIR ${LIB}/vkbootstrap/src)
target_include_directories(ambf_engine PUBLIC ${VKBOOTSTRAP_INCLUDE_DIR})

set(VMA_INCLUDE_DIR ${LIB}/vma)
target_include_directories(ambf_engine PUBLIC ${VMA_INCLUDE_DIR})

## LINK LIBRARIES
find_library(FASTGLTF fastgltf PATHS ${LIB}/fastgltf/build)
target_link_libraries(ambf_engine PUBLIC ${FASTGLTF})

find_library(FASTGLTF_SIMDJSON fastgltf_simdjson PATHS ${LIB}/fastgltf/build)
target_link_libraries(ambf_engine PUBLIC ${FASTGLTF_SIMDJSON})

find_library(FMT fmt PATHS ${LIB}/fmt/build)
target_link_libraries(ambf_engine PUBLIC ${FMT})

find_library(VKBOOTSTRAP vk-bootstrap PATHS ${LIB}/vkbootstrap/build)
target_link_libraries(ambf_engine PUBLIC ${VKBOOTSTRAP})

# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(ambf_engine PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(ambf_engine PRIVATE SDL2::SDL2)

find_library(VULKAN vulkan)
target_link_libraries(ambf_engine PUBLIC ${VULKAN})

find_library(XXF86VM Xxf86vm)
target_link_libraries(ambf_engine PUBLIC ${XXF86VM})

find_library(X11 X11)
target_link_libraries(ambf_engine PUBLIC ${X11})

find_library(PTHREAD pthread)
target_link_libraries(ambf_engine PUBLIC ${PTHREAD})

find_library(XRANDR Xrandr)
target_link_libraries(ambf_engine PUBLIC ${XRANDR})

find_library(XI Xi)
target_link_libraries(ambf_engine PUBLIC ${XI})

find_library(DL dl)
target_link_libraries(ambf_engine PUBLIC ${DL})





