/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkBSplineInterpolateImageFunction.h,v $
  Language:  C++
  Date:      $Date: 2004/07/29 20:28:57 $
  Version:   $Revision: 1.2 $
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
  virtual float EvaluateFunction (float x[ImageDimension]);
  virtual void     EvaluateGradient (float x[ImageDimension], float g[ImageDimension]);
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
  float * Origin;
  float * Spacing;
  int * Extent;
private:
  //BTX
  int initialized;
  vtkBSplineInterpolateImageFunction(const vtkBSplineInterpolateImageFunction&);  // Not implemented.
  void operator=(const vtkBSplineInterpolateImageFunction&);  // Not implemented.

  /** Determines the weights for interpolation of the value x */
  void SetInterpolationWeights( float *x, const long EvaluateIndex[][ImageDimension], double weights[][ImageDimension],unsigned int splineOrder ) const;

  /** Determines the weights for the derivative portion of the value x */
  void SetDerivativeWeights( float *x, const long EvaluateIndex[][ImageDimension], double weights[][ImageDimension], unsigned int splineOrder ) const;

  /** Precomputation for converting the 1D index of the interpolation neighborhood 
    * to an N-dimensional index. */
  void GeneratePointsToIndex();

  /** Determines the indicies to use give the splines region of support */
  void DetermineRegionOfSupport( long evaluateIndex[][ImageDimension], float x[], unsigned int splineOrder ) const;

  /** Set the indicies in evaluateIndex at the boundaries based on mirror 
    * boundary conditions. */
  void ApplyMirrorBoundaryConditions(long evaluateIndex[][ImageDimension], unsigned int splineOrder) const;


  unsigned int             m_MaxNumberInterpolationPoints; // number of neighborhood points used for interpolation
  std::vector<int>    m_PointsToIndex[ImageDimension];  // Preallocation of interpolation neighborhood indicies
  //ETX
};


#endif
