/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
// .NAME vtkImageCloseUp2D -  Creates a magnified 2D image
// .SECTION Description
// vtkImageCloseUp2D shows a magnified square portion of a 2D image.

#ifndef __vtkImageCloseUp2D_h
#define __vtkImageCloseUp2D_h

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageCloseUp2D : public vtkImageToImageFilter
{
public:
  static vtkImageCloseUp2D *New();
  vtkTypeMacro(vtkImageCloseUp2D,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the Center of the window (X,Y) that we zoom in on.
  // Set the Magnification
  // Set the half-width of the region to zoom in on (radius)
  // The half-length is set to the same value.
  vtkSetMacro(X, int);
  vtkSetMacro(Y, int);
  vtkSetMacro(Radius, int);
  vtkSetMacro(Magnification, int);

  int Magnification;
  int Radius;
  int X;
  int Y;

protected:
  vtkImageCloseUp2D();
  ~vtkImageCloseUp2D() {};
  vtkImageCloseUp2D(const vtkImageCloseUp2D&) {};
  void operator=(const vtkImageCloseUp2D&) {};

  void ExecuteInformation(vtkImageData *inData, 
                                vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};

  // Override this function since inExt != outExt
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
                       int extent[6], int id);
};

#endif



