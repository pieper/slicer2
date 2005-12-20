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
// .NAME vtkImageMosaik - Make a mosaik from 2 image inputs
// .SECTION Description
// vtkImageMosaik takes 2 inputs images and merges
// them into one output (the mosaik). All inputs must have the same extent,
// scalar type, and number of components.

#ifndef __vtkImageMosaik_h
#define __vtkImageMosaik_h

#include "vtkCompareModuleConfigure.h"

#include "vtkImageData.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkSlicer.h"

class VTK_COMPAREMODULE_EXPORT vtkImageMosaik : public vtkImageMultipleInputFilter
{
public:
  static vtkImageMosaik *New();
  vtkTypeMacro(vtkImageMosaik,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Sets the opacity used to overlay this layer on the others
  double GetOpacity();
  void SetOpacity(double opacity);

  // Sets the mosaik width and height
  int GetDivisionWidth();
  int GetDivisionHeight();
  void SetDivisionWidth(int width);
  void SetDivisionHeight(int height);

protected:
  vtkImageMosaik();
  ~vtkImageMosaik();
  vtkImageMosaik(const vtkImageMosaik&) {};
  void operator=(const vtkImageMosaik&) {};

  double opacity;
  int divisionWidth;
  int divisionHeight;

  void ExecuteData(vtkDataObject *data);
};

#endif
