cmake_minimum_required(VERSION 2.8.3)
project(basic_example)

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

include_directories(include
	  ${PROJECT_SOURCE_DIR}/../../../src/
	  ${PROJECT_SOURCE_DIR}/../../../libs/
	  
	  ${PROJECT_SOURCE_DIR}/../../../addons/cmx_imgui_app/src
	  ${PROJECT_SOURCE_DIR}/../../../addons/cmx_params/src

	${GTK2_INCLUDE_DIRS}
	${ARMADILLO_INCLUDE_DIRS}
)

link_directories(
	${GTK2_LIBRARY_DIRS}
)

add_definitions(
	${GTK2_CFLAGS_OTHER}
)

add_executable(basic_example
	${PROJECT_SOURCE_DIR}/main.cpp
	
	${PROJECT_SOURCE_DIR}/../../../src/cm_epsfile.cpp
	${PROJECT_SOURCE_DIR}/../../../src/cm_geom.cpp
	${PROJECT_SOURCE_DIR}/../../../src/cm_gfx.cpp
	${PROJECT_SOURCE_DIR}/../../../src/cm_math.cpp
	${PROJECT_SOURCE_DIR}/../../../src/cm_utils.cpp
	
	${PROJECT_SOURCE_DIR}/../../../src/deps/clipper/clipper.cpp
	${PROJECT_SOURCE_DIR}/../../../src/platform/cm_utils_linux.cpp

	${PROJECT_SOURCE_DIR}/../../../addons/cmx_params/src/cm_params.cpp
	${PROJECT_SOURCE_DIR}/../../../addons/cmx_params/src/tinyxml2/tinyxml2.cpp
		
	${PROJECT_SOURCE_DIR}/../../../addons/cmx_imgui_app/src/cm_imgui_app.cpp
	${PROJECT_SOURCE_DIR}/../../../addons/cmx_imgui_app/src/cm_imgui.cpp
	${PROJECT_SOURCE_DIR}/../../../addons/cmx_imgui_app/src/imgui.cpp
)

target_link_libraries(basic_example
rt
m
dl
Xrandr
Xinerama
Xxf86vm
Xcursor
Xext
Xrender
Xfixes
X11
pthread
xcb
Xau
Xdmcp
glfw

	${OPENGL_LIBRARIES}
	${GTK2_LIBRARIES}
	${ARMADILLO_LIBRARIES}
	${OpenCV_LIBRARIES}
	${GLEW_LIBRARIES}
)








