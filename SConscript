import os

shared = int( ARGUMENTS.get('shared', 0) )
arch = str( ARGUMENTS.get('arch','x86_64'))


if shared:
	print 'Building shared lib.'

src = []

src+= """
libs/glfw/lib/enable.c
libs/glfw/lib/fullscreen.c
libs/glfw/lib/glext.c
libs/glfw/lib/image.c
libs/glfw/lib/init.c
libs/glfw/lib/input.c
libs/glfw/lib/joystick.c
libs/glfw/lib/stream.c
libs/glfw/lib/tga.c
libs/glfw/lib/thread.c
libs/glfw/lib/time.c
libs/glfw/lib/window.c
libs/glfw/lib/cocoa/cocoa_enable.m
libs/glfw/lib/cocoa/cocoa_fullscreen.m
libs/glfw/lib/cocoa/cocoa_glext.m
libs/glfw/lib/cocoa/cocoa_init.m
libs/glfw/lib/cocoa/cocoa_joystick.m
libs/glfw/lib/cocoa/cocoa_time.m
libs/glfw/lib/cocoa/cocoa_window.m
""".split()

src += """
src/core/cmBuffer.cpp
src/core/cmFileUtils.cpp
src/core/cmMemoryMap.cpp
src/core/cmParam.cpp
src/core/cmParamList.cpp
src/core/cmRTTI.cpp
src/core/cmResource.cpp
src/core/cmThread.cpp
src/core/cmTime.cpp
src/core/cmUtils.cpp
""".split()

src += """
src/math/CMArcBall.cpp
src/math/CMPerlin.cpp
src/math/CMRay.cpp
src/math/CMTransform.cpp
src/math/cmFrustum.cpp
src/math/cmMathUtils.cpp
src/math/cmHomography.cpp
src/math/cmNumeric.cpp
""".split()

src += """
src/gfx/CMGeometryUtils.cpp
src/gfx/cmFont.cpp
src/gfx/cmGL.cpp
src/gfx/cmGeometry.cpp
src/gfx/cmGeometryDrawer.cpp
src/gfx/cmGfxUtils.cpp
src/gfx/cmIndexBuffer.cpp
src/gfx/cmQuad.cpp
src/gfx/cmRenderTarget.cpp
src/gfx/cmShader.cpp
src/gfx/cmTexture.cpp
src/gfx/cmVertexBuffer.cpp
src/gfx/cmVertexFormat.cpp
""".split()

src += """
src/geom/cvGabor/cvGabor.cpp
src/geom/clipper/clipper.cpp
src/geom/libtess2/bucketalloc.c
src/geom/libtess2/dict.c
src/geom/libtess2/geom.c
src/geom/libtess2/mesh.c
src/geom/libtess2/priorityq.c
src/geom/libtess2/sweep.c
src/geom/libtess2/tess.c
src/geom/Image.cpp
src/geom/Path2d.cpp
src/geom/PolyClipper.cpp
src/geom/Shape2d.cpp
src/geom/Triangulator.cpp
src/geom/TriMesh.cpp
src/geom/cmGeomGfx.cpp
src/geom/EpsFile.cpp
src/geom/Shape.cpp
""".split()


src += """
src/app/cmGLFWApp.cpp
src/app/CMCanvas.cpp
src/app/CMEvents.cpp
src/app/CMKeyboard.cpp
src/app/CMMouse.cpp
src/app/GLCanvas.cpp
src/app/QuickGUI.cpp
""".split()

src += """
libs/tinyxml/tinystr.cpp
libs/tinyxml/tinyxml.cpp
libs/tinyxml/tinyxmlerror.cpp
libs/tinyxml/tinyxmlparser.cpp

libs/stb/CMStbImage.cpp

""".split()

src += """
src/engine/effects/Accumulator.cpp
src/engine/effects/Blur.cpp
src/engine/effects/Downsample.cpp
src/engine/effects/Glow.cpp
src/engine/effects/ShaderEffect.cpp
src/engine/effects/Threshold.cpp
""".split()

