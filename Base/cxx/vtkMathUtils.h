/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#ifndef __vtkMathUtils_h
#define __vtkMathUtils_h

#include "vtkObject.h"
#include "vtkPoints.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"

class VTK_EXPORT vtkMathUtils : public vtkObject
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
};

#endif
