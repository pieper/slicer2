package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

if {[info commands vtkInsight] != "" ||
    [::vtk::load_component vtkInsightTCL] == ""} {
    package provide vtkInsight 1.0
}
