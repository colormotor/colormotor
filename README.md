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
TODO

## Building
```
mkdir build
cd build
cmake ..
make
```

