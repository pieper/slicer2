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




#
# Note: the local vars file, slicer2/slicer_variables.tcl, overrides the default values in this script
# - use it to set your local environment and then your change won't 
#   be overwritten when this file is updated
#
set localvarsfile [file dirname [info script]]/../slicer_variables.tcl
catch {set localvarsfile [file normalize $localvarsfile]}
if { [file exists $localvarsfile] } {
    source $localvarsfile
} else {
    puts "stderr: $localvarsfile not found - use this file to set up your build"
    exit 1
}

##
## All arguments do nothing by default
##
foreach var "VTK_ARG1 VTK_ARG2 VTK_ARG3 VTK_ARG4 VTK_ARG5 VTK_ARG6 VTK_ARG7 VTK_ARG8 SLICER_ARG1 SLICER_ARG2 SLICER_ARG3 SLICER_ARG4" {
    set $var "-DDUMMY:BOOL=ON"
}

set SLICER_ARG1 "-DVTKSLICERBASE_SOURCE_DIR:PATH=$SLICER_HOME/Base"
set SLICER_ARG2 "-DVTKSLICERBASE_BUILD_DIR:PATH=$SLICER_HOME/Base/builds/$env(BUILD)"
set SLICER_ARG3 "-DVTKSLICERBASE_BUILD_LIB:PATH=$VTKSLICERBASE_BUILD_LIB"
set SLICER_ARG4 "-DVTKSLICERBASE_BUILD_TCL_LIB:PATH=$VTKSLICERBASE_BUILD_TCL_LIB"

# use an already built version of vtk
set VTK_ARG1 "-DUSE_BUILT_VTK:BOOL=ON"
set VTK_ARG2 "-DVTK_DIR:PATH=$VTK_DIR"

## some operating systems 
switch $tcl_platform(os) {
    "SunOS" {
        # in order to bypass warnings about Source files
        #set VTK_ARG3 "-DCMAKE_BACKWARDS_COMPATIBILITY:STRING=1.7"
        set VTK_ARG3 "-DDUMMY:BOOL=ON"
        # explicitly specify the compiler used to compile the version of vtk that 
        # we link with
        set VTK_ARG4 "-DCMAKE_CXX_COMPILER:STRING=/local/os/bin/$COMPILER"
        set VTK_ARG5 "-DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=/local/os/bin/$COMPILER"
    }
    "Darwin" {
        set VTK_ARG3 "-DVTK_WRAP_HINTS:FILEPATH=$VTK_SRC_PATH/Wrapping/hints"
    }
}
# make sure to generate shared libraries
set VTK_ARG6 "-DBUILD_SHARED_LIBS:BOOL=ON"

# Do we have ITK?
if { $ITK_BINARY_PATH != "" } {
    set VTK_ARG7 "-DITK_DIR:FILEPATH=$ITK_BINARY_PATH"
}

# Do we have SLICER_DATA_ROOT?
if {[file isdirectory $SLICER_DATA_ROOT]} {
    set VTK_ARG8 "-DSLICER_DATA_ROOT:PATH=$SLICER_DATA_ROOT"
}

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


set TARGETS ""
set CLEANFLAG 0
set NOCMAKEFLAG 0

foreach dir $modulePaths {
    if { ![file isdirectory $dir] } {continue}
    set moduleName [file tail $dir]
    # if it's not the custom one (the example) 
    # but starts with vtk and has some c++ code, and has a cxx/CMakeLists.txt file append it to the list of targets
    if { [string match "vtk*" $moduleName] 
         && ![string match "*CustomModule*" $moduleName] 
         && [file exists $dir/cxx] 
         && [file exists $dir/cxx/CMakeLists.txt]} {
        if {[file exists [file join $dir cmaker_local.tcl]]} {
            lappend TARGETS $dir
        } else {
            set TARGETS "$dir $TARGETS"
        }
    } else {
        puts "Skipping invalid module $moduleName..."
    }
}

set TARGETS "$slicer_home/Base $TARGETS"

#
# by default, all modules are built.  If some are listed on commandline, only
# those are built.
#
# also skip the cmake step for efficiency if desired
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

set nocmakearg [lsearch $argv "--no-cmake"] 
if {$nocmakearg != -1} {
    puts "skipping cmake"
    set NOCMAKEFLAG 1
    # remove flag from the list
    set argv [lreplace $argv $nocmakearg $nocmakearg]
    if {[llength $argv] == 0} {
        set argv ""
    }
}


if { $argv != "" && $argv != {} } {
    set newtargets ""
    foreach argmodule $argv {
        set idx [lsearch -glob $TARGETS *$argmodule]
        if { $idx == -1 } {
            puts stderr "Can't find module $argmodule in search path, it may be invalid (options are: $TARGETS)"
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
        set build $target/builds/$env(BUILD) 
        puts "Deleting $build"
        if { [catch "file delete -force $build" res] } {
            puts stderr "coun't delete $build"
            puts stderr $res
        }
    }
}

#
# go through each target module and do cmake then build
# - use custom cmake argument if specified
#
set failed ""
foreach target $TARGETS {

    puts "\n----\nprocessing $target..."

    set build $target/builds/$env(BUILD) 

    catch "file mkdir $build"
    cd $build
    puts "enter directory $build..."

    if { $NOCMAKEFLAG == 0 } {
        puts "running cmake ..."

        set cmakecmd [list $CMAKE $target -G$GENERATOR \
            $VTK_ARG1 $VTK_ARG2 $VTK_ARG3 $VTK_ARG4 $VTK_ARG5 \
            $VTK_ARG6 $VTK_ARG7 \
            $SLICER_ARG1 $SLICER_ARG2 $SLICER_ARG3 $SLICER_ARG4] 

        if {[file exists [file join $target cmaker_local.tcl]]} {
            # Define SLICER_MODULE_ARG in cmaker_local.tcl
            source [file join $target cmaker_local.tcl]
            foreach elem $SLICER_MODULE_ARG  {
                lappend cmakecmd $elem 
            }
        }
        puts $cmakecmd
        if { [ catch "exec $cmakecmd" err] } {
            # catch here so that the build can continue if they're just warnings
            puts "\n----------\nCMAKE error: $err\n-----------\n"
        }
    }

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

