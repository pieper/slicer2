@echo off

::
:: startup batch file for Slicer2 on Windows
::

IF EXIST %SLICER_HOME% GOTO startup
SET SLICER_HOME=c:\pieper\bwh\slicer2\porting\slicer2
:startup

:: locations:
set TCL_BIN_DIR=c:\pieper\bwh\slicer\Tcl-8.3\bin
set VTK_BIN_DIR=c:\downloads\vtk40\VTK-build\bin\debug
set VTK_SRC_DIR=c:\downloads\vtk40\VTK

:: Windows system paths - fill these in with values appropriate for system
:: - these are the locations with the .dll files you want to use
:: - the Path entries use BACKSLASHES separated by SEMICOLONS
::
:: first the vtk
set Path=%SLICER_HOME%\Base\builds\Win32VC7\bin\debug;%Path%
:: then vtkSlicerBase
set Path=%VTK_BIN_DIR%;%Path%
:: then tcl and tk
set Path=%TCL_BIN_DIR%;%Path%


set ITCL_LIBRARY=c:\pieper\bwh\slicer\Tcl-8.3\lib\itcl3.2

set VTK_DATA_ROOT=c:/downloads/vtk40/VTKData

IF .%1==.-test GOTO test

start wish83.exe %SLICER_HOME%/Base/tcl/Go.tcl %1 %2 %3 %4 %5 %6 %7 %8 %9

exit

:: ####################### for testing ##################

:: launch slicer from pre-linked vtk rather than loading packages
:: from dlls

:: start %VTK_BIN_DIR%\vtk.exe Base/tcl/Go.tcl %1 %2 %3 %4 %5 %6 %7 %8 %9

:test
:: run just a test script with tkcon
::
::start %VTK_BIN_DIR%\vtk.exe Base/tcl/test-util.tcl %2 %3 %4 %5 %6 %7 %8 %9
start wish83.exe %SLICER_HOME%/Base/tcl/test-util.tcl %2 %3 %4 %5 %6 %7 %8 %9
::
:: #######################             ##################

exit

