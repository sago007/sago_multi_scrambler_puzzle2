cmake_minimum_required(VERSION 3.10.2)
project (sago_multi_scrambler_puzzle2)
find_package(Boost COMPONENTS program_options REQUIRED)

#Setup SDL2
find_package(SDL2 REQUIRED CONFIG)
include_directories(${SDL2_INCLUDE_DIRS})

#Setup things that use pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
include_directories(${SDL2MIXER_INCLUDE_DIRS})
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)
include_directories(${SDL2IMAGE_INCLUDE_DIRS})
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)
include_directories(${SDL2TTF_INCLUDE_DIRS})
pkg_search_module(SDL2GFX REQUIRED SDL2_gfx)
include_directories(${SDL2GFX_INCLUDE_DIRS})


pkg_search_module(IMGUI REQUIRED imgui)
include_directories(${IMGUI_INCLUDE_DIRS})


include_directories(SYSTEM "src/Libs/include")


file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp" "src/Libs/*.c")

add_executable(sago_multi_scrambler_puzzle2 ${SOURCES})
TARGET_LINK_LIBRARIES( sago_multi_scrambler_puzzle2 ${Boost_LIBRARIES} )
target_link_libraries( sago_multi_scrambler_puzzle2 ${SDL2_LIBRARIES})
target_link_libraries( sago_multi_scrambler_puzzle2 physfs)
target_link_libraries( sago_multi_scrambler_puzzle2 ${IMGUI_LIBRARIES})
target_link_libraries( sago_multi_scrambler_puzzle2 ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})