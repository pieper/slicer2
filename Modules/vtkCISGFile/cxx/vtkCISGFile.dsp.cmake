# Microsoft Developer Studio Project File - Name="vtkCISGFile" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vtkCISGFile - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vtkCISGFile.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vtkCISGFile.mak" CFG="vtkCISGFile - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vtkCISGFile - Win32 MinSizeRel" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vtkCISGFile - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vtkCISGFile - Win32 RelWithDebInfo" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vtkCISGFile - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
#  /I "." /I "E:\VTK40\include" /I "F:\project\vtkCISG\include" /I "F:\project\vtkCISG\objects\library\CISGFile" /I "F:\project\vtkCISG\objects\library\CISGProcessing" /I "F:\project\vtkCISG\objects\library\CISGTransform" /I "F:\project\vtkCISG\objects\library\CISGVisualization" /I "F:\project\vtkCISG\objects\library\CISGRegistration" /I "E:\VTK40\include\vtk" == include path
#   -DVTK_WITHOUT_PATENTED == compiler defines
# F:\project\vtkCISG\lib\ == override in output directory
# vtkCISGFile  == name of output library

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
        
!IF  "$(CFG)" == "vtkCISGFile - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "F:\project\vtkCISG\lib\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "vtkCISGFile_EXPORTS" /FD /c
# ADD CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL"  /FD /c
# ADD CPP  /I "." /I "E:\VTK40\include" /I "F:\project\vtkCISG\include" /I "F:\project\vtkCISG\objects\library\CISGFile" /I "F:\project\vtkCISG\objects\library\CISGProcessing" /I "F:\project\vtkCISG\objects\library\CISGTransform" /I "F:\project\vtkCISG\objects\library\CISGVisualization" /I "F:\project\vtkCISG\objects\library\CISGRegistration" /I "E:\VTK40\include\vtk"   -DVTK_WITHOUT_PATENTED /D "vtkCISGFile_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2 -DCMAKE_INTDIR=\"Release\"
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32   /LIBPATH:F:\project\vtkCISG\lib\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\lib\  /LIBPATH:F:\project\vtkCISG\bin\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\bin\  /LIBPATH:.\$(INTDIR)  /LIBPATH:.\  /LIBPATH:E:\VTK40\lib\vtk\$(INTDIR)  /LIBPATH:E:\VTK40\lib\vtk\  vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkCommon.lib vtkImaging.lib vtkParallel.lib vtkRendering.lib vtkGraphics.lib vtkHybrid.lib vtkFiltering.lib vtkIO.lib vtkCommon.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkFilteringTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkIOTCL.lib /STACK:10000000  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386



!ELSEIF  "$(CFG)" == "vtkCISGFile - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "F:\project\vtkCISG\lib\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "vtkCISGFile_EXPORTS" /FD /c
# ADD CPP /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL"  /FD /c
# ADD CPP  /I "." /I "E:\VTK40\include" /I "F:\project\vtkCISG\include" /I "F:\project\vtkCISG\objects\library\CISGFile" /I "F:\project\vtkCISG\objects\library\CISGProcessing" /I "F:\project\vtkCISG\objects\library\CISGTransform" /I "F:\project\vtkCISG\objects\library\CISGVisualization" /I "F:\project\vtkCISG\objects\library\CISGRegistration" /I "E:\VTK40\include\vtk"   -DVTK_WITHOUT_PATENTED /D "vtkCISGFile_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ -DCMAKE_INTDIR=\"Debug\"
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32   /LIBPATH:F:\project\vtkCISG\lib\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\lib\  /LIBPATH:F:\project\vtkCISG\bin\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\bin\  /LIBPATH:.\$(INTDIR)  /LIBPATH:.\  /LIBPATH:E:\VTK40\lib\vtk\$(INTDIR)  /LIBPATH:E:\VTK40\lib\vtk\  vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkCommon.lib vtkImaging.lib vtkParallel.lib vtkRendering.lib vtkGraphics.lib vtkHybrid.lib vtkFiltering.lib vtkIO.lib vtkCommon.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkFilteringTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkIOTCL.lib /STACK:10000000  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept



