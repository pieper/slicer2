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
# Packages: cmake, tcl, itcl, ITK, VTK, blt, gsl
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

if {[info exists ::env(CVS)]} {
    set ::CVS $::env(CVS)
} else {
    set ::CVS cvs
}

# for subversion repositories (Sandbox)
if {[info exists ::env(SVN)]} {
    set ::SVN $::env(SVN)
} else {
    set ::SVN svn
}

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
    set msg "$msg\n   --release : compile with optimization flags"
    puts stderr $msg
}

set GENLIB(clean) "false"
set isRelease 0
set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--clean" -
        "-f" {
            set GENLIB(clean) "true"
        }
        "--release" {
            set isRelease 1
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
    global isWindows
    puts "running: $args"

    # print the results line by line to provide feedback during long builds
    # interleaves the results of stdout and stderr, except on Windows
    if { $isWindows } {
        # Windows does not provide native support for cat
        set fp [open "| $args" "r"]
    } else {
        set fp [open "| $args |& cat" "r"]
    }
    while { ![eof $fp] } {
        gets $fp line
        puts $line
    }
    set ret [catch "close $fp" res] 
    if { $ret } {
        puts stderr $res
        if { $isWindows } {
            # Does not work on Windows
        } else {
            error $ret
        }
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
set localvarsfile $SLICER_HOME/slicer_variables.tcl
catch {set localvarsfile [file normalize $localvarsfile]}
if { [file exists $localvarsfile] } {
    puts "Sourcing $localvarsfile"
    source $localvarsfile
} else {
    puts "stderr: $localvarsfile not found - use this file to set up your build"
    exit 1
}

if ($isRelease) {
    set ::env(VTK_BUILD_TYPE) "Release"
    puts "Overriding slicer_variables.tcl; VTK_BUILD_TYPE is $::env(VTK_BUILD_TYPE)"
}

#initialize platform variables
switch $tcl_platform(os) {
    "SunOS" {
        set isSolaris 1
        set isWindows 0
        set isDarwin 0
        set isLinux 0
    }
    "Linux" { 
        set isSolaris 0
        set isWindows 0
        set isDarwin 0
        set isLinux 1
    }
    "Darwin" { 
        set isSolaris 0
        set isWindows 0
        set isDarwin 1
        set isLinux 0
    }
    default { 
        set isSolaris 0
        set isWindows 1
        set isDarwin 0
        set isLinux 0
    }
}

# tcl file delete is broken on Darwin, so use rm -rf instead
if { $GENLIB(clean) } {
    puts "Deleting slicer lib files..."
    if { $isDarwin } {
        runcmd rm -rf $SLICER_LIB
        if { [file exists $SLICER_HOME/isPatched] } {
            runcmd rm $SLICER_HOME/isPatched
        }
    } elseif { $isWindows }  {
        file delete -force $SLICER_LIB/VTK
        file delete -force $SLICER_LIB/VTK-build
        file delete -force $SLICER_LIB/Insight
        file delete -force $SLICER_LIB/Insight-build
    } else {
        file delete -force $SLICER_LIB
    }
}

if { ![file exists $SLICER_LIB] } {
    file mkdir $SLICER_LIB
}

################################################################################
# Get and build CMake
#

# set in slicer_vars
if { ![file exists $CMAKE] } {
    file mkdir $CMAKE_PATH
    cd $SLICER_LIB


    if {$isWindows} {
        puts stderr "-- genlib.tcl cannot generate the cmake, tcl, or gsl binaries for windows --"
        puts stderr ""
        puts stderr "Follow the instructions at the following link"
        puts stderr "to download precompiled versions of parts of the windows Lib"
        puts stderr "http://www.na-mic.org/Wiki/index.php/Slicer:Slicer_2.5_Building"
        puts stderr ""
        puts stderr "Then re-run genlib.tcl to build VTK and ITK"
        puts stderr ""
        exit
    } else {
        runcmd $::CVS -d :pserver:anonymous:cmake@www.cmake.org:/cvsroot/CMake login
        runcmd $::CVS -z3 -d :pserver:anonymous@www.cmake.org:/cvsroot/CMake checkout -r $::CMAKE_TAG CMake

        cd $CMAKE_PATH
        if { $isSolaris } {
            # make sure to pick up curses.h in /local/os/include
            runcmd $SLICER_LIB/CMake/bootstrap --init=$SLICER_HOME/Scripts/spl.cmake.init
        } else {
            runcmd $SLICER_LIB/CMake/bootstrap
        } 
        eval runcmd $::MAKE
    }
}


################################################################################
# Get and build teem
#

if { ![file exists $::TEEM_TEST_FILE] } {
    cd $SLICER_LIB

    runcmd $::CVS -d:pserver:anonymous:@cvs.sourceforge.net:/cvsroot/teem/ login 
    runcmd $::CVS -z3 -d:pserver:anonymous:@cvs.sourceforge.net:/cvsroot/teem/ checkout -r $::TEEM_TAG teem

    file mkdir $SLICER_LIB/teem-build
    cd $SLICER_LIB/teem-build

    if { $isDarwin } {
        set C_FLAGS -DCMAKE_C_FLAGS:STRING=$C_FLAGS \
    } else {
        set C_FLAGS ""
    }

    runcmd $CMAKE \
        -G$GENERATOR \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
        $C_FLAGS \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_TESTING:BOOL=OFF \
        ../teem

    if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE teem.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE teem.SLN /build  $::VTK_BUILD_TYPE
        }
    } else {
        eval runcmd $::MAKE -j 8
    }
}


