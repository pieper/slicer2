
package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

if {[info commands vtkFastMarching] != "" ||
    [::vtk::load_component vtkFastMarchingTCL] == ""} {
    global PACKAGE_DIR
    source  [file join $PACKAGE_DIR/../../../tcl/EdFastMarching.tcl]
    package provide vtkFastMarching 1.0
}
