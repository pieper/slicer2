#!/bin/sh
# \
exec tclkit "$0" ${1+"$@"}

#
# make-launchers.tcl
#

set __comment {

    This script creates the stand-alone executables
    (starpacks) to startup slicer from an arbitrary directory.

    See www.equi4.com for info about starkits and starpacks.

}


puts "making linux..."
exec sdx wrap slicer2 -runtime starkit-200209/tclkit-linux-x86
file rename -force slicer2 ../../slicer2-linux-x86

puts "making solaris..."
exec sdx wrap slicer2 -runtime starkit-200209/tclkit-solaris-sparc
file rename -force slicer2 ../../slicer2-solaris-sparc

puts "making win32..."
exec sdx wrap slicer2.exe -runtime starkit-200209/tclkit-win32.exe
file rename -force slicer2.exe ../../slicer2-win32.exe
