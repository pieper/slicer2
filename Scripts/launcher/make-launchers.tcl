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

set starkitRelease "-200209"

puts "making linux..."
exec tclkit starkit${starkitRelease}/sdx.kit wrap slicer2 -runtime starkit${starkitRelease}/tclkit-linux-x86
file rename -force slicer2 ../../slicer2-linux-x86

puts "making solaris..."
exec tclkit starkit${starkitRelease}/sdx.kit wrap slicer2 -runtime starkit${starkitRelease}/tclkit-solaris-sparc
file rename -force slicer2 ../../slicer2-solaris-sparc

puts "making win32..."
exec  tclkit starkit${starkitRelease}/sdx.kit wrap slicer2.exe -runtime starkit${starkitRelease}/tclkit-win32.exe
file rename -force slicer2.exe ../../slicer2-win32.exe
