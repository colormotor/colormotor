import os

shared = int( ARGUMENTS.get('shared', 0) )
arch = str( ARGUMENTS.get('arch','x86_64'))


if shared:
	print 'Building shared lib.'

src = []

src+= """
src/app/cmGLFWApp.cpp
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
src/math/cmArcBall.cpp
src/math/cmPerlin.cpp
src/math/cmRay.cpp
src/math/cmTransform.cpp
src/math/cmFrustum.cpp
src/math/cmMathUtils.cpp
src/math/cmHomography.cpp
src/math/cmNumeric.cpp
""".split()

src += """
src/gfx/cmGeometryUtils.cpp
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
src/app/cmCanvas.cpp
src/app/cmEvents.cpp
src/app/cmKeyboard.cpp
src/app/cmMouse.cpp
src/app/GLCanvas.cpp
src/app/QuickGUI.cpp
""".split()

src += """
libs/tinyxml/tinystr.cpp
libs/tinyxml/tinyxml.cpp
libs/tinyxml/tinyxmlerror.cpp
libs/tinyxml/tinyxmlparser.cpp

libs/stb/cmStbImage.cpp

""".split()




includePaths = """
#src
#libs/glfw3/include
""".split()

# modify these based on debug.

env = Environment(ENV = {'PATH' : os.environ['PATH'],
                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME']})

debug = True #ARGUMENTS.get('debug', 1)
if debug:
	print "Debug Build"

CCFLAGS = '-ggdb -pipe -Wall -ffast-math -fPIC -pthread -std=c++11'
warnings = """
-Wno-backslash-newline-escape
-Wno-overloaded-virtual
-Wno-reorder
-Wno-unused
-Wno-switch
-Wno-self-assign
-Wno-#warnings
-Wno-format-security
-Wno-sign-compare
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


env.MergeFlags(ARGUMENTS.get('CCFLAGS', '').split())
env.MergeFlags(ARGUMENTS.get('LDFLAGS', '').split())

if env['PLATFORM'] == 'win32':
	print("windows not supported yet")
	
elif env['PLATFORM'] == 'darwin':
	print "Building OSX variant"
	env["CXX"] = "clang++"
	env["CC"] = "clang"

	# append platform specific include directories to watch
	'''
	libPaths+="""
	#libs/opencv/lib
	""".split()
	'''

	# library paths
	libPaths = """
	#libs/glee/lib/osx
	#libs/glfw3/lib/osx
	""".split()

	includePaths+="""
	#libs/glfw/lib/cocoa
	""".split()

	src+="""
	src/core/platform/osx/cmOSX.mm
	""".split()

	frameworks = """
	Carbon
	OpenGL
	Cocoa
	""".split()

	libs = None
	
	if debug:
		env.Append(CCFLAGS = '-g')
		
	archflag = ' -arch ' + arch
	env.Append(CCFLAGS = archflag) #i386
	env.Append(LINKFLAGS = archflag)
	env.Append(LINKFLAGS = ' -stdlib=libc++')
	env.Append(CCFLAGS = '-stdlib=libc++')
	env.Append(CCFLAGS = ' -mmacosx-version-min=10.8 ')
	env.Append(CPPDEFINES=['__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0'])
	env.Append(LINKFLAGS=['-Wl,--whole-archive','-lglfw3','-Wl,--no-whole-archive']) 

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


else:
	# Assume posix
	print 'Building posix variant'

	# library paths
	libPaths = """
	#libs/glee/lib/osx
	#libs/glfw3/lib/linux
	""".split()

	includePaths+="""
	#libs/glfw/lib/cocoa
	""".split()

	src+="""
	src/core/platform/linux/cmLinux.cpp
	""".split()

	libs = None

	if debug:
		env.Append(CCFLAGS = '-g')
		
	#archflag = ' -arch ' + arch
	#env.Append(CCFLAGS = archflag) #i386

	#env.Append(LINKFLAGS = archflag)
	#env.Append(LINKFLAGS = ' -stdlib=libc++')
	env.Append(LINKFLAGS=['-Wl,--whole-archive','-lglfw3','-Wl,--no-whole-archive']) 

	env.ParseConfig('pkg-config --cflags --libs gtk+-2.0 opencv glew glfw3')

	# homebrew
	#includePaths += ['/usr/local/Cellar/opencv/2.4.6.1/include']
	#libPaths += ['/usr/local/Cellar/opencv/2.4.6.1/lib']
	includePaths += ['/usr/local/include']
	libPaths += ['/usr/local/lib']
	includePaths += ['/usr/include']
	libPaths += ['/usr/lib']

	env.Append(CPPPATH=includePaths)
	env.Append(LIBPATH=libPaths)
	if libs:
		env.Append(LIBS=libs)

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
		env.SharedLibrary(target='../lib/libcm.so',source=src)
	else:
		env.StaticLibrary(target='../lib/libcm.a',source=src) #LIBS=libs,




