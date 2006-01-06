/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageDouble2D.h,v $
  Date:      $Date: 2006/01/06 17:56:40 $
  Version:   $Revision: 1.17 $

=========================================================================auto=*/
// .NAME vtkImageDouble2D -  Doubles size of input image
// .SECTION Description
// Used in the slicer to convert 256 by 256 size reformatted images
// into 512 by 512 size images.  Uses pixel duplication, not interpolation.

#ifndef __vtkImageDouble2D_h
#define __vtkImageDouble2D_h

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"

#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageDouble2D : public vtkImageToImageFilter
{
public:    
  static vtkImageDouble2D *New();
  vtkTypeMacro(vtkImageDouble2D,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageDouble2D();
  ~vtkImageDouble2D(){};
  vtkImageDouble2D(const vtkImageDouble2D&) {};
  void operator=(const vtkImageDouble2D&) {};

  // Override this function since inExt != outExt
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  
  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
                       int extent[6], int id);
};

#endif



