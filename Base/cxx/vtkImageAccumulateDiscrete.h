/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageAccumulateDiscrete.h,v $
  Date:      $Date: 2006/02/22 23:47:15 $
  Version:   $Revision: 1.18 $

=========================================================================auto=*/
// .NAME vtkImageAccumulateDiscrete - Generalized histograms upto 4 dimensions.
// .SECTION Description
// vtkImageAccumulateDiscrete - This filter divides component space into
// discrete bins.  It then counts the number of pixels associated
// with each bin.  The output is this "scatter plot".
// The input can be any type, but the output is always int.


#ifndef __vtkImageAccumulateDiscrete_h
#define __vtkImageAccumulateDiscrete_h


#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageAccumulateDiscrete : public vtkImageToImageFilter
{
public:
  static vtkImageAccumulateDiscrete *New();
  vtkTypeMacro(vtkImageAccumulateDiscrete,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageAccumulateDiscrete();
  ~vtkImageAccumulateDiscrete() {};
  vtkImageAccumulateDiscrete(const vtkImageAccumulateDiscrete&);
  void operator=(const vtkImageAccumulateDiscrete&);

  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteData(vtkDataObject *);

};

#endif



