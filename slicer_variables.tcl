
#
# Note: this local vars file overrides sets the default environment for :
#   Scripts/genlib.tcl -- make all the support libs
#   Scripts/cmaker.tcl -- makes slicer code
#   launch.tcl -- sets up the runtime env and starts slicer
#   Scripts/tarup.tcl -- makes a tar.gz files with all the support files
#
# - use this file to set your local environment and then your change won't 
#   be overwritten when those files are updated through CVS
#


## variables that are the same for all systems
set ::SLICER_DATA_ROOT ""

if {[info exists ::env(SLICER_HOME)]} {
    # already set by the launcher
    set ::SLICER_HOME $::env(SLICER_HOME)
} else {
    # if sourcing this into cmaker, SLICER_HOME may not be set
    # set the SLICER_HOME directory to the one in which this script resides
    set cwd [pwd]
    cd [file dirname [info script]]
    set ::SLICER_HOME [pwd]
    cd $cwd
}

# set up variables for the OS Builds, to facilitate the move to solaris9
# - solaris can be solaris8 or solaris9
set solaris "solaris8"
set linux "linux-x86"
set darwin "darwin-ppc"
set windows "win32"
#
# set the default locations for the main components
#
switch $tcl_platform(os) {
    "SunOS" { set ::env(BUILD) $solaris }
    "Linux" { set ::env(BUILD) $linux }
    "Darwin" { set ::env(BUILD) $darwin }
    default { 
        set ::env(BUILD) $windows 
        set ::SLICER_HOME [file attributes $::SLICER_HOME -shortname]
        set ::env(SLICER_HOME) $::SLICER_HOME
    }
}

puts "SLICER_HOME is $::SLICER_HOME"

set ::SLICER_LIB $SLICER_HOME/Lib/$::env(BUILD)
set ::VTK_DIR  $::SLICER_LIB/VTK-build
set ::VTK_SRC_DIR $::SLICER_LIB/VTK
set ::VTK_BUILD_TYPE ""
set ::env(VTK_BUILD_TYPE) $::VTK_BUILD_TYPE
set ::ITK_BINARY_PATH $::SLICER_LIB/Insight-build
set ::TCL_BIN_DIR $::SLICER_LIB/tcl-build/bin
set ::TCL_LIB_DIR $::SLICER_LIB/tcl-build/lib
set ::TCL_INCLUDE_DIR $::SLICER_LIB/tcl-build/include
set ::CMAKE_PATH $::SLICER_LIB/CMake-build
set ::GSL_LIB_DIR $::SLICER_LIB/gsl/lib
set ::GSL_INC_DIR $::SLICER_LIB/gsl/include
set ::GSL_SRC_DIR $::SLICER_LIB/gsl-build/gsl

## system dependent variables

switch $tcl_platform(os) {
    "SunOS" {
        set ::VTKSLICERBASE_BUILD_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/vtkSlicerBase.so
        set ::VTKSLICERBASE_BUILD_TCL_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/vtkSlicerBaseTCL.so
        set ::GENERATOR "Unix Makefiles"
        set ::COMPILER_PATH "/local/os/bin"
        set ::COMPILER "g++"
        set ::CMAKE $::CMAKE_PATH/bin/cmake
        set ::MAKE "gmake -j15" 
    }
    "Linux" {
        set ::VTKSLICERBASE_BUILD_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/vtkSlicerBase.so
        set ::VTKSLICERBASE_BUILD_TCL_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/vtkSlicerBaseTCL.so
        set ::GENERATOR "Unix Makefiles" 
        set ::COMPILER_PATH "/usr/bin"
        set ::COMPILER "g++"
        set ::CMAKE $::CMAKE_PATH/bin/cmake
        set ::MAKE make
    }
    "Darwin" {
        set ::VTKSLICERBASE_BUILD_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/vtkSlicerBase.dylib
        set ::VTKSLICERBASE_BUILD_TCL_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/vtkSlicerBaseTCL.dylib
        set ::GENERATOR "Unix Makefiles" 
        set ::COMPILER_PATH "/usr/bin"
        set ::COMPILER "c++"
        set ::CMAKE $::CMAKE_PATH/bin/cmake
        set ::MAKE make
    }
    default {
        # different windows machines say different things, so assume
        # that if it doesn't match above it must be windows
        # (VC7 is Visual C++ 7.0, also known as the .NET version)

        #
        ### Set your peferred build type: 
        #

        #set ::VTK_BUILD_TYPE RelWithDebInfo ;# good if you have the full (expensive) compiler
        #set ::VTK_BUILD_TYPE Release  ;# faster, but no debugging
        set ::VTK_BUILD_TYPE Debug  ;# a good default

        set ::env(VTK_BUILD_TYPE) $VTK_BUILD_TYPE

        set ::VTKSLICERBASE_BUILD_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/$::VTK_BUILD_TYPE/vtkSlicerBase.lib
        set ::VTKSLICERBASE_BUILD_TCL_LIB $::SLICER_HOME/Base/builds/$::env(BUILD)/bin/$::VTK_BUILD_TYPE/vtkSlicerBaseTCL.lib

        set ::CMAKE $::CMAKE_PATH/bin/cmake.exe

        #
        ## match this to the version of the compiler you have:
        #

        ## for Visual Studio 7:
        set ::GENERATOR "Visual Studio 7" 
        set ::MAKE "c:/Program\ Files/Microsoft\ Visual\ Studio\ .NET/Common7/IDE/devenv.exe"
        set ::COMPILER_PATH "c:/Program\ Files/Microsoft\ Visual\ Studio\ .NET/Common7/Vc7/bin"

        ## for Visual Studio 7.1:
        #set ::GENERATOR "Visual Studio 7 .NET 2003" 
        #set ::MAKE "c:/Program\ Files/Microsoft\ Visual\ Studio\ .NET 2003/Common7/IDE/devenv.exe"
        #set ::MAKE "c:/Program\ Files/Microsoft\ Visual\ Studio\ .NET 2003/Common7/Vc7/bin"

        set ::COMPILER "cl"

    }
}


