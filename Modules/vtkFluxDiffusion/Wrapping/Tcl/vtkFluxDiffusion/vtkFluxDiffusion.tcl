package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

if {[info commands vtkFluxDiffusion] != "" ||
    [::vtk::load_component vtkFluxDiffusionTCL] == ""} {
    global PACKAGE_DIR_VTKFLUXDIFFUSION
    source  $PACKAGE_DIR_VTKFLUXDIFFUSION/../../../tcl/FluxDiffusion.tcl
    package provide vtkFluxDiffusion 1.0
}
