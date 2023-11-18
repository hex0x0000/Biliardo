cmake_minimum_required(VERSION 3.5)
project(biliardo)

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Threads REQUIRED)
#find_package(OpenSSL REQUIRED)

#add_subdirectory(SDL2)
#set(SDL2IMAGE_INSTALL OFF)
#set(BUILD_SHARED_LIBS FALSE)
#add_subdirectory(SDL2_image)
#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")
#add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
#target_link_libraries(${CMAKE_PROJECT_NAME} SDL2::SDL2-static SDL2_image-static)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.26.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.6.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(SDL2IMAGE_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)
FetchContent_MakeAvailable(SDL2_image)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} SDL2::SDL2-static SDL2_image::SDL2_image-static)
