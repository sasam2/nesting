Solution: Nesting

Dependencies:
- Glut (freeglut)
- Glui
- Glew

- boost 1.56
- cgal 3.9
- opencv 231/249

Tools: 
CMake 2.8.12.2
Visual Studio 2010
Git (optional)
Cygwin, with "make" "perl" and "python"(2,3,..) installed.


Build dependencies:

1. Glut (freeglut)
- Download http://freeglut.sourceforge.net/
- Generate with cmake project for VS10-Win32 
- Open generated project in Visual Studio 10, Build and Install.


2. Glui 
- Download/Clone https://github.com/libglui/glui
- Generate with cmake project for VS10-Win32
	.notes: specify path to GLUT_ROOT_PATH\lib and GLUT_ROOT_PATH\include, for these to be found.
- Open generated project in Visual Studio 10, Build.


3. Glew
- Download/Clone https://github.com/nigels-com/glew
- Run Cygwin. Install "make" "perl" and "python" if it hasn't. Do:
# cd to glew/auto/
# make
- This should create include dir (according to: https://github.com/nigels-com/glew/issues/13)
- Open glew VS project on glew\build\vc10.
- Go to Properties>Linker>Input>IgnoreAllDefaultLibraries and set it to false.
- Build.


4. Booost
- Download https://www.boost.org/doc/libs/1_56_0/more/getting_started/windows.html
- Build using boost build (explanation: http://informilabs.com/building-boost-32-bit-and-64-bit-libraries-on-windows/)
	.\bootstrap.bat
	.\b2 --build-dir=build/x86 address-model=32 threading=multi --build-type=complete --stagedir=./stage/x86 --toolset=msvc-10.0 -j 8 
	.\b2 install
	
	
5. CGAL	
- Download https://www.cgal.org/2011/09/27/cgal-39/
- Generate with cmake project for VS10-Win32
	.notes: BOOST_LIBRARYDIR and BOOST_INCLUDEDIR must be defined, pointing to the lin and include folders of the previously generated boost build.


6. OpenCV
- Go to https://opencv.org/releases, download and extract package 2.4.9.


After building dependencies, on Nesting project:

Go to properties>C/C++>Additional Include Directories and add the paths to dependencies include folders.
Go to properties>Linker>Additional Library Directories and add the path to dependencies lib folders.
Go to properties>Debugging>Environment and edit the "path" variable to include the dependencises bin folders.