!ELSEIF  "$(CFG)" == "vtkCISGFile - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "F:\project\vtkCISG\lib\MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL"  /FD /c
# ADD CPP  /I "." /I "E:\VTK40\include" /I "F:\project\vtkCISG\include" /I "F:\project\vtkCISG\objects\library\CISGFile" /I "F:\project\vtkCISG\objects\library\CISGProcessing" /I "F:\project\vtkCISG\objects\library\CISGTransform" /I "F:\project\vtkCISG\objects\library\CISGVisualization" /I "F:\project\vtkCISG\objects\library\CISGRegistration" /I "E:\VTK40\include\vtk"   -DVTK_WITHOUT_PATENTED /D "vtkCISGFile_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\"
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 
# ADD LINK32   /LIBPATH:F:\project\vtkCISG\lib\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\lib\  /LIBPATH:F:\project\vtkCISG\bin\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\bin\  /LIBPATH:.\$(INTDIR)  /LIBPATH:.\  /LIBPATH:E:\VTK40\lib\vtk\$(INTDIR)  /LIBPATH:E:\VTK40\lib\vtk\  vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkCommon.lib vtkImaging.lib vtkParallel.lib vtkRendering.lib vtkGraphics.lib vtkHybrid.lib vtkFiltering.lib vtkIO.lib vtkCommon.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkFilteringTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkIOTCL.lib /STACK:10000000  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386



!ELSEIF  "$(CFG)" == "vtkCISGFile - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0         
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "F:\project\vtkCISG\lib\RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "vtkCISGFile_EXPORTS" /FD /c
# ADD CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL"  /FD /c
# ADD CPP  /I "." /I "E:\VTK40\include" /I "F:\project\vtkCISG\include" /I "F:\project\vtkCISG\objects\library\CISGFile" /I "F:\project\vtkCISG\objects\library\CISGProcessing" /I "F:\project\vtkCISG\objects\library\CISGTransform" /I "F:\project\vtkCISG\objects\library\CISGVisualization" /I "F:\project\vtkCISG\objects\library\CISGRegistration" /I "E:\VTK40\include\vtk"   -DVTK_WITHOUT_PATENTED /D "vtkCISGFile_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\"
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /pdbtype:sept
# ADD LINK32   /LIBPATH:F:\project\vtkCISG\lib\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\lib\  /LIBPATH:F:\project\vtkCISG\bin\$(INTDIR)  /LIBPATH:F:\project\vtkCISG\bin\  /LIBPATH:.\$(INTDIR)  /LIBPATH:.\  /LIBPATH:E:\VTK40\lib\vtk\$(INTDIR)  /LIBPATH:E:\VTK40\lib\vtk\  vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkFilteringTCL.lib vtkIOTCL.lib vtkCommonTCL.lib vtkCommon.lib vtkImaging.lib vtkParallel.lib vtkRendering.lib vtkGraphics.lib vtkHybrid.lib vtkFiltering.lib vtkIO.lib vtkCommon.lib vtkCommonTCL.lib vtkImagingTCL.lib vtkParallelTCL.lib vtkRenderingTCL.lib vtkFilteringTCL.lib vtkGraphicsTCL.lib vtkHybridTCL.lib vtkIOTCL.lib /STACK:10000000  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept



!ENDIF 

# Begin Target

# Name "vtkCISGFile - Win32 Release"
# Name "vtkCISGFile - Win32 Debug"
# Name "vtkCISGFile - Win32 MinSizeRel"
# Name "vtkCISGFile - Win32 RelWithDebInfo"

# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt

!IF  "$(CFG)" == "vtkCISGFile - Win32 Release"
USERDEP__HACK=\
	"E:\Program Files\CMake\Modules\FindTCL.cmake"\
	"E:\Program Files\CMake\Modules\FindTclsh.cmake"\
	"E:\Program Files\CMake\Modules\FindWish.cmake"\
	"E:\Program Files\CMake\Templates\CMakeWindowsSystemConfig.cmake"\
	F:\project\vtkCISG\CMakeLists.txt\
	F:\project\vtkCISG\objects\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CMakeLists.txt\
	F:\project\vtkCISG\objects\vtkCISGInclude.cmake