################################################################################
# Get and build tcl, tk, itcl, widgets
#

# on windows, tcl won't build right, as can't configure, so save commands have to run
if { ![file exists $::TCL_TEST_FILE] } {

    if {$isWindows} {
        puts stderr "-- genlib.tcl cannot generate the tcl binaries for windows --"
        puts stderr "1) Get a copy of Tcl 8.4 from tcl.activestate.com."
        puts stderr "2) Set the TCL_BIN_DIR, TCL_LIB_DIR and TCL_INCLUDE_DIR in slicer_variables.tcl."
        puts stderr "3) Get BLT from http://prdownloads.sourceforge.net/blt/blt24z-for-tcl84.exe?download"
        puts stderr "4) Install BLT to a *different* directory than where you installed activetcl"
        puts stderr "5) copy BLT24.dll and BLT24lite24.dll to TCL_BIN_DIR"
        puts stderr "6) copy blt2.4 to TCL_LIB_DIR"
        puts stderr "With these pieces in place, genlib can build VTK and ITK"
        exit
    }

    file mkdir $SLICER_LIB/tcl
    cd $SLICER_LIB/tcl

    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer login
    runcmd $::CVS -z3 -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer checkout -r $::TCL_TAG tcl

    if {$isWindows} {
        # can't do windows
    } else {
        cd $SLICER_LIB/tcl/tcl/unix

        runcmd ./configure --enable-threads --prefix=$SLICER_LIB/tcl-build
        eval runcmd $::MAKE
        eval runcmd $::MAKE install
    }
}

if { ![file exists $::TK_TEST_FILE] } {
    cd $SLICER_LIB/tcl

    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer login
    runcmd $::CVS -z3 -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer checkout -r $::TK_TAG tk

    if {$isDarwin} {
        if { ![file exists $SLICER_HOME/isPatched] } {
                puts "Patching..."
                runcmd curl -k -O https://share.spl.harvard.edu/share/birn/public/software/External/Patches/tkEventPatch.diff
                runcmd cp tkEventPatch.diff $SLICER_LIB/tcl/tk/generic 
                cd $SLICER_LIB/tcl/tk/generic
                runcmd patch -i tkEventPatch.diff

                # create a file to make sure tkEvent.c isn't patched twice
                runcmd touch $SLICER_HOME/isPatched
                file delete $SLICER_LIB/tcl/tk/generic/tkEventPatch.diff
        } else {
            puts "tkEvent.c already patched."
        }
    }

    if {$isWindows} {
        # can't do windows
    } else {
        cd $SLICER_LIB/tcl/tk/unix

        runcmd ./configure --with-tcl=$SLICER_LIB/tcl-build/lib --prefix=$SLICER_LIB/tcl-build
        eval runcmd $::MAKE
        eval runcmd $::MAKE install
    }
}

