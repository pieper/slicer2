package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

if {[info commands vtkITK] != "" ||
    [::vtk::load_component vtkITKTCL] == ""} {
    package provide vtkITK 1.0
}
