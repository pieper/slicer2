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
exec tclkit starkit-200209/sdx.kit wrap slicer2 -runtime starkit-200209/tclkit-linux-x86
file rename -force slicer2 ../../slicer2-linux-x86

puts "making solaris..."
exec tclkit starkit-200209/sdx.kit wrap slicer2 -runtime starkit-200209/tclkit-solaris-sparc
file rename -force slicer2 ../../slicer2-solaris-sparc

puts "making win32..."
#exec  tclkit starkit-200209/sdx.kit wrap slicer2.exe -runtime starkit-200209/tclkit-win32.exe
#file rename -force slicer2.exe ../../slicer2-win32.exe
puts "NOTE: the windows starpack method doesn't pass arguments correctly."
puts "Use \"freewrap.exe slicer2-win32.tcl\" to create the launcher"
puts "and copy it up to the top level (must be done on windows)."
