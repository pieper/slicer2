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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkStreamlineConvolve.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkStreamlineConvolve - extracts points whose scalar value satisfies threshold criterion
// .SECTION Description
// vtkStreamlineConvolve is a filter that extracts points from a dataset that 
// satisfy a threshold criterion. The criterion can take three forms:
// 1) greater than a particular value; 2) less than a particular value; or
// 3) between a particular value. The output of the filter is polygonal data.
// .SECTION See Also
// vtkThreshold

#ifndef __vtkStreamlineConvolve_h
#define __vtkStreamlineConvolve_h

#include "vtkDTMRIConfigure.h"

#include <vtkStructuredPointsToPolyDataFilter.h>
#include "vtkDoubleArray.h"

#include "vtkPolyData.h"

class VTK_DTMRI_EXPORT vtkStreamlineConvolve : public vtkStructuredPointsToPolyDataFilter
{
public:
  static vtkStreamlineConvolve *New();
  vtkTypeRevisionMacro(vtkStreamlineConvolve,vtkStructuredPointsToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
    // Description:
  // Get the kernel size
  vtkGetVector3Macro(KernelSize, int);

  // Description:
  // Set the kernel to be a given 3x3 or 5x5 or 7x7 kernel.
  void SetKernel3x3(vtkDoubleArray* kernel);
  void SetKernel5x5(vtkDoubleArray* kernel);
  void SetKernel7x7(vtkDoubleArray* kernel);

  // Description:
  // Set the kernel to be a 3x3x3 or 5x5x5 or 7x7x7 kernel.
  void SetKernel3x3x3(vtkDoubleArray* kernel);
  void SetKernel5x5x5(vtkDoubleArray* kernel);
  void SetKernel7x7x7(vtkDoubleArray* kernel);

  // Description:
  // Return an array that contains the kernel
  void GetKernel(vtkDoubleArray *kernel); 
  double* GetKernel();

  // Description:
  // Set/Get the Streamline to convolve with.
  vtkSetObjectMacro(Streamlines,vtkPolyData);
  vtkGetObjectMacro(Streamlines,vtkPolyData); 
   

protected:
  vtkStreamlineConvolve();
  ~vtkStreamlineConvolve() {};

  // Usual data generation method
  void Execute();
  
  void SetKernel(vtkDoubleArray* kernel,
                 int sizeX, int sizeY, int sizeZ);
  

  int KernelSize[3];
  double Kernel[343];
  vtkPolyData *Streamlines;

private:
  vtkStreamlineConvolve(const vtkStreamlineConvolve&);  // Not implemented.
  void operator=(const vtkStreamlineConvolve&);  // Not implemented.
};

#endif
