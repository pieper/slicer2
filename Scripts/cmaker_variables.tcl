
#
# Note: the local vars file overrides the default values in this script
# - use it to set your local environment and then your change won't 
#   be overwritten when this file is updated
#

## variables that are the same for all systems
set SLICER_DATA_ROOT ""

## system dependent variables
switch $tcl_platform(os) {
    "SunOS" {
        set BUILD $solaris
        set SLICER_HOME /spl/tmp/nicole/slicer2
        set VTK_DIR /projects/birn/slicer2/Lib/solaris8/vtk/VTK-build-4.2.3
        set ITK_BINARY_PATH /projects/birn/itk/itk-1.2/itk-build
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles"
        set COMPILER "g++"
        set CMAKE cmake
        set MAKE "gmake -j15"
    }
    "Linux" {
        set BUILD $linux
        set SLICER_HOME /home/pieper/slicer2/latest/slicer2
        set VTK_BINARY_PATH $SLICER_HOME/Lib/$BUILD/vtk/VTK-build
        set VTK_DIR $VTK_BINARY_PATH
        set ITK_BINARY_PATH /home/pieper/downloads/itk/itk-build
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles" 
        set COMPILER "g++"
        set CMAKE cmake
        set MAKE make
    }
    "Darwin" {
        set BUILD $darwin
        set SLICER_HOME /Users/pieper/slicer2/latest/slicer2
        set ITK_BINARY_PATH /Users/pieper/downloads/itk/itk-build
        set VTK_SRC_PATH /Users/pieper/downloads/vtk/vtk4.2/VTK-4.2.1
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.dylib
        set GENERATOR "Unix Makefiles" 
        set COMPILER "c++"
        set CMAKE cmake
        set MAKE make
    }
    default {
        # different windows machines say different things, so assume
        # that if it doesn't match above it must be windows
        # (VC7 is Visual C++ 7.0, also known as the .NET version)

        set BUILD $windows
        set SLICER_HOME c:/pieper/bwh/slicer2/latest/slicer2
        set VTK_BINARY_PATH c:/downloads/vtk/VTK-4.2.5-build
        set VTK_DIR $VTK_BINARY_PATH
        set ITK_BINARY_PATH "c:/downloads/itk/InsightToolkit-1.4.0-build"
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/debug/vtkSlicerBase.lib
        set GENERATOR "Visual Studio 7" 
        set COMPILER "cl"
        set CMAKE "c:/Program Files/CMake/bin/cmake.exe"
        set MAKE make
    }
}
