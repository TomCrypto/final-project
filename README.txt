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

Instructions
------------

    To build and run the program, please use the provided Makefile. A solution
    file Project.sln is also available for Visual Studio (>= 2012) on Windows.

    The compiled program will be generated in the `bin` folder and can then be
    run, note that the working directory of the program is the `bin` folder so
    for Windows double-clicking will work, for Linux please cd into the folder
    before executing the program, e.g. as follows:

        $ make
        $ cd bin && ./Project
        $ cd ..

Folder Contents
---------------

    The different folders and their respective purpose are enumerated below:

        - `bin`: output folder for the final executable
        - `doc`: supporting documentation (paperwork, SIGGRAPH papers)
        - `lib`: third-party libraries, excluding OpenGL-related libs
        - `res`: assets used in our project (textures, models, etc.)
        - `src`: C/C++ source code and headers

External Links
--------------

http://freeimage.sourceforge.net/
http://www.fftw.org/
http://anttweakbar.sourceforge.net/
http://glew.sourceforge.net/
http://glm.g-truc.net/0.9.5/index.html
http://freeglut.sourceforge.net/

http://resources.mpi-inf.mpg.de/lensflareRendering/
https://graphics.tudelft.nl/Publications-new/2013/LE13/PracticalReal-TimeLens.pdf
http://www.graphics.stanford.edu/papers/nightsky/
http://www.cs.utah.edu/~shirley/papers/sunsky/
http://escholarship.org/uc/item/5n07m4p6
