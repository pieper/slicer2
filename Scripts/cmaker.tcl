#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

# 
# usage: cmaker.tcl [list of modules]
# by default, run cmake and make on each of the slicer modules
# but you can override the default and only run on specified modules
#
# make slicerbase and all the modules using the same configurations
# - sp 2003-01-25
#
# This is a utility script to help manage the combination of many Modules
# needing to be built on many platforms.  To use this, edit the variables
# at the top level to reflect your local configuration.  You will need to have
# a version of vtk built using the same compiler options picked here.  In general,
# much of the state will be inherited from the vtk build (e.g. where tcl is
# installed).  
#
# When you run this, build errors may not be easy to debug -- if there are errors,
# go to the appropriate build directory and run make or dev studio by hand
# and get the build working from there.
#

switch $tcl_platform(os) {
    "SunOS" {
        set SLICER_HOME /projects/birn/slicer2/latest/slicer2
        set VTK_BINARY_PATH /projects/birn/slicer2/Lib/solaris8/vtk/VTK-build
        set ITK_BINARY_PATH ""
        set BUILD solaris8
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles"
        set COMPILER "g++"
        set CMAKE cmake
    }
    "Linux" {
        set SLICER_HOME /home/pieper/slicer2/latest/slicer2
        set VTK_BINARY_PATH /home/pieper/vtk/latest/VTK-build
        set ITK_BINARY_PATH ""
        set BUILD redhat7.3
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles" 
        set COMPILER "g++"
        set CMAKE cmake
    }
    "Darwin" {
        set SLICER_HOME /Users/pieper/slicer2/latest/slicer2
        set VTK_BINARY_PATH /Users/pieper/downloads/vtk/vtk4.2/VTK-4.2.1-build
        set ITK_BINARY_PATH /Users/pieper/downloads/itk/itk-build
        set VTK_SRC_PATH /Users/pieper/downloads/vtk/vtk4.2/VTK-4.2.1
        set BUILD Darwin
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.dylib
        set GENERATOR "Unix Makefiles" 
        set COMPILER "c++"
        set CMAKE cmake
    }
    default {
        # different windows machines say different things, so assume
        # that if it doesn't match above it must be windows
        # (VC7 is Visual C++ 7.0, also known as the .NET version)

        set SLICER_HOME c:/pieper/bwh/slicer2/latest/slicer2
        set VTK_BINARY_PATH c:/downloads/vtk/latest/Win32VC7
        set ITK_BINARY_PATH ""
        set BUILD Win32VC7
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/debug/vtkSlicerBase.lib
        set GENERATOR "Visual Studio 7" 
        set COMPILER "cl"
        set CMAKE "c:/Program Files/CMake/bin/cmake.exe"
    }
}



#
# Add any modules here, as found in the Modules/vtk* directories of SLICER_HOME
# or in the SLICER_MODULES environment variable
#
regsub -all {\\} $SLICER_HOME / slicer_home
set baseModulePath $slicer_home/Modules
set modulePaths [glob -nocomplain $baseModulePath/vtk*]
if { [info exists env(SLICER_MODULES)] } {
    foreach dir $env(SLICER_MODULES) {
        eval lappend modulePaths [glob -nocomplain $dir/vtk*]
    }
}


set TARGETS "$slicer_home/Base"

foreach dir $modulePaths {
    if { ![file isdirectory $dir] } {continue}
    set moduleName [file tail $dir]
    # if it's not the custom one (the example) but starts with vtk, append it to the list of targets
    if { [string match "vtk*" $moduleName] && ![string match "*CustomModule*" $moduleName] } {
        lappend TARGETS $dir
    }
}

#
# by default, all modules are built.  If some are listed on commandline, only
# those are built.
#
if { $argv != "" } {
    set newtargets ""
    foreach argmodule $argv {
        set idx [lsearch -glob $TARGETS *$argmodule]
        if { $idx == -1 } {
            puts stderr "can't find module $argmodule in search path (options are: $TARGETS)"
            exit
        } else {
            lappend newtargets [lindex $TARGETS $idx]
        }
    }
    set TARGETS $newtargets
}

