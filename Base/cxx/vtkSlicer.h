/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicer.h,v $
  Date:      $Date: 2006/02/22 22:54:50 $
  Version:   $Revision: 1.7 $

=========================================================================auto=*/

/*
 * This is needed for loading slicer code as module.
 * Added here to isolate the changes to the main files in case
 * further mods are needed.
 * - sp 2002-04-19
 */

#ifndef __vtkSlicer_h
#define __vtkSlicer_h

#include "vtkSystemIncludes.h"

#ifndef vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

//
// use an ifdef on SLICER_VTK5 to flag code that won't
// compile on vtk4.4 and before
//
#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
#define SLICER_VTK5
#endif


#include "vtkSlicerBaseWin32Header.h"

// This is a trick to work around a feature in the current vtkWrapTcl
// This should be removed once slicer fully switch to VTK 5.x execution style
#ifdef SLICER_VTK5
#include "vtkImageAlgorithm.h"
#include "vtkImageSpatialAlgorithm.h"
#define vtkSlicerImageAlgorithm vtkImageAlgorithm
#define vtkSlicerImageSpatialAlgorithm vtkImageSpatialAlgorithm
#else
#include "vtkImageToImageFilter.h"
#include "vtkImageSpatialFilter.h"
#define vtkSlicerImageAlgorithm vtkImageToImageFilter
#define vtkSlicerImageSpatialAlgorithm vtkImageSpatialFilter
#endif

#endif
