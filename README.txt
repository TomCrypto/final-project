                        ==============================
                        COMP308 2014T2 - Final Project
                        ==============================

Joseph Zachariah Sim (300263896)                           simjose@myvuw.ac.nz
Thomas Beneteau (300250968)                             benetethom@myvuw.ac.nz

This is our final project for COMP308, which consists of rendering a realistic
sky and convincing lens flares using physically-based techniques, efficiently.

We used C++ with OpenGL, and used a few libraries to help us implement various
parts of the project - notably, FreeImage, for fast and flexible image loading
and simple image processing, FFTW for calculating two-dimensional fast Fourier
transforms (an important part of the lens flare pipeline) and AntTweakBar, for
the simple GUI widget used to interact with our program.

The code is written in standard C++11 and should run on Visual Studio 2012 and
later, g++, clang++, and the Intel C++ compiler on Linux. OpenGL 3.0 required.

Instructions
------------

    To build and run the program, please use the provided Makefile. A solution
    file Project.sln is also available for Visual Studio (>= 2012) on Windows.

    The compiled program will be generated in the `bin` folder and can then be
    run, note that the working directory of the program is the `bin` folder so
    for Windows double-clicking will work, for Linux please cd into the folder
    before executing the program, e.g. as follows:

        $ make deps        # if dependencies are not installed
        $ make
        $ cd bin && ./Project
        $ cd ..

    IMPORTANT: if GLEW fails to build, just ignore the error and run make. The
    university computers should already have it installed by default anyway.

    To navigate the demo, drag the cursor using the left mouse button or right
    click then move the mouse around, and use WASD to move the camera. The 'c'
    key makes the camera go downwards, space makes it go upwards - hitting the
    escape key will close the project - and the various program parameters are
    available for modification in the graphical user interface (tweak bar).

Folder Contents
---------------

    The different folders and their respective purpose are enumerated below:

        - `bin`: output folder for the final executable
        - `doc`: supporting documentation (paperwork, SIGGRAPH papers)
        - `lib`: third-party libraries, excluding OpenGL-related libs
        - `res`: assets used in our project (textures, models, etc.)
        - `src`: C/C++ source code and headers

    The `src/core` folder contains the actual implementations of the different
    rendering subsystems. The `src/gui` folder contains only source pertaining
    to the user interface, and `src/utils` general purpose utilities. Note the
    different vertex and fragment shaders are in the `res/shaders` folder.

External Links
--------------

http://glm.g-truc.net/0.9.5/index.html
http://anttweakbar.sourceforge.net/
http://freeimage.sourceforge.net/
http://freeglut.sourceforge.net/
http://glew.sourceforge.net/
http://www.fftw.org/
