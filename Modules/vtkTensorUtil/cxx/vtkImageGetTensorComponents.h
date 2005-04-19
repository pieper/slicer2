/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkImageGetTensorComponents - Six scalar components from tensor
// .SECTION Description
// vtkImageGetTensorComponents gets from the tensor the 6 components and
// exports them as scalar components, ordered as follows:
// 1 2 3
// 2 4 5
// 3 5 6


// .SECTION See Also
// vtkImageSetTensorComponents

#ifndef __vtkImageGetTensorComponents_h
#define __vtkImageGetTensorComponents_h

#include "vtkTensorUtilConfigure.h"

#include "vtkImageToImageFilter.h"

class VTK_TENSORUTIL_EXPORT vtkImageGetTensorComponents : public vtkImageToImageFilter
{
public:
  static vtkImageGetTensorComponents *New();
  vtkTypeMacro(vtkImageGetTensorComponents,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the components to extract.
  vtkGetVector3Macro(Components,int);
  
  // Description:
  // Get the number of components to extract. This is set implicitly by the 
  // SetComponents() method.
  vtkGetMacro(NumberOfComponents,int);

protected:
  vtkImageGetTensorComponents();
  ~vtkImageGetTensorComponents() {};

  int NumberOfComponents;
  int Components[3];

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
                       int ext[6], int id);
private:
  vtkImageGetTensorComponents(const vtkImageGetTensorComponents&);  // Not implemented.
  void operator=(const vtkImageGetTensorComponents&);  // Not implemented.
};

#endif










