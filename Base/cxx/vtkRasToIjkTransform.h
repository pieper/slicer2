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
// .NAME vtkRasToIjkTransform - linear transformations for medical data
// .SECTION Description
// vtkRasToIjkTransform provides a generic interface for linear 
// (affine or 12 degree-of-freedom) geometric transformations
// that convert coordinates from the metric RAS space to the voxel IJK space.
// This class needs to be derived from vtkLinearTransform in the future.
// .SECTION see also
// vtkLinearTransform

// These types are returned by GetSliceOrder
#define SLICE_ORDER_IS         0
#define SLICE_ORDER_SI         1 
#define SLICE_ORDER_LR         2
#define SLICE_ORDER_RL         3
#define SLICE_ORDER_PA         4
#define SLICE_ORDER_AP         5

#ifndef __vtkRasToIjkTransform_h
#define __vtkRasToIjkTransform_h

#include "vtkVector3.h"
#include "vtkMatrix4x4.h"
#include "vtkObject.h"

class VTK_EXPORT vtkRasToIjkTransform : public vtkObject
{
public:
  // VTK requisites
  static vtkRasToIjkTransform *New();
  vtkTypeMacro(vtkRasToIjkTransform,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Apply the transformation to a point.
  // You can use the same array to store both the input and output.
  void RasToIjkTransformPoint(const float in[3], float out[3]);
  void IjkToRasTransformPoint(const float in[3], float out[3]);

  // Description:
  // Apply the transformation to a double-precision point.
  // You can use the same array to store both the input and output.
  void RasToIjkTransformPoint(const double in[3], double out[3]);
  void IjkToRasTransformPoint(const double in[3], double out[3]);

  // Description:
  // Apply the transformation to a vector.
  // You can use the same array to store both the input and output.
  void RasToIjkTransformVector3(vtkVector3 *in, vtkVector3 *out);
  void IjkToRasTransformVector3(vtkVector3 *in, vtkVector3 *out);

  //--------------------------------------------------------------------------
  // IJK vs RAS
  //--------------------------------------------------------------------------

  vtkSetVector6Macro(Extent, int);
  vtkGetVector6Macro(Extent, int);

  vtkSetVector3Macro(Spacing, float);
  vtkGetVector3Macro(Spacing, float);

  void SetSlicerMatrix(vtkMatrix4x4 *m) {this->SlicerMatrix->DeepCopy(m);};

  vtkGetObjectMacro(SlicerMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(RasToIjk, vtkMatrix4x4);
  vtkGetObjectMacro(IjkToRas, vtkMatrix4x4);

  // The scan order of slices. One of: IS, SI, LR, RL, PA, AP
  vtkSetMacro(SliceOrder, int);
  vtkGetMacro(SliceOrder, int);
  const char *GetSliceOrderAsString() { 
    return ((this->SliceOrder == SLICE_ORDER_IS) ? "IS" : \
           ((this->SliceOrder == SLICE_ORDER_SI) ? "SI" : \
           ((this->SliceOrder == SLICE_ORDER_LR) ? "LR" : \
           ((this->SliceOrder == SLICE_ORDER_RL) ? "RL" : \
           ((this->SliceOrder == SLICE_ORDER_PA) ? "PA" : \
           ((this->SliceOrder == SLICE_ORDER_AP) ? "AP" : \
           "Undefined"))))));};
  void SetSliceOrderAsString(const char *s);
  void SetSliceOrderToIS() {this->SetSliceOrder(SLICE_ORDER_IS);};
  void SetSliceOrderToSI() {this->SetSliceOrder(SLICE_ORDER_SI);};
  void SetSliceOrderToLR() {this->SetSliceOrder(SLICE_ORDER_LR);};
  void SetSliceOrderToRL() {this->SetSliceOrder(SLICE_ORDER_RL);};
  void SetSliceOrderToPA() {this->SetSliceOrder(SLICE_ORDER_PA);};
  void SetSliceOrderToAP() {this->SetSliceOrder(SLICE_ORDER_AP);};

  // Update RAS<->IJK transforms. Call after change to Extent,Spacing,Order,Matrix.

  // Description:
  // Uses Extent Spacing, SliceOrder.
  // Computes Corners, Transforms
  void ComputeCorners();

  // Description:
  // Uses Extent, Spacing, SlicerMatrix.
  // Computes Corners, SliceOrder, Transforms.
  void ComputeCornersFromSlicerMatrix();

  // Description:
  // Uses ExtentExtent, Spacing, Corners.
  // Computes Transforms.
  void SetCorners(double *ftl, double *ftr, double *fbr, double *ltl);
  void SetCorners(vtkVector3 *ftl, vtkVector3 *ftr, vtkVector3 *fbr, vtkVector3 *ltl);

  void GetCorners(vtkVector3 *ftl, vtkVector3 *ftr, vtkVector3 *fbr, vtkVector3 *ltl);

  // Description:
  // Convert a gradient calculated on IJK-oriented data to RAS-oriented space.
  // The gradient should be already scaled by voxel size, so that only a
  // permutation of axes is required by this procedure.
  void IjkToRasGradient(vtkVector3 *Ras, vtkVector3 *Ijk);

  void Copy(vtkRasToIjkTransform *t);

  void Print(const char *x);
  void Dump();

protected:
  vtkRasToIjkTransform();
  ~vtkRasToIjkTransform();

private:
  vtkRasToIjkTransform(const vtkRasToIjkTransform&);  // Not implemented.
  void operator=(const vtkRasToIjkTransform);  // Not implemented.

  // Numbers
  int SliceOrder;

  // Arrays
  int Extent[6];
  float Spacing[3];

  // Corners
  vtkVector3 *CnrFtl;
  vtkVector3 *CnrFtr;
  vtkVector3 *CnrFbr;
  vtkVector3 *CnrLtl;

  // Matrices
  vtkMatrix4x4 *RasToIjk;
  vtkMatrix4x4 *IjkToRas;
  vtkMatrix4x4 *SlicerMatrix;

  void ComputeTransforms();
  void ComputeSliceOrder();
};

#endif
