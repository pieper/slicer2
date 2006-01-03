package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

#if {[info commands vtkEditorGeometryDraw] != "" ||
#    [::vtk::load_component vtkEditorGeometryDrawTCL] == ""} {

if {[info commands EditorGeometryDrawInit] == ""} {
    global PACKAGE_DIR_VTKEditorGeometryDraw
    package provide vtkEditorGeometryDraw 1.0

    # source the Module's tcl file that contains its init procedure
    source [file join $PACKAGE_DIR_VTKEditorGeometryDraw/../../../tcl/EdGeometryDraw.tcl]
    # add this module's name to the list of custom modules in order 
    # to have its init procedure get called, @ModuleName@Init will be 
    # called by the Slicer Base code
    
    #global Module
    #lappend Module(customModules) EditorGeometryDraw

    # This is an editor submodule and should be loaded automatically
    # by putting information into the global Ed array.
}
