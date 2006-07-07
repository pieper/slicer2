/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTensorFlip.h,v $
  Date:      $Date: 2006/07/07 19:39:36 $
  Version:   $Revision: 1.2.2.1.2.3 $

=========================================================================auto=*/
// .NAME vtkTensorFlip - flip Y axis and negate xy and zy terms
// .SECTION Description
// Make tend estim generated vtk files compatible with slicer
//


#ifndef __vtkTensorFlip_h
#define __vtkTensorFlip_h


#include "vtkTensorUtilConfigure.h"
#include "vtkImageToImageFilter.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"

class VTK_TENSORUTIL_EXPORT vtkTensorFlip : public vtkImageToImageFilter
{
public:
  static vtkTensorFlip *New();
  vtkTypeMacro(vtkTensorFlip,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkFloatArray *GetOutTensors() { return (this->OutTensors); }
  

protected:
  vtkTensorFlip();
  ~vtkTensorFlip() {};
  vtkTensorFlip(const vtkTensorFlip&);
  void operator=(const vtkTensorFlip&);

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
        int extent[6], int id);

  // We override this in order to allocate output tensors
  // before threading happens.  This replaces the superclass 
  // vtkImageMultipleInputFilter's Execute function.
  void ExecuteData(vtkDataObject *out);

  // need to store our own copy of the tensors during the pipeline execution, because
  // vtk will copy the input tensors over to the output by default
  vtkFloatArray *OutTensors;
};

#endif













