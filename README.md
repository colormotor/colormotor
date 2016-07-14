# COLORMOTOR


Colormotor is a creative coding framework with a "scientific twist". The framework builds on top of Armadillo for linear algebra operations, OpenCV for image manipulation and OpenGL for hardware accellerated rendering. 

Armadillo is definitely not the fastest or most lightweight solution for a graphics-oriented framework. On the other hand, the library provides many utilities that make it easy to switch between C++, Python and Matlab, and vectorised operations result in much more concise code.

### Python Bindings
Colormotor includes [Python bindings](https://github.com/colormotor/colormotor/tree/master/addons/pycolormotor) that can be used independently as a module, or within the [Colormotor Sandbox](https://github.com/colormotor/pycm_sandbox_gl) live coding environment.  

## Dependencies
The framework depends on:

- [OpenCV](http://opencv.org)
- [Armadillo](http://arma.sourceforge.net)

#### Platform specific
#### OSX
The paths in the CMake files to build the frameworks, and the Python bindings as well, assume that the dependencies are installed with [Homebrew](http://brew.sh), and located in the */usr/local* folder.

#### Linux
The frameworkd relies on some of the [Clang language extensions](http://clang.llvm.org/docs/LanguageExtensions.html), so you will need to install clang with
```
sudo apt-get update
sudo apt-get install clang
```

## Building
### Linking the libraray
Navigate to the *colormotor* repository directory and then
```
mkdir build
cd build
cmake ..
make
```
### Building the example app
#### OSX
Build the and run the *apps/examples/basic/basic.xcodeproj* XCode project.
#### Linux 
Navigate to the *apps/examples/basic* directory and then
```
mkdir build
cd build
cmake ..
make
```
To launch the app
```
./basic_example
```


