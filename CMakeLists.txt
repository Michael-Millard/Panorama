cmake_minimum_required(VERSION 3.14)

project(Panorama LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find OpenCV 4.x
find_package(OpenCV 4.0 REQUIRED COMPONENTS core imgproc imgcodecs highgui features2d stitching)

message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")

include_directories(
	${CMAKE_SOURCE_DIR}/include
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB_RECURSE PANORAMA_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(panorama
	${CMAKE_SOURCE_DIR}/main.cpp
	${PANORAMA_SRC}
)

target_link_libraries(panorama PRIVATE ${OpenCV_LIBS})

if(WIN32)
	# Ensure runtime can find OpenCV DLLs when launching from build tree
	add_custom_command(TARGET panorama POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo "Copy OpenCV runtime if needed"
	)
endif()