puts "Dirs to make: "
foreach t $TARGETS {
    puts $t
}
puts ""

# use an already built version of vtk
set VTK_ARG1 "-DUSE_BUILT_VTK:BOOL=ON"
set VTK_ARG2 "-DVTK_BINARY_PATH:PATH=$VTK_BINARY_PATH"
switch $tcl_platform(os) {
    "SunOS" {
        # in order to bypass warnings about Source files
        #set VTK_ARG3 "-DCMAKE_BACKWARDS_COMPATIBILITY:STRING=1.7"
        set VTK_ARG3 "-DDUMMY:BOOL=ON"
        # explicitly specify the compiler used to compile the version of vtk that 
        # we link with
        set VTK_ARG4 "-DCMAKE_CXX_COMPILER:STRING=$COMPILER"
        set VTK_ARG5 "-DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=/local/os/bin/$COMPILER"
    }
    "Darwin" {
        set VTK_ARG3 "-DVTK_WRAP_HINTS:FILEPATH=$VTK_SRC_PATH/Wrapping/hints"
        set VTK_ARG4 "-DDUMMY:BOOL=ON"
        set VTK_ARG5 "-DDUMMY:BOOL=ON"
    }
    default {
        set VTK_ARG3 "-DDUMMY:BOOL=ON"
        set VTK_ARG4 "-DDUMMY:BOOL=ON"
        set VTK_ARG5 "-DDUMMY:BOOL=ON"
    }
}
# make sure to generate shared libraries
set VTK_ARG6 "-DBUILD_SHARED_LIBS:BOOL=ON"
if { $ITK_BINARY_PATH != "" } {
    set VTK_ARG7 "-DITK_DIR:FILEPATH=$ITK_BINARY_PATH"
} else {
    set VTK_ARG5 "-DDUMMY:BOOL=ON"
}

set SLICER_ARG1 "-DVTKSLICERBASE_SOURCE_DIR:PATH=$SLICER_HOME/Base"
set SLICER_ARG2 "-DVTKSLICERBASE_BUILD_DIR:PATH=$SLICER_HOME/Base/builds/$BUILD"
set SLICER_ARG3 "-DVTKSLICERBASE_BUILD_LIB:PATH=$VTKSLICERBASE_BUILD_LIB"


foreach target $TARGETS {

    puts "\n----\nprocessing $target..."

    #set build $SLICER_HOME/$target/builds/$BUILD 
    set build $target/builds/$BUILD 
    catch "file mkdir $build"
    cd $build
    puts "enter directory $build..."

    puts "running cmake ..."
    puts "$CMAKE $target -G$GENERATOR \
        $VTK_ARG1 $VTK_ARG2 $VTK_ARG3 $VTK_ARG4 $VTK_ARG5 $VTK_ARG6 $VTK_ARG7 \
        $SLICER_ARG1 $SLICER_ARG2 $SLICER_ARG3"
    exec $CMAKE $target -G$GENERATOR \
        $VTK_ARG1 $VTK_ARG2 $VTK_ARG3 $VTK_ARG4 $VTK_ARG5 $VTK_ARG6 $VTK_ARG7 \
        $SLICER_ARG1 $SLICER_ARG2 $SLICER_ARG3

    switch $tcl_platform(os) {
        "SunOS" -
        "Darwin" -
        "Linux" {
            puts "running: make"

            # print the results line by line to provide feedback during long builds
            set fp [open "| make |& cat" "r"]
            while { ![eof $fp] } {
                gets $fp line
                puts $line
            }
            close $fp
        }
        default {
            if { [file tail $target] == "Base" } {
                set sln VTKSLICER.sln
            } else {
                set sln [string toupper [file tail $target]].sln
            }
            puts "running: devenv $sln /build debug"

            # no output from devenv so just go ahead and run it with no loop
            exec devenv $sln /build debug
        }
    }
}


exit 0

