package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

if {[info commands vtkFreeSurferReaders] != "" ||
    [::vtk::load_component vtkFreeSurferReadersTCL] == ""} {
    global PACKAGE_DIR_VTKFREESURFERREADERS
    source  [file join $PACKAGE_DIR_VTKFREESURFERREADERS/../../../tcl/VolFreeSurferReaders.tcl]
    package provide vtkFreeSurferReaders 1.0
}

