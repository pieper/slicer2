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
    set msg "$msg\n   --force : delete the target first"
    puts stderr $msg
}

set GENLIB(force) "false"
set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--force" -
        "-f" {
            set GENLIB(force) "true"
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
# TODO - determine the build name consistently
# - determine the location
# - determine the build
# 

set SLICER_HOME [file dirname [file dirname [file normalize [info script]]]]
set BUILD win32

if { $argc == 0 } {
    set LIB $SLICER_HOME/Lib/$BUILD
} else {
    set LIB $argv
}

if { $GENLIB(force) } {
    file delete -force $LIB
}

if { [file exists $LIB] } {
    puts stderr "target directory $LIB already exists -- please delete it first"
    exit 1
}

file mkdir $LIB


################################################################################
# Get and build CMake
#

file mkdir $LIB/cmake
cd $LIB/cmake

runcmd cvs -d :pserver:anonymous:cmake@www.cmake.org:/cvsroot/CMake login
runcmd cvs -d :pserver:anonymous@www.cmake.org:/cvsroot/CMake checkout -r 2.0.1 CMake

cd CMake




################################################################################
# Get and build tcl
#

