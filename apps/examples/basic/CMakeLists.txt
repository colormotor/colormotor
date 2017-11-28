cmake_minimum_required(VERSION 2.8.3)
project(basic_example)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

find_package(Armadillo REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

#find_library(GLFW3_LIB glfw3 ${PROJECT_SOURCE_DIR}/../../../addons/cmx_imgui_app/src/glfw3/lib/linux)

pkg_check_modules(GTK2 REQUIRED gtk+-2.0)

# add_definitions(-Ddefine) # example def

add_definitions(-g)
# make it c++11 compatible
add_definitions(-std=c++11)

set(GLFWLIB_SEARCH_PATH /usr/lib/x86_64-linux-gnu/)
find_library(GLFW_LIB glfw PATHS ${GLFWLIB_SEARCH_PATH})

include_directories(include
	  ${PROJECT_SOURCE_DIR}/../../../src/
	  ${PROJECT_SOURCE_DIR}/../../../libs/
	  
	  ${PROJECT_SOURCE_DIR}/../../../addons/cmx_imgui_app/src
	  ${PROJECT_SOURCE_DIR}/../../../addons/cmx_params/src

	${GTK2_INCLUDE_DIRS}
	${ARMADILLO_INCLUDE_DIRS}
)

link_directories(
	${PROJECT_SOURCE_DIR}/../../../lib/
	${GTK2_LIBRARY_DIRS}
)

add_definitions(
	${GTK2_CFLAGS_OTHER}
)

add_executable(basic_example
	${PROJECT_SOURCE_DIR}/main.cpp
)

target_link_libraries(basic_example
colormotor
	${OPENGL_LIBRARIES}
	${GTK2_LIBRARIES}
	${ARMADILLO_LIBRARIES}
	${OpenCV_LIBRARIES}
	${GLEW_LIBRARIES}
		pthread
		${GLFW_LIB}
)








