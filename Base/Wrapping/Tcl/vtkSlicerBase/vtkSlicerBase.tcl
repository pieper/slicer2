package require vtk

#
# rely on the fact that vtkMrmlData is uniquely available through
# vtkSlicerBase
#

if {[info commands vtkMrmlData] != "" ||
    [::vtk::load_component vtkSlicerBaseTCL] == ""} {
    package provide vtkSlicerBase 2.0
}