# PROP Ignore_Default_Tool 1
# Begin Custom Build

"vtkCISGFile.dsp.cmake" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"E:\Program Files\CMake\bin\cmake.exe" F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt -HF:\project\vtkCISG -SF:\project\vtkCISG\objects\library\CISGFile -OF:\project\vtkCISG\objects\library\CISGFile -BF:\project\vtkCISG

# End Custom Build

!ELSEIF  "$(CFG)" == "vtkCISGFile - Win32 Debug"
USERDEP__HACK=\
	"E:\Program Files\CMake\Modules\FindTCL.cmake"\
	"E:\Program Files\CMake\Modules\FindTclsh.cmake"\
	"E:\Program Files\CMake\Modules\FindWish.cmake"\
	"E:\Program Files\CMake\Templates\CMakeWindowsSystemConfig.cmake"\
	F:\project\vtkCISG\CMakeLists.txt\
	F:\project\vtkCISG\objects\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CMakeLists.txt\
	F:\project\vtkCISG\objects\vtkCISGInclude.cmake
# PROP Ignore_Default_Tool 1
# Begin Custom Build

"vtkCISGFile.dsp.cmake" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"E:\Program Files\CMake\bin\cmake.exe" F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt -HF:\project\vtkCISG -SF:\project\vtkCISG\objects\library\CISGFile -OF:\project\vtkCISG\objects\library\CISGFile -BF:\project\vtkCISG

# End Custom Build

!ELSEIF  "$(CFG)" == "vtkCISGFile - Win32 MinSizeRel"
USERDEP__HACK=\
	"E:\Program Files\CMake\Modules\FindTCL.cmake"\
	"E:\Program Files\CMake\Modules\FindTclsh.cmake"\
	"E:\Program Files\CMake\Modules\FindWish.cmake"\
	"E:\Program Files\CMake\Templates\CMakeWindowsSystemConfig.cmake"\
	F:\project\vtkCISG\CMakeLists.txt\
	F:\project\vtkCISG\objects\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CMakeLists.txt\
	F:\project\vtkCISG\objects\vtkCISGInclude.cmake
# PROP Ignore_Default_Tool 1
# Begin Custom Build

"vtkCISGFile.dsp.cmake" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"E:\Program Files\CMake\bin\cmake.exe" F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt -HF:\project\vtkCISG -SF:\project\vtkCISG\objects\library\CISGFile -OF:\project\vtkCISG\objects\library\CISGFile -BF:\project\vtkCISG

# End Custom Build

!ELSEIF  "$(CFG)" == "vtkCISGFile - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"E:\Program Files\CMake\Modules\FindTCL.cmake"\
	"E:\Program Files\CMake\Modules\FindTclsh.cmake"\
	"E:\Program Files\CMake\Modules\FindWish.cmake"\
	"E:\Program Files\CMake\Templates\CMakeWindowsSystemConfig.cmake"\
	F:\project\vtkCISG\CMakeLists.txt\
	F:\project\vtkCISG\objects\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt\
	F:\project\vtkCISG\objects\library\CMakeLists.txt\
	F:\project\vtkCISG\objects\vtkCISGInclude.cmake
# PROP Ignore_Default_Tool 1
# Begin Custom Build

"vtkCISGFile.dsp.cmake" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"E:\Program Files\CMake\bin\cmake.exe" F:\project\vtkCISG\objects\library\CISGFile\CMakeLists.txt -HF:\project\vtkCISG -SF:\project\vtkCISG\objects\library\CISGFile -OF:\project\vtkCISG\objects\library\CISGFile -BF:\project\vtkCISG

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGAnalyzeReader.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGAnalyzeWriter.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGCifstream.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGCofstream.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGGiplReader.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGGiplWriter.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGInterfileReader.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGInterfileWriter.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGMultiReader.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGMultiWriter.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGRREPReader.cxx

# End Source File
# Begin Source File

SOURCE=F:\project\vtkCISG\objects\library\CISGFile\vtkCISGVersion.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

