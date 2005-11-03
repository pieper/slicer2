# Initialize values

set SLICER_MODULE_ARG "-DVTKEMATLAS_SOURCE_DIR:PATH=$SLICER_HOME/Modules/vtkEMAtlasBrainClassifier"
lappend SLICER_MODULE_ARG "-DVTKEMLOCAL_BUILD_DIR:PATH=$SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)" 

if {[file exists $SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)/bin/libvtkEMAtlasBrainClassifier.so] == 1} {
     lappend SLICER_MODULE_ARG "-DVTKEMATLAS_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)/bin/libvtkEMAtlasBrainClassifier.so"
     lappend SLICER_MODULE_ARG "-DVTKEMATLAS_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)/bin/libvtkEMAtlasBrainClassifierTCL.lib" 
} else {
     lappend SLICER_MODULE_ARG "-DVTKEMATLAS_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)/bin/debug/vtkEMAtlasBrainClassifier.lib"
     lappend SLICER_MODULE_ARG "-DVTKEMATLAS_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)/bin/debug/vtkEMAtlasBrainClassifierTCL.lib" 
}

# Necessary step after the '$Dir' directory is erased 
set Dir $SLICER_HOME/Modules/vtkEMLocalSegment/builds/$env(BUILD)
catch "file mkdir $Dir"

if {[file exists $Dir/vtkEMAtlasBrainClassifierConfigure.h] == 0 } {
    if {[catch "file copy ${Dir}/../vtkEMAtlasBrainClassifierConfigure.h ${Dir}/."]} {
        puts "Error: Failed copying  ${Dir}/../vtkEMAtlasBrainClassifierConfigure.h to ${Dir}/."
    }  else {
        puts ""
        puts "========================= Warning ===================================== " 
        puts "Copied ${Dir}/../vtkEMAtlasBrainClassifierConfigure.h"
        puts "to ${Dir}/. successfull !"
        puts "The version of vtkEMAtlasBrainClassifierConfigure.h might be an old version !"  
        puts "If you get strange error messages compiling vtkEMLocalSegment do the following:"
        puts "- compile vtkEMAtlasBrainClassifierConfigure first "
        puts "- copy $SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/builds/$env(BUILD)/vtkEMAtlasBrainClassifierConfigure.h"
        puts "  to ${Dir}/../vtkEMAtlasBrainClassifierConfigure.h"
        puts "- remove ${Dir}"
        puts "- compile vtkEMLocalSegment"
        puts "If this fixes the compilation error please do a cvs commit"
        puts "to $SLICER_HOME/Modules/vtkEMLocalSegment/builds/vtkEMAtlasBrainClassifierConfigure.h" 
        puts "======================================================================= " 
        puts ""
    }
}

set Dir $SLICER_HOME/Modules/vtkEMLocalSegment/cxx

if {($tcl_platform(os) == "Windows NT") &&([file exists ${Dir}/vtkThread.h] == 0) } {
    if {[catch "file copy $SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/cxx/vtkThread.h ${Dir}/."] || [catch "file copy $SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/cxx/vtkThread.cxx ${Dir}/."]} {
    puts "Error: Failed copying $SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/cxx/vtkThread.* to ${Dir}/."
    }    else {
    puts "Copied $SLICER_HOME/Modules/vtkEMAtlasBrainClassifier/cxx/vtkThread.* to ${Dir}/."
    }
} 

