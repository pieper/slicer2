/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicer.h,v $
  Date:      $Date: 2005/12/20 22:44:35 $
  Version:   $Revision: 1.5.2.1 $

=========================================================================auto=*/

/*
 * This is needed for loading slicer code as module.
 * Added here to isolate the changes to the main files in case
 * further mods are needed.
 * - sp 2002-04-19
 */


#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

//
// use an ifdef on SLICER_VTK5 to flag code that won't
// compile on vtk4.4 and before
//
#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
#define SLICER_VTK5
#endif

#include "vtkSlicerBaseWin32Header.h"