if { ![file exists $::ITCL_TEST_FILE] } {
    cd $SLICER_LIB/tcl

    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer login
    runcmd $::CVS -z3 -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer checkout -r $::ITCL_TAG incrTcl

    cd $SLICER_LIB/tcl/incrTcl

    exec chmod +x ../incrTcl/configure 

    if {$isWindows} {
        # can't do windows
    } else {
        runcmd ../incrTcl/configure --with-tcl=$SLICER_LIB/tcl-build/lib --with-tk=$SLICER_LIB/tcl-build/lib --prefix=$SLICER_LIB/tcl-build
        if { $isDarwin } {
            # need to run ranlib separately on lib for Darwin
            # file is created and ranlib is needed inside make all
            catch "eval runcmd $::MAKE all"
            runcmd ranlib ../incrTcl/itcl/libitclstub3.2.a
        }
        eval runcmd $::MAKE all
        eval runcmd $::MAKE install
    }
}

if { ![file exists $::IWIDGETS_TEST_FILE] } {
    cd $SLICER_LIB/tcl

    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer login
    runcmd $::CVS -z3 -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer checkout -r $::IWIDGETS_TAG iwidgets


    if {$isWindows} {
        # can't do windows
    } else {
        cd $SLICER_LIB/tcl/iwidgets
        runcmd ../iwidgets/configure --with-tcl=$SLICER_LIB/tcl-build/lib --with-tk=$SLICER_LIB/tcl-build/lib --with-itcl=$SLICER_LIB/tcl/incrTcl --prefix=$SLICER_LIB/tcl-build
        # make all doesn't do anything... 
        eval runcmd $::MAKE all
        eval runcmd $::MAKE install
    }
}


################################################################################
# Get and build blt
#

if { ![file exists $::BLT_TEST_FILE] } {
    cd $SLICER_LIB/tcl
    
    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer login
    runcmd $::CVS -z3 -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer co -r $::BLT_TAG blt 

    if { $isWindows } {
        # can't do Windows
    } elseif { $isDarwin } {
        if { ![file exists $SLICER_HOME/isPatchedBLT] } {
            puts "Patching..."
            runcmd curl -k -O https://share.spl.harvard.edu/share/birn/public/software/External/Patches/bltPatch.diff
            cd $SLICER_LIB/tcl/blt
            runcmd patch -p 1 < ../bltPatch.diff
            
            # create a file to make sure BLT isn't patched twice
            runcmd touch $SLICER_HOME/isPatchedBLT
            file delete $SLICER_LIB/tcl/bltPatch.diff
        } else {
            puts "BLT already patched."
        }
        
        # this fails, but gets blt far enough along to build what is needed 
        cd $SLICER_LIB/tcl/blt
        runcmd ./configure --with-tcl=$SLICER_LIB/tcl-build --with-tk=$SLICER_LIB/tcl-build --prefix=$SLICER_LIB/tcl-build 
        catch "eval runcmd $::MAKE"
        catch "eval runcmd $::MAKE install"
    } else {
        cd $SLICER_LIB/tcl/blt
        runcmd ./configure --with-tcl=$SLICER_LIB/tcl-build --with-tk=$SLICER_LIB/tcl-build --prefix=$SLICER_LIB/tcl-build 
        eval runcmd $::MAKE
        eval runcmd $::MAKE install
    }
}


################################################################################
# Get and build gsl
#

