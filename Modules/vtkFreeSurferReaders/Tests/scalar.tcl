#! /usr/local/bin/vtk

# first we load in the standard vtk packages into tcl
package require vtk
package require vtkinteraction

load ../builds/bin/libvtkFreeSurferReadersTCL.so

# This loads the scalar file.
set reader [vtkFSSurfaceScalarReader _curv]
$reader SetFileName "/home/kteich/subjects/anders/surf/lh.curv"
$reader Update

# check some values
set curv [$reader GetOutput]
if { $curv != "" } {
    set lRange [$curv GetRange]
    puts "Range: [lindex $lRange 0] -> [lindex $lRange 1]"
} else {
    puts "Didn't get it."
}


exit

