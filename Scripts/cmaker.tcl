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

# set up variables for the OS Builds, to facilitate the move to solaris9
# can be solaris8 or solaris9
set solaris "solaris8"
set linux "redhat7.3"
set darwin "Darwin"
set windows "Win32VC7"

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
        set ITK_BINARY_PATH /home/pieper/downloads/itk/itk-1.4-build
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
        set VTK_DIR c:/downloads/vtk/build
        #set VTK_DIR $SLICER_HOME/Lib/$BUILD/vtk/VTK-4.2.2-build
        set ITK_BINARY_PATH "c:/downloads/itk/itk-build"
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/debug/vtkSlicerBase.lib
        set GENERATOR "Visual Studio 7" 
        set COMPILER "cl"
        set CMAKE "c:/Program Files/CMake/bin/cmake.exe"
        set MAKE make
    }
}

# use an already built version of vtk
set VTK_ARG1 "-DUSE_BUILT_VTK:BOOL=ON"
set VTK_ARG2 "-DVTK_DIR:PATH=$VTK_DIR"
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
    set VTK_ARG7 "-DDUMMY:BOOL=ON"
}

set SLICER_ARG1 "-DVTKSLICERBASE_SOURCE_DIR:PATH=$SLICER_HOME/Base"
set SLICER_ARG2 "-DVTKSLICERBASE_BUILD_DIR:PATH=$SLICER_HOME/Base/builds/$BUILD"
set SLICER_ARG3 "-DVTKSLICERBASE_BUILD_LIB:PATH=$VTKSLICERBASE_BUILD_LIB"

#
# ----------------------------- Shouldn't need to edit anything below here...
#

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
set CLEANFLAG 0

foreach dir $modulePaths {
    if { ![file isdirectory $dir] } {continue}
    set moduleName [file tail $dir]
    # if it's not the custom one (the example) 
    # but starts with vtk and has some c++ code, append it to the list of targets
    if { [string match "vtk*" $moduleName] 
            && ![string match "*CustomModule*" $moduleName] 
            && [file exists $dir/cxx] } {
        lappend TARGETS $dir
    }
}

#
# by default, all modules are built.  If some are listed on commandline, only
# those are built.
#
set cleanarg [lsearch $argv "--clean"] 
if {$cleanarg != -1} {
    puts "Removing build directories"
    set CLEANFLAG 1
    # remove flag from the list
    set argv [lreplace $argv $cleanarg $cleanarg]
    if {[llength $argv] == 0} {
        set argv ""
    }
}
if { $argv != "" && $argv != {} } {
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


# clean all first if needed
if { $CLEANFLAG } {
    foreach target $TARGETS {
        set build $target/builds/$BUILD 
        puts "Deleting $build"
        catch "file delete -force $build"
    }
}

set failed ""
foreach target $TARGETS {

    puts "\n----\nprocessing $target..."

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
            puts "running: $MAKE"

            # print the results line by line to provide feedback during long builds
            set fp [open "| $MAKE |& cat" "r"]
            while { ![eof $fp] } {
                gets $fp line
                puts $line
            }
            if { [catch "close $fp" res] } {
                lappend failed [file tail $target]
                puts $res
            }
        }
        default {
            if { [file tail $target] == "Base" } {
                set sln VTKSLICER.sln
            } else {
                set sln [string toupper [file tail $target]].sln
            }
            puts "running: devenv $sln /build debug"

            # no output from devenv so just go ahead and run it with no loop
            set ret [catch "exec devenv $sln /build debug" res]

            puts $res
            
            if { $ret } {
                lappend failed [file tail $target]
            }
        }
    }
}

if { [llength $failed] } {
    puts "\nBuild failed for: $failed"
} else {
    puts "\nBuild complete"
}

exit 0

