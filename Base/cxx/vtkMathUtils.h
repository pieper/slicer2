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
#ifndef __vtkMathUtils_h
#define __vtkMathUtils_h

#include "vtkObject.h"
#include "vtkPoints.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMathUtils : public vtkObject
{
  public:
  static vtkMathUtils *New() {return new vtkMathUtils;};
  const char *GetClassName() {return "vtkMathUtils";};
  
  static int PrincipalMomentsAndAxes( vtkPoints *Points, vtkDataArray *Weights,
                                      vtkDataArray *Values, vtkDataArray *Vectors );
  static int AlignPoints( vtkPoints *Data, vtkPoints *Ref,
                          vtkMatrix4x4 *Xform );
  static void SVD3x3( float A[][3], float U[][3], float W[], float V[][3] );
  
  // Description:
  // Outer product of two 3-vectors.
  static void Outer3(float x[3], float y[3], float A[3][3]);

  // Description:
  // Outer product of two 2-vectors.
  static void Outer2(float x[2], float y[2], float A[2][2]);

  // Description:
  // General matrix multiplication.  You must allocate
  // output storage.
  static void MatrixMultiply(double **A, double **B, double **C, int rowA, 
                 int colA, int rowB, int colB);
  // Description:
  // Dump matrix contents to cout
  static void PrintMatrix(double **A, int rowA, int colA);
  // Description:
  // For use in PrintSelf Methods 
  static void PrintMatrix(double **A, int rowA, int colA, ostream& os, 
                          vtkIndent indent);
};

#endif
