# PyrLK-QMLTracker
An object tracker using the pyramidal Lucas-Kanade algorithm.

## Preview
<p align="center">
<img src="preview/preview1.gif">
</p>

## Build environment
* macOS Sierra (10.12.6)
* Qt 5.12.3 Clang 10.0
* OpenCV 3.1.0_4

## Steps to build
* Install OpenCV
* Update paths in pro file following installed paths of OpenCV
* Add new build environment in the projects tab 
  * Variable : PKG_CONFIG_PATH
  * Value : your/path/opencv3/3.1.0_4/pkgconfig
