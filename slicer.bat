
set TCL_DIR=.
set ITCL_LIBRARY=c:\pieper\bwh\slicer\Tcl-8.3\lib\itcl3.2

set Path=Base\builds\Win32VC7\bin\debug;c:\downloads\vtk40\VTK-build\bin\debug;%Path%
set TCLLIBPATH=Base/Wrapping/Tcl/vtkSlicerBase c:/downloads/vtk40/VTK/Wrapping/Tcl Base/tcl %TCLLIBPATH%


start c:\pieper\bwh\slicer\Tcl-8.3\bin/wish83.exe Base/tcl/Go.tcl %1 %2 %3 %4 %5 %6 %7 %8 %9
exit

