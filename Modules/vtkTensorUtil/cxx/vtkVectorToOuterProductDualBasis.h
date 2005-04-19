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
// .NAME vtkVectorToOuterProductDualBasis 
// .SECTION Description
//  Implementation in VTK of C-F Westin's method 
//  for calculating a dual basis.

#ifndef __vtkVectorToOuterProductDualBasis_h
#define __vtkVectorToOuterProductDualBasis_h

#include "vtkTensorUtilConfigure.h"
#include "vtkObject.h"

class VTK_TENSORUTIL_EXPORT vtkVectorToOuterProductDualBasis : public vtkObject
{
public:
  static vtkVectorToOuterProductDualBasis *New();
  vtkTypeMacro(vtkVectorToOuterProductDualBasis,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // The number of input vectors to use when creating the
  // outer products.  Call this before setting input
  // vectors since it does the allocation.
  void SetNumberOfInputVectors(int num);
  vtkGetMacro(NumberOfInputVectors,int);

  // Description:
  // Set number "num" input vector.
  void SetInputVector(int num, vtkFloatingPointType vector[3]);
  void SetInputVector(int num, vtkFloatingPointType v0, vtkFloatingPointType v1, vtkFloatingPointType v2);
  
  // Description:
  // Get number "num" input vector.
  vtkFloatingPointType *GetInputVector(int num)
    {
      return this->V[num];
    };

  // Description:
  // Calculate output based on input vectors.
  void CalculateDualBasis();

  // Description:
  // Access the output of this class (after calling
  // CalculateDualBasis to calculate this from input vectors).
  // The matrix dimensions are NumberOfInputVectorsx9.
  double **GetPseudoInverse() {return this->PInv;}

  // Description:
  // Print the PseudoInverse matrix, this->Pinv
  void PrintPseudoInverse(ostream &os);
  // for access from tcl, just dumps to stdout for now
  void PrintPseudoInverse();

   // Description:
   // Singular value decomposition of a mxn matrix.
  // SVD is given by: A = U W V^T;
  // The input arguments are: mxn matrix a, m  and n.
  // The outpur arguments are: w diagonal of matrix W, v matrix and
  // U replaces a on output.
  static int SVD(float **a, int m,int n, float *w, float **v);
  static int SVD(double **a, int m,int n, double *w, double **v);

  // Description:
  // PseudoInverse of a mxn matrix.
  static int PseudoInverse(double **A, double **AI, int m, int n);

protected:
  vtkVectorToOuterProductDualBasis();
  ~vtkVectorToOuterProductDualBasis();
  vtkVectorToOuterProductDualBasis(const vtkVectorToOuterProductDualBasis&) {};

  int NumberOfInputVectors;

  vtkFloatingPointType **V;
  double **VV;
  double **VVT;

  double **VVTVV;
  double **VVTVVI;
  double **PInv;

  void AllocateInternals();
  void DeallocateInternals();

  void operator=(const vtkVectorToOuterProductDualBasis&) {};
};

#endif
