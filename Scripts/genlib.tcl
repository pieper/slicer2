#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

################################################################################
#
# genlib.tcl
#
# generate the Lib directory with the needed components for slicer
# to build
#
# Steps:
# - pull code from anonymous cvs
# - configure (or cmake) with needed options
# - build for this platform
#
# Packages: cmake, tcl, itcl, ITK, VTK, gsl
# 
# Usage:
#   genlib [options] [target]
#
# run genlib from the slicer directory where you want the packages to be build
# E.g. if you run /home/pieper/slicer2/Scripts/genlib.tcl on a redhat7.3
# machine it will create /home/pieper/slicer2/Lib/redhat7.3
#
# - sp - 2004-06-20
#


################################################################################
#
# simple command line argument parsing
#

proc Usage { {msg ""} } {
    global SLICER
    
    set msg "$msg\nusage: genlib \[options\] \[target\]"
    set msg "$msg\n  \[target\] is determined automatically if not specified"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   --help : prints this message and exits"
    set msg "$msg\n   --clean : delete the target first"
    puts stderr $msg
}

set GENLIB(clean) "false"
set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--clean" -
        "-f" {
            set GENLIB(clean) "true"
        }
        "--help" -
        "-h" {
            Usage
            exit 1
        }
        "-*" {
            Usage "unknown option $a\n"
            exit 1
        }
        default {
            lappend strippedargs $a
        }
    }
}
set argv $strippedargs
set argc [llength $argv]

if {$argc > 1 } {
    Usage
    exit 1
}


################################################################################
#
# Utilities:

proc runcmd {args} {
    puts "running: $args"

    # print the results line by line to provide feedback during long builds
    set fp [open "| $args |& cat" "r"]
    while { ![eof $fp] } {
        gets $fp line
        puts $line
    }
    set ret [catch "close $fp" res] 
    if { $ret } {
        puts stderr $res
        error $ret
    } 
}

################################################################################
# First, set up the directory
# - determine the location
# - determine the build
# 

# hack to work around lack of normalize option in older tcl
# set SLICER_HOME [file dirname [file dirname [file normalize [info script]]]]
set cwd [pwd]
cd [file dirname [info script]]
cd ..
set SLICER_HOME [pwd]
cd $cwd

switch $::tcl_platform(os) {
    "Linux" {
        switch $::tcl_platform(machine) {
            "ia64" {
                set BUILD linux-ia64
            }
            default {
                set BUILD linux-x86
            }
       }
    }
    "windows" {
        set BUILD win32
    }
}

if { $argc == 0 } {
    set LIB $SLICER_HOME/Lib/$BUILD
} else {
    set LIB $argv
}

if { $GENLIB(clean) } {
    file delete -force $LIB
}

if { ![file exists $LIB] } {
    file mkdir $LIB
}


################################################################################
# Get and build CMake
#

set CMAKE $LIB/cmake/CMake-build/bin/cmake
if { ![file exists $CMAKE] } {
    file mkdir $LIB/cmake
    cd $LIB/cmake

    runcmd cvs -d :pserver:anonymous:cmake@www.cmake.org:/cvsroot/CMake login
    runcmd cvs -z3 -d :pserver:anonymous@www.cmake.org:/cvsroot/CMake checkout -r CMake-2-0-1 CMake

    file mkdir $LIB/cmake/CMake-build
    cd $LIB/cmake/CMake-build

    runcmd ../CMake/bootstrap
    runcmd make
}


################################################################################
# Get and build tcl, tk, itcl, widgets
#

if { ![file exists $LIB/tcl-build/bin/tclsh8.4] } {
    file mkdir $LIB/tcl
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/tcl login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/tcl checkout -r core-8-4-6 tcl

    cd $LIB/tcl/tcl/unix

    runcmd ./configure --enable-threads --prefix=$LIB/tcl-build
    runcmd make
    runcmd make install
}

if { ![file exists $LIB/tcl-build/bin/wish8.4] } {
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/tktoolkit login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/tktoolkit checkout -r core-8-4-6 tk

    cd $LIB/tcl/tk/unix

    runcmd ./configure --with-tcl=$LIB/tcl-build/lib --prefix=$LIB/tcl-build
    runcmd make
    runcmd make install
}

if { ![file exists $LIB/tcl-build/lib/libitcl3.2.so] } {
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/incrtcl login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/incrtcl checkout -r itcl-3-2-1 incrTcl

    cd $LIB/tcl/incrTcl

    exec chmod +x ../incrTcl/configure 
    runcmd ../incrTcl/configure --with-tcl=$LIB/tcl-build/lib --with-tk=$LIB/tcl-build/lib --prefix=$LIB/tcl-build
    runcmd make all
    runcmd make install
}

if { ![file exists $LIB/tcl-build/lib/iwidgets4.0.1/iwidgets.tcl] } {
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/incrtcl login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/incrtcl checkout -r iwidgets-4-0-1 iwidgets

    cd $LIB/tcl/iwidgets
    runcmd ../iwidgets/configure --with-tcl=$LIB/tcl-build/lib --with-tk=$LIB/tcl-build/lib --with-itcl=$LIB/tcl/incrTcl --prefix=$LIB/tcl-build
    runcmd make all
    runcmd make install
}

################################################################################
# Get and build vtk
#

if { ![file exists $LIB/VTK-build/bin/vtk] } {
    cd $LIB

    runcmd cvs -d :pserver:anonymous:vtk@public.kitware.com:/cvsroot/VTK login
    runcmd cvs -z3 -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK checkout -r release-4-2-6 VTK

    file mkdir $LIB/VTK-build
    cd $LIB/VTK-build

    runcmd $CMAKE \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_TESTING:BOOL=OFF \
        -DVTK_WRAP_TCL:BOOL=ON \
        -DVTK_USE_HYBRID:BOOL=ON \
        -DVTK_USE_PATENTED:BOOL=ON \
        -DTCL_INCLUDE_PATH:PATH=../tcl-build/include \
        -DTK_INCLUDE_PATH:PATH=../tcl-build/include \
        -DTCL_LIBRARY:FILEPATH=../tcl-build/lib/libtcl8.4.so \
        -DTK_LIBRARY:FILEPATH=../tcl-build/lib/libtk8.4.so \
        -DTCL_TCLSH:FILEPATH=../tcl-build/bin/tclsh8.4 \
        ../VTK

    runcmd make -j4

}

################################################################################
# Get and build itk
#

if { ![file exists $LIB/Insight-build/lib/vtk] } {
    cd $LIB

    runcmd cvs -d :pserver:anoncvs:@www.itk.org:/cvsroot/Insight login
    runcmd cvs -z3 -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight checkout -r ITK-1-6-0 Insight

    file mkdir $LIB/Insight-build
    cd $LIB/Insight-build

    runcmd $CMAKE \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        ../Insight

    runcmd make -j4

}
