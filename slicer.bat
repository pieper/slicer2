
set TCL_DIR=.
set ITCL_LIBRARY=c:\pieper\bwh\slicer\Tcl-8.3\lib\itcl3.2

set Path=Base\builds\Win32VC7\bin\debug;%Path%
set Path=c:\downloads\vtk40\VTK-build\bin\debug;%Path%
set Path=c:\pieper\bwh\slicer\Tcl-8.3\bin;%Path%

set TCLLIBPATH=Base/Wrapping/Tcl/vtkSlicerBase %TCLLIBPATH%
set TCLLIBPATH=c:/downloads/vtk40/VTK/Wrapping/Tcl %TCLLIBPATH%
set TCLLIBPATH=Base/tcl %TCLLIBPATH%

set VTK_DATA_ROOT=c:/downloads/vtk40/VTKData

start c:\pieper\bwh\slicer\Tcl-8.3\bin/wish83.exe Base/tcl/Go.tcl %1 %2 %3 %4 %5 %6 %7 %8 %9

rem ####################### for testing ##################
rem start c:\downloads\vtk40\VTK-build\bin\debug\vtk.exe Base/tcl/Go.tcl %1 %2 %3 %4 %5 %6 %7 %8 %9

rem start c:\downloads\vtk40\VTK-build\bin\debug\vtk.exe c:/pieper/test.tcl %1
rem
rem #######################             ##################

exit

