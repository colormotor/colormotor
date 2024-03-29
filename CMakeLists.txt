cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(colormotor)

add_definitions(-g)
# make it c++11 compatible
add_definitions(-std=c++11)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(src
	${PROJECT_SOURCE_DIR}/src/cm_epsfile.cpp
	${PROJECT_SOURCE_DIR}/src/cm_geom.cpp
	${PROJECT_SOURCE_DIR}/src/cm_gfx.cpp
	${PROJECT_SOURCE_DIR}/src/cm_math.cpp
	${PROJECT_SOURCE_DIR}/src/cm_utils.cpp
	${PROJECT_SOURCE_DIR}/src/cm_vector_export.cpp
	
	${PROJECT_SOURCE_DIR}/src/deps/clipper/clipper.cpp
	
	${PROJECT_SOURCE_DIR}/addons/cmx_params/src/cm_params.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_params/src/tinyxml2/tinyxml2.cpp
		
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/cm_imgui_app.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/cm_imgui.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/gfx_ui.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src/imgui_widgets.cpp
)

include_directories(
	  ${PROJECT_SOURCE_DIR}/src/
	  ${PROJECT_SOURCE_DIR}/libs/
	  
	  ${PROJECT_SOURCE_DIR}/addons/cmx_imgui_app/src
	  ${PROJECT_SOURCE_DIR}/addons/cmx_params/src
)


if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shorten-64-to-32 -Wno-deprecated-declarations")
	set(src
		${src}
		${PROJECT_SOURCE_DIR}/src/platform/cm_utils_osx.mm
	)
elseif(UNIX)
	set(src
		${src}
		${PROJECT_SOURCE_DIR}/src/platform/cm_utils_linux.cpp
	)
endif()


add_library(colormotor STATIC
	${src}
)

if(APPLE)
	# Change this to your anaconda path, wander if there is an automatic way to handle this
	set(BASE_SEARCH_DIR $ENV{HOME}/miniconda3)

	include_directories(
		${BASE_SEARCH_DIR}/lib/python2.7/site-packages/numpy/core/include/
		${BASE_SEARCH_DIR}/include
		${BASE_SEARCH_DIR}/include/opencv4
	 )

	link_directories(
		${BASE_SEARCH_DIR}/lib
		/usr/local/Frameworks
	)
	
	target_link_libraries(colormotor
		opencv_core
		opencv_imgproc
		opencv_highgui
		opencv_imgcodecs
		opencv_videoio
		armadillo
		LAPACK
		blas
		glfw
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
elseif(UNIX)
	
	find_package(Armadillo REQUIRED)
	find_package(PkgConfig REQUIRED)
	find_package(OpenCV REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)
	pkg_check_modules(GTK2 REQUIRED gtk+-2.0)

	set(GLFWLIB_SEARCH_PATH /usr/lib/x86_64-linux-gnu/)
	find_library(GLFW_LIB glfw PATHS ${GLFWLIB_SEARCH_PATH})

	include_directories(
			${GTK2_INCLUDE_DIRS}
			${ARMADILLO_INCLUDE_DIRS}
			${OpenCV_INCLUDE_DIRS}
	)

	link_directories(
		${GTK2_LIBRARY_DIRS}
	)

	add_definitions(
	${GTK2_CFLAGS_OTHER}
	)

	# Mess

	target_link_libraries(colormotor
			${GLFW_LIB}
			${OPENGL_LIBRARIES}
			${GTK2_LIBRARIES}
			${ARMADILLO_LIBRARIES}
			${OpenCV_LIBRARIES}
			${GLEW_LIBRARIES}
	)
endif()









