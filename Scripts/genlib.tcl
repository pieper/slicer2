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
# Packages: cmake, tcl, itcl, ITK, VTK, gsl, blt
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

# set up the versions to check out
set cmakeTag "CMake-2-0-1"
set vtkTag "Slicer-2-4"
set itkTag "Slicer-2-4"
set tclTag "core-8-4-6"
set tkTag "core-8-4-6"
set itclTag "itcl-3-2-1"
set iwidgetsTag "iwidgets-4-0-1"
set bltTag "blt24z"

# when using this on window, some things will have to be run from the cygwin terminal
set winMsg "Sorry, this isn't all automated for windows. Open a cygwin terminal and do the following:\n"



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

#######
#
# Note: the local vars file, slicer2/slicer_variables.tcl, overrides the default values in this script
# - use it to set your local environment and then your change won't 
#   be overwritten when this file is updated
#
set localvarsfile ${SLICER_HOME}/slicer_variables.tcl
catch {set localvarsfile [file normalize $localvarsfile]}
if { [file exists $localvarsfile] } {
    puts "Sourcing $localvarsfile"
    source $localvarsfile
} else {
    puts "stderr: $localvarsfile not found - use this file to set up your build"
    exit 1
}


if { $argc == 0 } {
    set LIB $SLICER_HOME/Lib/$env(BUILD)
} else {
    set LIB $argv
}

if { $GENLIB(clean) } {
    file delete -force $LIB
}

if { ![file exists $LIB] } {
    file mkdir $LIB
}

# set up cross platform files to check for existence
switch $tcl_platform(os) {
    "SunOS" -
    "Linux" -
    "Darwin" {
        set isWindows 0
        set tclTestFile $LIB/tcl-build/bin/tclsh8.4
        set tkTestFile  $LIB/tcl-build/bin/wish8.4
        set itclTestFile $LIB/tcl-build/lib/libitclstub3.2.so
        set iwidgetsTestFile $LIB/tcl-build/lib/iwidgets4.0.1/iwidgets.tcl
        set bltTestFile $LIB/tcl-build/bin/bltwish
        set vtkTestFile $LIB/VTK-build/bin/vtk
        set vtkTclLib ../tcl-build/lib/libtcl8.4.so 
        set vtkTkLib ../tcl-build/lib/libtk8.4.so
        set vtkTclsh ../tcl-build/bin/tclsh8.4
        set itkTestFile $LIB/Insight-build/lib/vtk
    }
    default {
        # different windows machines return different values, assume if none of the above, windows
        set isWindows 1
        set tclTestFile $LIB/tcl-build/bin/tclsh84.exe
        set tkTestFile $LIB/tcl-build/bin/wish84.exe
        set itclTestFile  $LIB/tcl-build/lib/itcl3.2/itcl32.dll
        set iwidgetsTestFile $LIB/tcl-build/lib/iwidgets4.0.2/iwidgets.tcl
        set bltTestFile $LIB/tcl-build/bin/bltwish.exe
        set vtkTestFile $LIB/VTK-build/bin/vtk.exe
        set vtkTclLib ../tcl-build/lib/tcl84.lib
        set vtkTkLib ../tcl-build/lib/tk84.lib
        set vtkTclsh ../tcl-build/bin/tclsh84.exe
        set itkTestFile $LIB/Insight-build/bin/libITKCommon.dll
    }
}

################################################################################
# Get and build CMake
#

# set in slicer_vars
# set CMAKE $LIB/cmake/CMake-build/bin/cmake
if { ![file exists $CMAKE] } {
    file mkdir $LIB/cmake
    cd $LIB/cmake

    runcmd cvs -d :pserver:anonymous:cmake@www.cmake.org:/cvsroot/CMake login
    runcmd cvs -z3 -d :pserver:anonymous@www.cmake.org:/cvsroot/CMake checkout -r $cmakeTag CMake

    file mkdir $LIB/cmake/CMake-build

    if {$isWindows} {
        append winMsg "cd $LIB/cmake/CMake-build\n../CMake/bootstrap\nmake\n"

    } else {
        cd $LIB/cmake/CMake-build
        runcmd ../CMake/bootstrap
        runcmd make
    }
}


################################################################################
# Get and build tcl, tk, itcl, widgets
#

# on windows, tcl won't build right, as can't configure, so save commands have to run
if { ![file exists $tclTestFile] } {
    file mkdir $LIB/tcl
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/tcl login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/tcl checkout -r $tclTag tcl

    if {$isWindows} {
        append winMsg "cd $LIB/tcl/tcl/unix\n./configure --enable-threads --prefix=$LIB/tcl-build\nmake\nmake install\n"
    } else {
        cd $LIB/tcl/tcl/unix

        runcmd ./configure --enable-threads --prefix=$LIB/tcl-build
        runcmd make
        runcmd make install
    }
}

if { ![file exists $tkTestFile] } {
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/tktoolkit login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/tktoolkit checkout -r $tkTag tk

    if {$isWindows} {
        append winMsg "cd $LIB/tcl/tk/unix\n./configure --with-tcl=$LIB/tcl-build/lib --prefix=$LIB/tcl-build\nmake\make install"
    } else {
        cd $LIB/tcl/tk/unix

        runcmd ./configure --with-tcl=$LIB/tcl-build/lib --prefix=$LIB/tcl-build
        runcmd make
        runcmd make install
    }
}

