cmake_minimum_required(VERSION 3.23)

project(DOOM)

# Enable C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SDL2_image_DIR "/opt/homebrew/Cellar/sdl2_image/2.6.3_2/lib/libSDL2_image-2.0.0.dylib")
set(SDL2_TTF_DIR "/opt/homebrew/Cellar/sdl2_ttf/2.20.2/lib/libSDL2_ttf-2.0.0.dylib")
set(SDL2_MIXER_DIR "/opt/homebrew/Cellar/sdl2_mixer/2.6.3_1/lib/libSDL2_mixer-2.0.0.dylib")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_SOURCE_DIR}/libs/sdl2_image/include
    PUBLIC /opt/homebrew/Cellar/sdl2_ttf/2.20.2/include
    PUBLIC /opt/homebrew/Cellar/sdl2_mixer/2.6.3_1/include
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_image_DIR}
    ${SDL2_TTF_DIR}
    ${SDL2_MIXER_DIR}
)