if { ![file exists $::GSL_TEST_FILE] } {
    file mkdir $SLICER_LIB/gsl-build
    file mkdir $SLICER_LIB/gsl

    cd $SLICER_LIB/gsl-build/

    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer login
    runcmd $::CVS -d :pserver:anonymous:bwhspl@cvs.spl.harvard.edu:/projects/cvs/slicer co -r $::GSL_TAG gsl

    if { !$isWindows } {
        # can't do Windows
        cd $SLICER_LIB/gsl-build/gsl

        if { $isDarwin } {
            # equivalent of autogen.sh for Darwin (libtoolize => glibtoolize)    
            runcmd glibtoolize --automake
            runcmd aclocal
            runcmd automake --add-missing --gnu
            runcmd autoconf
        } else {
            if { $isSolaris } {
                # patch configure.ac
                runcmd /usr/bin/patch -i $SLICER_HOME/Scripts/gslConfigureAc.diff $SLICER_LIB/gsl-build/gsl/configure.ac
            }
            # equivalent of autogen.sh with hack to make autoconf run on Solaris and RH 7.3.  
            runcmd libtoolize --automake
            runcmd aclocal
            runcmd automake --add-missing --gnu
            # autoconf will fail the first time, then succeed on RH 7.3 and Solaris 
            catch "runcmd autoconf -I ."
            runcmd autoconf -I .
        }   
        runcmd ./configure --prefix=$SLICER_LIB/gsl
        runcmd touch doc/version-ref.texi
        eval runcmd $::MAKE
        eval runcmd $::MAKE install
    }

    # check if gsl_multifit.h is in the right location
    if { ![file exists $SLICER_LIB/gsl/include/gsl/gsl_multifit.h] } {
        puts "GSL built in the wrong place; moving files to $SLICER_LIB/gsl/include/gsl/"
        file copy $SLICER_LIB/gsl/include/gsl_multifit.h $SLICER_LIB/gsl/include/gsl/gsl_multifit.h
        file copy $SLICER_LIB/gsl/include/gsl_sf.h $SLICER_LIB/gsl/include/gsl/gsl_sf.h
    }

}


################################################################################
# Get and build vtk
#

if { ![file exists $::VTK_TEST_FILE] } {
    cd $SLICER_LIB

    runcmd $::CVS -d :pserver:anonymous:vtk@public.kitware.com:/cvsroot/VTK login
    runcmd $::CVS -z3 -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK checkout -r $::VTK_TAG VTK

    # Andy's temporary hack to get around wrong permissions in VTK cvs repository
    # catch statement is to make file attributes work with RH 7.3
    if { !$isWindows } {
        catch "file attributes $SLICER_LIB/VTK/VTKConfig.cmake.in -permissions a+rw"
    }

    file mkdir $SLICER_LIB/VTK-build
    cd $SLICER_LIB/VTK-build

    set USE_VTK_ANSI_STDLIB ""
    if { $isWindows } {
        if {$MSVC6} {
            set USE_VTK_ANSI_STDLIB "-DVTK_USE_ANSI_STDLIB:BOOL=ON"
        }
    }

    #
    # Note - the two banches are identical down to the line starting -DOPENGL...
    # -- the text needs to be duplicated to avoid quoting problems with paths that have spaces
    #
    if { $isLinux && $::tcl_platform(machine) == "x86_64" } {
        runcmd $CMAKE \
            -G$GENERATOR \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_X:BOOL=ON \
            -DVTK_WRAP_TCL:BOOL=ON \
            -DVTK_USE_HYBRID:BOOL=ON \
            -DVTK_USE_PATENTED:BOOL=ON \
            -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
            -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
            -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
            $USE_VTK_ANSI_STDLIB \
            -DOPENGL_INCLUDE_DIR:PATH=/usr/include \
            -DOPENGL_gl_LIBRARY:FILEPATH=/usr/lib64/libGL.so \
            -DOPENGL_glu_LIBRARY:FILEPATH=/usr/lib64/libGLU.so \
            -DX11_X11_LIB:FILEPATH=/usr/X11R6/lib64/libX11.a \
            -DX11_Xext_LIB:FILEPATH=/usr/X11R6/lib64/libXext.a \
            -DCMAKE_MODULE_LINKER_FLAGS:STRING=-L/usr/X11R6/lib64 \
            -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
            ../VTK
    } else {
        runcmd $CMAKE \
            -G$GENERATOR \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_X:BOOL=ON \
            -DVTK_WRAP_TCL:BOOL=ON \
            -DVTK_USE_HYBRID:BOOL=ON \
            -DVTK_USE_PATENTED:BOOL=ON \
            -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
            -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
            -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
            $USE_VTK_ANSI_STDLIB \
            ../VTK
    }


    if { $isDarwin } {
        # Darwin will fail on the first make, then succeed on the second
        catch "eval runcmd $::MAKE -j4"
        set OpenGLString "-framework OpenGL -lgl"
        runcmd $CMAKE -G$GENERATOR -DOPENGL_gl_LIBRARY:STRING=$OpenGLString -DVTK_USE_SYSTEM_ZLIB:BOOL=ON ../VTK
    }
    
    if { $isWindows } {
        if { $MSVC6 } {
            runcmd $::MAKE VTK.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE VTK.SLN /build  $::VTK_BUILD_TYPE
        }
    } else {
        eval runcmd $::MAKE -j 8
    }
}