src += """
src/engine/renderSys/AnimCache.cpp
src/engine/renderSys/Animation.cpp
src/engine/renderSys/BVHParser.cpp
src/engine/renderSys/Camera.cpp
src/engine/renderSys/DefaultMaterial.cpp
src/engine/renderSys/Material.cpp
src/engine/renderSys/Mesh.cpp
src/engine/renderSys/MeshAdjacency.cpp
src/engine/renderSys/Node.cpp
src/engine/renderSys/RenderChunk.cpp
src/engine/renderSys/RenderSys.cpp
src/engine/renderSys/Renderable.cpp
src/engine/renderSys/Renderer.cpp
src/engine/renderSys/ResourceManager.cpp
src/engine/renderSys/Scene.cpp
src/engine/renderSys/SceneLoader.cpp
src/engine/renderSys/ShadowMesh.cpp
src/engine/renderSys/Skeleton.cpp
src/engine/renderSys/SkeletonAnimInstance.cpp
src/engine/renderSys/SkeletonAnimSource.cpp
src/engine/renderSys/SkeletonAnimator.cpp
src/engine/renderSys/SkeletonAnimSystem.cpp
src/engine/renderSys/RunningSkeleton.cpp
src/engine/renderSys/TextureLines.cpp
src/engine/renderSys/utils/HalfEdgeCm.cpp
""".split()



# library paths
libPaths = """
#libs/glee/lib/osx
#libs/glfw/osx/lib
#libs/rtaudio/lib
""".split()

includePaths = """
#src
#libs/glfw/include
#libs/glfw/lib
""".split()

# modify these based on debug.

env = Environment()

debug = True #ARGUMENTS.get('debug', 1)
if debug:
	print "Debug Build"

CCFLAGS = '-ggdb -pipe -Wall -ffast-math -fPIC -pthread -stdlib=libc++ -std=c++11'
warnings = """
-Wno-backslash-newline-escape
-Wno-overloaded-virtual
-Wno-reorder
-Wno-unused
-Wno-switch
-Wno-self-assign
-Wno-#warnings
-Wno-format-security
""".split()

for w in warnings:
	CCFLAGS += ' '
	CCFLAGS += w
	

if not debug:
	CCFLAGS += ' -O3'

for f in CCFLAGS.split():
	env.Append(CXXFLAGS=f)
	

# setup clang and colored output
env['ENV']['TERM'] = os.environ['TERM']
env["CXX"] = "clang++"
env["CC"] = "clang"

env.MergeFlags(ARGUMENTS.get('CCFLAGS', '').split())
env.MergeFlags(ARGUMENTS.get('LDFLAGS', '').split())

if env['PLATFORM'] == 'win32':
	print("!!!! not supported yet")
	
if env['PLATFORM'] == 'darwin':
	# append platform specific include directories to watch
	'''
	libPaths+="""
	#libs/opencv/lib
	""".split()
	'''

	includePaths+="""
	#libs/glfw/lib/cocoa
	""".split()

	src+="""
	src/core/platform/osx/CMOSX.mm
	""".split()

	'''
	opencv_core
	opencv_imgproc
	opencv_legacy
	opencv_highgui
	'''
	
	libs = """
	rtaudio
	""".split()
	

	frameworks = """
	Carbon
	OpenGL
	Cocoa
	""".split()

	
	if debug:
		env.Append(CCFLAGS = '-g')
		
	archflag = ' -arch ' + arch
	env.Append(CCFLAGS = archflag) #i386
	env.Append(LINKFLAGS = archflag)
	env.Append(LINKFLAGS = ' -stdlib=libc++')
	env.Append(CCFLAGS = ' -mmacosx-version-min=10.8 ')

	# homebrew
	#includePaths += ['/usr/local/Cellar/opencv/2.4.6.1/include']
	#libPaths += ['/usr/local/Cellar/opencv/2.4.6.1/lib']
	includePaths += ['/usr/local/include']
	libPaths += ['/usr/local/lib']
	'''
	if False: #os.path.exists('/opt/local/lib'):
		# macports
		includePaths += ['/opt/local/include']
		libPaths += ['/opt/local/lib']
	else:
		# homebrew
		includePaths += ['/usr/local/include']
		libPaths += ['/usr/local/lib']
	'''
	
if shared:
	env.SharedLibrary(target='../lib/libcm.so',source=src,LIBS=libs,FRAMEWORKS=frameworks,LIBPATH=libPaths,CPPPATH=includePaths)
else:
	env.StaticLibrary(target='../lib/libcm.a',source=src,LIBS=libs,FRAMEWORKS=frameworks,LIBPATH=libPaths,CPPPATH=includePaths) #LIBS=libs,


