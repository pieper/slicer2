/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicer.h,v $
  Date:      $Date: 2006/02/27 19:21:52 $
  Version:   $Revision: 1.10 $

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
#include "vtkAlgorithm.h"
#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
// Helper functions for the VTK4.4 / VTK5 intermixed. We need to provide an API
// with vtkAlgorithm since this is the only common class in between the old ImageToImage filters
// and the new vtkImageAlgorithm one.
// Unfortunately there is no notion of Input or Output in vtkAlgorithm therefore
// we need to try/cast (guess) the real type of the filter in order to get the output:
inline vtkImageData *GetImageOutput(vtkAlgorithm *filter)
{
  // Is this a pure VTK class that derive from vtkImageAlgorithm
  vtkImageAlgorithm *ia = vtkImageAlgorithm::SafeDownCast(filter);
  if( ia )
    {
    return ia->GetOutput();
    }
  // else this is a class from Slicer which still derived from vtkImageToImageFilter old layer
  vtkImageToImageFilter *itoi = vtkImageToImageFilter::SafeDownCast(filter);
  if(itoi)
    {
    return itoi->GetOutput();
    }
  // else
  vtkGenericWarningMacro( "Problem executing GetImageOutput() " );
  return NULL;
}
inline void SetImageInput(vtkAlgorithm *filter, vtkImageData *output)
{
  // Is this a pure VTK class that derive from vtkImageAlgorithm
  vtkImageAlgorithm *ia =  vtkImageAlgorithm::SafeDownCast(filter);
  if( ia )
    {
    ia->SetInput(output);
    return;
    }
  // else this is a class from Slicer which still derived from vtkImageToImageFilter old layer
  vtkImageToImageFilter *itoi =  vtkImageToImageFilter::SafeDownCast(filter);
  if(itoi)
    {
    itoi->SetInput(output);
    return;
    }
  // else
  vtkGenericWarningMacro( "Problem executing SetImageInput() " );
}
#define vtkSlicerImageAlgorithm vtkAlgorithm
#define vtkSlicerImageAlgorithmCppCommand vtkImageAlgorithmCppCommand
#else
#include "vtkImageToImageFilter.h"
#include "vtkImageSpatialFilter.h"
// Dummy stub. See above
inline vtkImageData *GetImageOutput(vtkImageToImageFilter *filter)
{
  return filter->GetOutput();
}
inline void SetImageInput(vtkImageToImageFilter *filter, vtkImageData *output)
{
  filter->SetInput(output);
}
#define vtkSlicerImageAlgorithm vtkImageToImageFilter
#define vtkSlicerImageSpatialAlgorithm vtkImageSpatialFilter
#define vtkSlicerImageAlgorithmCommand vtkImageToImageFilterCommand
#define vtkSlicerImageAlgorithmCppCommand vtkImageToImageFilterCppCommand
#define vtkSlicerImageSpatialAlgorithmCppCommand vtkImageSpatialFilterCppCommand
#endif

#endif
