
set SLICER_MODULE_ARG1 "-DVTKTENSORUTIL_SOURCE_DIR:PATH=$SLICER_HOME/Modules/vtkTensorUtil"
set SLICER_MODULE_ARG2 "-DVTKTENSORUTIL_BUILD_DIR:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)" 

switch $tcl_platform(os) {
    "SunOS" {
       set SLICER_MODULE_ARG3 "-DVTKSLICERTENSOR_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/libvtkTensorUtil.so"
       set SLICER_MODULE_ARG4 "-DVTKSLICERTENSOR_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/libvtkTensorUtilTCL.so" 
     }
    "Linux" {
       set SLICER_MODULE_ARG3 "-DVTKSLICERTENSOR_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/libvtkTensorUtil.so"
       set SLICER_MODULE_ARG4 "-DVTKSLICERTENSOR_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/libvtkTensorUtilTCL.so" 
    }
    "Darwin" {
       set SLICER_MODULE_ARG3 "-DVTKSLICERTENSOR_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/libvtkTensorUtil.dylib"
       set SLICER_MODULE_ARG4 "-DVTKSLICERTENSOR_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/libvtkTensorUtilTCL.dylib" 
    }
    default {
         set SLICER_MODULE_ARG3 "-DVTKTENSORUTIL_BUILD_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/debug/vtkTensorUtil.lib"
         set SLICER_MODULE_ARG4 "-DVTKTENSORUTIL_BUILD_TCL_LIB:PATH=$SLICER_HOME/Modules/vtkTensorUtil/builds/$env(BUILD)/bin/debug/vtkTensorUtilTCL.lib" 

    } 
}
