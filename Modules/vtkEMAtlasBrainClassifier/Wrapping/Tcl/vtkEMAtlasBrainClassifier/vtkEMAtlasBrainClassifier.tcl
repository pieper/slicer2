package require vtk

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#
# Because we do not have any vtk code we changed this a little bit
# source the Module's tcl file that contains it's init procedure

global PACKAGE_DIR_VTKEMAtlasBrainClassifier

source [file join $PACKAGE_DIR_VTKEMAtlasBrainClassifier/../../../tcl/EMAtlasBrainClassifier.tcl]

package provide vtkEMAtlasBrainClassifier 1.0

# add this module's name to the list of custom modules in order 
# to have it's init procedure get called, @ModuleName@Init will be 
# called by the Slicer Base code
global Module
lappend Module(customModules) EMAtlasBrainClassifier

