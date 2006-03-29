/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTensorMask.h,v $
  Date:      $Date: 2006/03/29 14:53:28 $
  Version:   $Revision: 1.7 $

=========================================================================auto=*/
// .NAME vtkTensorMask - Combines a mask and an image.
// .SECTION Description
// vtkTensorMask combines a mask with an image.  Non zero mask
// implies the output pixel will be the same as the image.
// If a mask pixel is zero,  the the output pixel
// is set to "MaskedValue".  The filter also has the option to pass
// the mask through a boolean not operation before processing the image.
// This reverses the passed and replaced pixels.
// The two inputs should have the same "WholeExtent".
// The mask input should be unsigned char, and the image scalar type
// is the same as the output scalar type.


#ifndef __vtkTensorMask_h
#define __vtkTensorMask_h

#include "vtkTensorUtilConfigure.h"
#include "vtkImageToImageFilter.h"

class VTK_TENSORUTIL_EXPORT vtkTensorMask : public vtkImageToImageFilter
{
public:
  static vtkTensorMask *New();
  vtkTypeMacro(vtkTensorMask,vtkImageMask);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkTensorMask();
  ~vtkTensorMask();
  vtkTensorMask(const vtkTensorMask&);
  void operator=(const vtkTensorMask&);

  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData, int extent[6], int id);

  // We override this in order to allocate output tensors
  // before threading happens.  This replaces the superclass 
  // vtkImageMultipleInputFilter's Execute function.
  void ExecuteData(vtkDataObject *out);
};

#endif



