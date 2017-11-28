cmake_minimum_required(VERSION 2.8.3)
if(APPLE)
	set(CMAKE_MACOSX_RPATH 0)
endif()

project(pycm)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

add_definitions(-g)
# make it c++11 compatible
add_definitions(-std=c++11)

add_definitions(-DSWIG_TYPE_TABLE=colormotor)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/modules)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/modules)

set(src
	${PROJECT_SOURCE_DIR}/cm_wrap.cxx
	${PROJECT_SOURCE_DIR}/../../src/cm_epsfile.cpp
	${PROJECT_SOURCE_DIR}/../../src/cm_geom.cpp
	${PROJECT_SOURCE_DIR}/../../src/cm_gfx.cpp
	${PROJECT_SOURCE_DIR}/../../src/cm_math.cpp
	${PROJECT_SOURCE_DIR}/../../src/cm_utils.cpp
	
	${PROJECT_SOURCE_DIR}/../../src/deps/clipper/clipper.cpp

	${PROJECT_SOURCE_DIR}/../cmx_params/src/cm_params.cpp
	${PROJECT_SOURCE_DIR}/../cmx_params/src/tinyxml2/tinyxml2.cpp
		
	${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/cm_imgui_app.cpp
	${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/cm_imgui.cpp
	${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/imgui.cpp
	${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/gfx_ui.cpp
	${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/imgui_draw.cpp
)

if(APPLE)
	set(src
		${src}
		${PROJECT_SOURCE_DIR}/../../src/platform/cm_utils_osx.mm
	)
elseif(UNIX)
	message ("LINUXXXX")
	set(src
		${src}
		${PROJECT_SOURCE_DIR}/../../src/platform/cm_utils_linux.cpp
	)
endif()

add_library(pycm SHARED
	${src}
)

include_directories(
	  ${PROJECT_SOURCE_DIR}/../../src/
	  ${PROJECT_SOURCE_DIR}/../../libs/
	  
	  ${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src
	  ${PROJECT_SOURCE_DIR}/../cmx_params/src
)

if(APPLE)
	include_directories(
		/usr/local/Frameworks/Python.framework/Headers
		/usr/local/lib/python2.7/site-packages/numpy/core/include/
		/usr/local/include
	)

	link_directories(
		/usr/local/Frameworks
		${PROJECT_SOURCE_DIR}/../cmx_imgui_app/src/glfw3/lib/osx
		/usr/local/lib
	)

	target_link_libraries(pycm
		opencv_core
		opencv_imgproc
		opencv_legacy
		opencv_highgui
		armadillo
		lapack
		blas
		glfw3
		/usr/local/Frameworks/Python.framework
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

	add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND rm -f ${PROJECT_SOURCE_DIR}/modules/_cm.so 
    COMMAND ln -s ${PROJECT_SOURCE_DIR}/modules/libpycm.dylib ${PROJECT_SOURCE_DIR}/modules/_cm.so 
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

	#pkg_search_module(PYTHON REQUIRED python>=2.6)
	# Nifty trick from http://stackoverflow.com/questions/24174394/cmake-is-not-able-to-find-python-libraries

	execute_process(COMMAND python -c "import sys; from distutils.sysconfig import get_python_inc; sys.stdout.write(get_python_inc())" OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS)

	execute_process(COMMAND python -c "import sys; import distutils.sysconfig as sysconfig; sys.stdout.write(sysconfig.get_config_var('LIBDIR'))" OUTPUT_VARIABLE PYTHON_LIBRARY_DIRS)

	execute_process(COMMAND python -c "import sys; import distutils.sysconfig as sysconfig; sys.stdout.write(sysconfig.get_config_var('LDLIBRARY'))" OUTPUT_VARIABLE PYTHON_LIBRARIES)

	message('Python lib dir')
	message(${PYTHON_LIBRARY_DIRS})
	message('Python lib')
	message(${PYTHON_LIBRARY_DIRS}/${PYTHON_LIBRARIES})

	include_directories(
		${GTK2_INCLUDE_DIRS}
		${ARMADILLO_INCLUDE_DIRS}
		${PYTHON_INCLUDE_DIRS}
		${PYTHON_LIBRARY_DIRS}/python2.7/dist-packages/numpy/core/include/
	)

	link_directories(
		${GTK2_LIBRARY_DIRS}
		${PYTHON_LIBRARY_DIRS}
	)

	add_definitions(
	${GTK2_CFLAGS_OTHER}
	)

	# Mess
	# Using PYTHON LIB DIR FOR ANACONDA COMPAT
	target_link_libraries(pycm
	glfw
	${PYTHON_LIBRARY_DIRS}/libopencv_core.so
	${PYTHON_LIBRARY_DIRS}/libopencv_imgproc.so
	${PYTHON_LIBRARY_DIRS}/libopencv_legacy.so
	${PYTHON_LIBRARY_DIRS}/libopencv_highgui.so
	${OPENGL_LIBRARIES}
	${GTK2_LIBRARIES}
	${ARMADILLO_LIBRARIES}
	${GLEW_LIBRARIES}
	${PYTHON_LIBRARY_DIRS}/${PYTHON_LIBRARIES}
			pthread
			${GLFW_LIB}
	)

	add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND rm -f ${PROJECT_SOURCE_DIR}/modules/_cm.so 
    COMMAND ln -s ${PROJECT_SOURCE_DIR}/modules/libpycm.so ${PROJECT_SOURCE_DIR}/modules/_cm.so 
	)
endif()










