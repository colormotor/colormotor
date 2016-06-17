# COLORMOTOR 2

Yet another iteration of the Colormotor graphics framework, principally aimed at research purposes. Builds on top of Armadillo for linear algebra operations, OpenCV for image manipulation and OpenGL for hardware accellerated rendering. 

Armadillo is definitely not a fast or lightweight solution for a graphics-oriented framework. On the other hand it provides many utilities that make it easy to switch between C++, Python and Matlab, and vectorised operations result in much more concise code.

## **Dependencies**

#### OpenCV

#### Armadillo

Download and Install the latest version of armadillo (linear algebra library), following the recommandations and instructions provided at this link:
http://arma.sourceforge.net/download.html#linux


#### glfw3

Clone and install the latest version of GLFW to a folder (not in the ros path):

```
git clone https://github.com/glfw/glfw.git```
cd glfw
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ../
make
sudo make install
```

**Notes** The `-DBUILD_SHARED_LIBS` is necessary otherwise cmake will create a static library.

At the current date this is not the case, but eventualy GLFW3 may be available on apt get with:
*sudo apt-get install libglfw3-dev*

## **Building**

### OSX
Duplicate the basic folder (*apps/examples/basic/*) and compile the xcodeproj file in Xcode.

### Linux
Duplicate the basic folder (*apps/examples/basic/*). 
Navigate to the folder and then:

```
mkdir build
cd build
cmake ..
make
```
