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
