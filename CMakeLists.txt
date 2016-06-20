cmake_minimum_required(VERSION 2.8.3)
project(colormotor)

add_definitions(-g)
# make it c++11 compatible
add_definitions(-std=c++11)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(colormotor STATIC
	${PROJECT_SOURCE_DIR}/src/cm_epsfile.cpp
	${PROJECT_SOURCE_DIR}/src/cm_geom.cpp
	${PROJECT_SOURCE_DIR}/src/cm_gfx.cpp
	${PROJECT_SOURCE_DIR}/src/cm_math.cpp
	${PROJECT_SOURCE_DIR}/src/cm_utils.cpp
	
	${PROJECT_SOURCE_DIR}/src/deps/clipper/clipper.cpp
	${PROJECT_SOURCE_DIR}/src/platform/cm_utils_osx.mm

	${PROJECT_SOURCE_DIR}/addons/cmx_params/src/cm_params.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_params/src/tinyxml2/tinyxml2.cpp
		
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/cm_imgui_app.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/cm_imgui.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/gfx_ui.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui_draw.cpp
	
)

include_directories(
	  ${PROJECT_SOURCE_DIR}/src/
	  ${PROJECT_SOURCE_DIR}/libs/
	  
	  ${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src
	  ${PROJECT_SOURCE_DIR}/addons/cmx_params/src
)

if(APPLE)
	include_directories(
		/usr/local/lib/python2.7/site-packages/numpy/core/include/
		/usr/local/include
	)

	link_directories(
		/usr/local/Frameworks
		${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/glfw3/lib/osx
		/usr/local/lib
	)

	target_link_libraries(colormotor
		opencv_core
		opencv_imgproc
		opencv_legacy
		opencv_highgui
		armadillo
		lapack
		blas
		glfw3
		"-framework CoreFoundation"
		"-framework OpenGL"	
		"-framework Cocoa"
		"-framework AppKit"
		"-framework CoreVideo"
		"-framework QTKit"
		"-framework IOKit"
		"-framework AGL"
		"-framework GLUT"
	)
endif(APPLE)

if(LINUX)
	find_package(Armadillo REQUIRED)
	find_package(PkgConfig REQUIRED)
	find_package(OpenCV REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)
	pkg_check_modules(GTK2 REQUIRED gtk+-2.0)

	include_directories(
		${GTK2_INCLUDE_DIRS}
		${ARMADILLO_INCLUDE_DIRS}
	)

	link_directories(
		${GTK2_LIBRARY_DIRS}
	)

	add_definitions(
	${GTK2_CFLAGS_OTHER}
	)

	# Mess

	target_link_libraries(colormotor
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
endif(LINUX)









