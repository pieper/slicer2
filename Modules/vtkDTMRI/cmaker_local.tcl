
set SLICER_MODULE_ARG "-DVTKTENSORUTIL_SOURCE_DIR:PATH=$SLICER_HOME/Modules/vtkTensorUtil"
lappend SLICER_MODULE_ARG "-DVTKTENSORUTIL_BUILD_DIR:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)" 

switch $tcl_platform(os) {
    "SunOS" {
       lappend SLICER_MODULE_ARG "-DVTKSLICERTENSOR_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/libvtkTensorUtil.so"
       lappend SLICER_MODULE_ARG "-DVTKSLICERTENSOR_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/libvtkTensorUtilTCL.so" 
     }
    "Linux" {
       lappend SLICER_MODULE_ARG "-DVTKSLICERTENSOR_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/libvtkTensorUtil.so"
       lappend SLICER_MODULE_ARG "-DVTKSLICERTENSOR_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/libvtkTensorUtilTCL.so" 
    }
    "Darwin" {
       lappend SLICER_MODULE_ARG "-DVTKSLICERTENSOR_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/libvtkTensorUtil.dylib"
       lappend SLICER_MODULE_ARG "-DVTKSLICERTENSOR_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/libvtkTensorUtilTCL.dylib" 
    }
    default {
         lappend SLICER_MODULE_ARG "-DVTKTENSORUTIL_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/debug/vtkTensorUtil.lib"
         lappend SLICER_MODULE_ARG "-DVTKTENSORUTIL_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$::env(BUILD)/bin/debug/vtkTensorUtilTCL.lib" 
    } 
}
