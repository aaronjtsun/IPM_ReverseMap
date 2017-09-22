1. If not installed in machine already, install CMake and OpenCV by following steps online
	For Linux (Ubuntu 16.04): http://www.pyimagesearch.com/2016/10/24/ubuntu-16-04-how-to-install-opencv/
	For Windows: http://docs.opencv.org/3.2.0/d3/d52/tutorial_windows_install.html

2. Navigate to this directory using specified terminal

3. Compile code using CMake
	For Linux: Run 'cmake .' to output CMakeLists.txt to Makefile, then run 'make' to compile into executable	
	For Windows: https://cmake.org/runningcmake/
***DO NOT DELETE IPM.cpp OR CMakeLists.txt OR PROGRAM WILL NOT BE ABLE TO COMPILE***

4. Run executable 'ipm' with argument 2 as the path to an image file relative to this directory
	In both terminal/command line:
		Example 1: './ipm example1.jpg'
		Example 2: './ipm ~/example2.jpg'
