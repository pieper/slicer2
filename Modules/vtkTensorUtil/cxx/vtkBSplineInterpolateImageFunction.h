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
  Module:    $RCSfile: vtkBSplineInterpolateImageFunction.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 22:07:02 $
  Version:   $Revision: 1.5 $
*/
// .NAME vtkBSplineInterpolateImageFunction - BSpline interpolation of a image dataset of points
// .SECTION Description
// vtkBSplineInterpolateImageFunction


#ifndef __vtkBSplineInterpolateImageFunction_h
#define __vtkBSplineInterpolateImageFunction_h

#define VTK_INTEGRATE_MAJOR_EIGENVECTOR  0
#define VTK_INTEGRATE_MEDIUM_EIGENVECTOR 1
#define VTK_INTEGRATE_MINOR_EIGENVECTOR  2

#include <vector>  // for the buffer
#include "vtkImplicitFunction.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"  // for storing output of vtk/itk filter

#include "vtkTensorUtilConfigure.h"

#define ImageDimension 3

class VTK_TENSORUTIL_EXPORT vtkBSplineInterpolateImageFunction : public vtkImplicitFunction
{
 public:
  static vtkBSplineInterpolateImageFunction *New();
  vtkTypeRevisionMacro(vtkBSplineInterpolateImageFunction, vtkImplicitFunction );
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual vtkFloatingPointType EvaluateFunction (vtkFloatingPointType x[ImageDimension]);
  virtual void     EvaluateGradient (vtkFloatingPointType x[ImageDimension], vtkFloatingPointType g[ImageDimension]);
  void SetInput(vtkImageData* dataset);
  int GetSplineOrder(void) { return m_SplineOrder; }
  void SetSplineOrder(unsigned int order);
  
protected:

  vtkBSplineInterpolateImageFunction() {
    initialized = 0;
    SetSplineOrder(3);
  }
  ~vtkBSplineInterpolateImageFunction() {}
  int                                 m_DataLength[ImageDimension];  // Image size
  unsigned int                        m_SplineOrder;    // User specified spline order (3rd or cubic is the default)

  vtkImageData *                      m_Coefficients; // Spline coefficients  
  vtkFloatingPointType * Origin;
  vtkFloatingPointType * Spacing;
  int * Extent;
private:
  //BTX
  int initialized;
  vtkBSplineInterpolateImageFunction(const vtkBSplineInterpolateImageFunction&);  // Not implemented.
  void operator=(const vtkBSplineInterpolateImageFunction&);  // Not implemented.

  /** Determines the weights for interpolation of the value x */
  void SetInterpolationWeights( vtkFloatingPointType *x, long *EvaluateIndex[ImageDimension], double *weights[ImageDimension],unsigned int splineOrder ) const;

  /** Determines the weights for the derivative portion of the value x */
  void SetDerivativeWeights( vtkFloatingPointType *x, long *EvaluateIndex[ImageDimension], double *weights[ImageDimension], unsigned int splineOrder ) const;

  /** Precomputation for converting the 1D index of the interpolation neighborhood 
    * to an N-dimensional index. */
  void GeneratePointsToIndex();

  /** Determines the indicies to use give the splines region of support */
  void DetermineRegionOfSupport( long *evaluateIndex[ImageDimension], vtkFloatingPointType x[], unsigned int splineOrder ) const;

  /** Set the indicies in evaluateIndex at the boundaries based on mirror 
    * boundary conditions. */
  void ApplyMirrorBoundaryConditions(long *evaluateIndex[ImageDimension], unsigned int splineOrder) const;


  unsigned int             m_MaxNumberInterpolationPoints; // number of neighborhood points used for interpolation
  std::vector<int>    m_PointsToIndex[ImageDimension];  // Preallocation of interpolation neighborhood indicies
  //ETX
};


#endif