################################################################################
# Get and build itk
#

if { ![file exists $::ITK_TEST_FILE] } {
    cd $SLICER_LIB

    runcmd $::CVS -d :pserver:anoncvs:@www.itk.org:/cvsroot/Insight login
    runcmd $::CVS -z3 -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight checkout -r $::ITK_TAG Insight

    file mkdir $SLICER_LIB/Insight-build
    cd $SLICER_LIB/Insight-build



    runcmd $CMAKE \
        -G$GENERATOR \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        ../Insight

    if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE ITK.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE ITK.SLN /build  $::VTK_BUILD_TYPE
        }
    } else {
        eval runcmd $::MAKE -j 8
    }
}



################################################################################
# Get and build the sandbox

if { ![file exists $::SANDBOX_TEST_FILE] } {
    cd $SLICER_LIB

    runcmd $::SVN checkout -r $::SANDBOX_TAG http://www.na-mic.org:8000/svn/NAMICSandBox 

    file mkdir $SLICER_LIB/NAMICSandBox-build
    cd $SLICER_LIB/NAMICSandBox-build

    runcmd $CMAKE \
        -G$GENERATOR \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        -DVTK_DIR:PATH=$VTK_DIR \
        -DITK_DIR:FILEPATH=$ITK_BINARY_PATH \
        ../NAMICSandBox

    if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE NAMICSandBox.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            #runcmd $::MAKE NAMICSandBox.SLN /build  $::VTK_BUILD_TYPE

            # These two lines fail on windows because the .sln file has a problem.
            # Perhaps this is a cmake issue.
            #cd $SLICER_LIB/NAMICSandBox-build/SlicerTractClusteringImplementation
            #runcmd $::MAKE SlicerClustering.SLN /build  $::VTK_BUILD_TYPE

            # Building within the subdirectory works
            cd $SLICER_LIB/NAMICSandBox-build/SlicerTractClusteringImplementation/Code
            runcmd $::MAKE SlicerClustering.vcproj /build  $::VTK_BUILD_TYPE
            cd $SLICER_LIB/NAMICSandBox-build/SlicerTractClusteringImplementation/Code
            runcmd $::MAKE SlicerClustering.vcproj /build  $::VTK_BUILD_TYPE
            # However then it doesn't pick up this needed library
            cd $SLICER_LIB/NAMICSandBox-build/SpectralClustering
            runcmd $::MAKE SpectralClustering.SLN /build  $::VTK_BUILD_TYPE
        }
    } else {

        # Just build the two libraries we need, not the rest of the sandbox.
        # This line builds the SlicerClustering library.
        # It also causes the SpectralClustering lib to build, 
        # since SlicerClustering depends on it.
        # Later in the slicer Module build process, 
        # vtkDTMRI links to SlicerClustering.
        # At some point in the future, the classes in these libraries
        # will become part of ITK and this will no longer be needed.
        cd $SLICER_LIB/NAMICSandBox-build/SlicerTractClusteringImplementation   
        eval runcmd $::MAKE -j 8
    }
}