if { ![file exists $itclTestFile] } {
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/incrtcl login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/incrtcl checkout -r $itclTag incrTcl

    cd $LIB/tcl/incrTcl

    exec chmod +x ../incrTcl/configure 
    if {$isWindows} {
        append winMsg "cd $LIB/tcl/incrTcl\n./configure --with-tcl=$LIB/tcl-build/lib --with-tk=$LIB/tcl-build/lib --prefix=$LIB/tcl-build\nmake all\nmake install\n"
    } else {
        runcmd ../incrTcl/configure --with-tcl=$LIB/tcl-build/lib --with-tk=$LIB/tcl-build/lib --prefix=$LIB/tcl-build
        runcmd make all
        runcmd make install
    }
}

if { ![file exists $iwidgetsTestFile] } {
    cd $LIB/tcl

    runcmd cvs -d :pserver:anonymous:@cvs.sourceforge.net:/cvsroot/incrtcl login
    runcmd cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:/cvsroot/incrtcl checkout -r $iwidgetsTag iwidgets


    if {$isWindows} {
        append winMsg "cd $LIB/tcl/iwidgets\n./configure --with-tcl=$LIB/tcl-build/lib --with-tk=$LIB/tcl-build/lib --with-itcl=$LIB/tcl/incrTcl --prefix=$LIB/tcl-build\nmake install\n"
    } else {
        cd $LIB/tcl/iwidgets
        runcmd ../iwidgets/configure --with-tcl=$LIB/tcl-build/lib --with-tk=$LIB/tcl-build/lib --with-itcl=$LIB/tcl/incrTcl --prefix=$LIB/tcl-build
        # make all doesn't do anything... 
        runcmd make all
        runcmd make install
    }
}


################################################################################
# Get and build blt
#

if {![file exists $bltTestFile] } {
    cd $LIB/tcl
    
    puts "blt password is empty, just hit return"
    runcmd cvs -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/blt login
    runcmd cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/blt co -r $bltTag blt

    if {$isWindows} {
        append winMsg "cd $LIB/tcl/blt\n./configure --with-tcl=$LIB/tcl-build --with-tk=$LIB/tcl-build --prefix=$LIB/tcl-build\nmake\nmake install"
    } else {
        cd $LIB/tcl/blt
        runcmd ./configure --with-tcl=$LIB/tcl-build --with-tk=$LIB/tcl-build --prefix=$LIB/tcl-build 
        runcmd make
        runcmd make install
    }

}

################################################################################
# Get and build vtk
#

if { ![file exists $vtkTestFile] } {
    cd $LIB

    runcmd cvs -d :pserver:anonymous:vtk@public.kitware.com:/cvsroot/VTK login
    runcmd cvs -z3 -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK checkout -r $vtkTag VTK

    file mkdir $LIB/VTK-build
    cd $LIB/VTK-build

    if {$isWindows} {
    append winMsg "cd $LIB/VTK-build\n$CMAKE -G$GENERATOR -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_USE_CARBON:BOOL=OFF -DVTK_USE_X:BOOL=ON -DVTK_WRAP_TCL:BOOL=ON -DVTK_USE_HYBRID:BOOL=ON -DVTK_USE_PATENTED:BOOL=ON -DTCL_INCLUDE_PATH:PATH=../tcl-build/include -DTK_INCLUDE_PATH:PATH=../tcl-build/include -DTCL_LIBRARY:FILEPATH=$vtkTclLib -DTK_LIBRARY:FILEPATH=$vtkTkLib -DTCL_TCLSH:FILEPATH=$vtkTclsh ../VTK make -j4\n"
    } else {
    runcmd $CMAKE \
        -G$GENERATOR \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_TESTING:BOOL=OFF \
        -DVTK_USE_CARBON:BOOL=OFF \
        -DVTK_USE_X:BOOL=ON \
        -DVTK_WRAP_TCL:BOOL=ON \
        -DVTK_USE_HYBRID:BOOL=ON \
        -DVTK_USE_PATENTED:BOOL=ON \
        -DTCL_INCLUDE_PATH:PATH=../tcl-build/include \
        -DTK_INCLUDE_PATH:PATH=../tcl-build/include \
        -DTCL_LIBRARY:FILEPATH=$vtkTclLib \
        -DTK_LIBRARY:FILEPATH=$vtkTkLib \
        -DTCL_TCLSH:FILEPATH=$vtkTclsh \
        ../VTK
    runcmd make -j4
    }

}

################################################################################
# Get and build itk
#

if { ![file exists $itkTestFile] } {
    cd $LIB

    runcmd cvs -d :pserver:anoncvs:@www.itk.org:/cvsroot/Insight login
    runcmd cvs -z3 -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight checkout -r $itkTag Insight

    file mkdir $LIB/Insight-build
    cd $LIB/Insight-build


    if {$isWindows} {
    append winMsg "cd $LIB/Insight-build\nruncmd $CMAKE -G$GENERATOR -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF ../Insight\nmake -j4\n"
    } else {
    runcmd $CMAKE \
        -G$GENERATOR \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        ../Insight
    runcmd make -j4
    }

}

if {$isWindows} {
    puts $winMsg
}
