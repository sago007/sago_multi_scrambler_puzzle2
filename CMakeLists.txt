cmake_minimum_required(VERSION 3.10.2..3.31.6)
project (sago_multi_scrambler_puzzle2)
find_package(Boost COMPONENTS program_options REQUIRED NO_MODULE)

option(USE_EMBEDDED_PLATFORM_FOLDERS "Use the embedded version of PlatformFolders" ON)

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

if (USE_EMBEDDED_PLATFORM_FOLDERS)
	message("Using embedded PlatformFolders")
	add_subdirectory("embedded_libs/PlatformFolders-4.2.0" EXCLUDE_FROM_ALL)
	message("Done processing PlatformFolders")
endif()


include_directories(SYSTEM "src/Libs/include")

include_directories(SYSTEM "src/Libs/imgui")


file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp" "src/*/*/*/*.cpp" "src/Libs/*.c")

add_executable(sago_multi_scrambler_puzzle2 ${SOURCES})
TARGET_LINK_LIBRARIES( sago_multi_scrambler_puzzle2 ${Boost_LIBRARIES} )
target_link_libraries( sago_multi_scrambler_puzzle2 ${SDL2_LIBRARIES})
target_link_libraries( sago_multi_scrambler_puzzle2 physfs platform_folders rhash)
target_link_libraries( sago_multi_scrambler_puzzle2 ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})