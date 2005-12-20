/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkBSplineInterpolateImageFunction.cxx,v $
  Date:      $Date: 2005/12/20 22:56:23 $
  Version:   $Revision: 1.5.8.1 $

=========================================================================auto=*/
#include "vtkBSplineInterpolateImageFunction.h"

vtkCxxRevisionMacro(vtkBSplineInterpolateImageFunction, "$Revision: 1.5.8.1 $");
vtkStandardNewMacro(vtkBSplineInterpolateImageFunction);

void vtkBSplineInterpolateImageFunction::SetInterpolationWeights( vtkFloatingPointType *x, long * EvaluateIndex[ImageDimension], double * weights[ImageDimension], unsigned int splineOrder ) const
{
  // For speed improvements we could make each case a separate function and use
  // function pointers to reference the correct weight order.
  // Left as is for now for readability.
  double w, w2, w4, t, t0, t1;
  unsigned int n;
  
  switch (splineOrder)
    {
    case 3:
      for (n = 0; n < ImageDimension; n++)
        {
        w = x[n] - (double) EvaluateIndex[n][1];
        weights[n][3] = (1.0 / 6.0) * w * w * w;
        weights[n][0] = (1.0 / 6.0) + 0.5 * w * (w - 1.0) - weights[n][3];
        weights[n][2] = w + weights[n][0] - 2.0 * weights[n][3];
        weights[n][1] = 1.0 - weights[n][0] - weights[n][2] - weights[n][3];
        }
      break;
    case 0:
      for (n = 0; n < ImageDimension; n++)
        {
        weights[n][0] = 1; // implements nearest neighbor
        }
      break;
    case 1:
      for (n = 0; n < ImageDimension; n++)
        {
        w = x[n] - (double) EvaluateIndex[n][0];
        weights[n][1] = w;
        weights[n][0] = 1.0 - w;
        }
      break;
    case 2:
      for (n = 0; n < ImageDimension; n++)
        {
        /* x */
        w = x[n] - (double)EvaluateIndex[n][1];
        weights[n][1] = 0.75 - w * w;
        weights[n][2] = 0.5 * (w - weights[n][1] + 1.0);
        weights[n][0] = 1.0 - weights[n][1] - weights[n][2];
        }
      break;
    case 4:
      for (n = 0; n < ImageDimension; n++)
        {
        /* x */
        w = x[n] - (double)EvaluateIndex[n][2];
        w2 = w * w;
        t = (1.0 / 6.0) * w2;
        weights[n][0] = 0.5 - w;
        weights[n][0] *= weights[n][0];
        weights[n][0] *= (1.0 / 24.0) * weights[n][0];
        t0 = w * (t - 11.0 / 24.0);
        t1 = 19.0 / 96.0 + w2 * (0.25 - t);
        weights[n][1] = t1 + t0;
        weights[n][3] = t1 - t0;
        weights[n][4] = weights[n][0] + t0 + 0.5 * w;
        weights[n][2] = 1.0 - weights[n][0] - weights[n][1] - weights[n][3] - weights[n][4];
        }
      break;
    case 5:
      for (n = 0; n < ImageDimension; n++)
        {
        /* x */
        w = x[n] - (double)EvaluateIndex[n][2];
        w2 = w * w;
        weights[n][5] = (1.0 / 120.0) * w * w2 * w2;
        w2 -= w;
        w4 = w2 * w2;
        w -= 0.5;
        t = w2 * (w2 - 3.0);
        weights[n][0] = (1.0 / 24.0) * (1.0 / 5.0 + w2 + w4) - weights[n][5];
        t0 = (1.0 / 24.0) * (w2 * (w2 - 5.0) + 46.0 / 5.0);
        t1 = (-1.0 / 12.0) * w * (t + 4.0);
        weights[n][2] = t0 + t1;
        weights[n][3] = t0 - t1;
        t0 = (1.0 / 16.0) * (9.0 / 5.0 - t);
        t1 = (1.0 / 24.0) * w * (w4 - w2 - 5.0);
        weights[n][1] = t0 + t1;
        weights[n][4] = t0 - t1;
        }
      break;
    default:
      // SplineOrder not implemented yet.
      //vtkErrorMacro(<<"BSplineInterpolateImageFunction : SplineOrder must be between 0 and 5. Requested spline order has not been implemented yet." );
      break;
    }
    
}

void vtkBSplineInterpolateImageFunction::SetDerivativeWeights( vtkFloatingPointType *x, long *EvaluateIndex[ImageDimension], double *weights[ImageDimension], unsigned int splineOrder ) const
{
  // For speed improvements we could make each case a separate function and use
  // function pointers to reference the correct weight order.
  // Another possiblity would be to loop inside the case statement (reducing the number
  // of switch statement executions to one per routine call.
  // Left as is for now for readability.
  double w, w1, w2, w3, w4, w5, t, t0, t1, t2;
  int derivativeSplineOrder = (int) splineOrder -1;
  unsigned int n;

  switch (derivativeSplineOrder)
    {
    
      // Calculates B(splineOrder) ( (x + 1/2) - xi) - B(splineOrder -1) ( (x - 1/2) - xi)
    case -1:
      // Why would we want to do this?
      for (n = 0; n < ImageDimension; n++)
        {
      weights[n][0] = 0.0;
        }
      break;
    case 0:
      for (n = 0; n < ImageDimension; n++)
        {
      weights[n][0] = -1.0;
      weights[n][1] =  1.0;
        }
      break;
    case 1:
      for (n = 0; n < ImageDimension; n++)
        {
      w = x[n] + 0.5 - (double)EvaluateIndex[n][1];
      // w2 = w;
      w1 = 1.0 - w;

      weights[n][0] = 0.0 - w1;
      weights[n][1] = w1 - w;
      weights[n][2] = w; 
        }
      break;
    case 2: 
      for (n = 0; n < ImageDimension; n++)
        {
      w = x[n] + .5 - (double)EvaluateIndex[n][2];
      w2 = 0.75 - w * w;
      w3 = 0.5 * (w - w2 + 1.0);
      w1 = 1.0 - w2 - w3;

      weights[n][0] = 0.0 - w1;
      weights[n][1] = w1 - w2;
      weights[n][2] = w2 - w3;
      weights[n][3] = w3; 
        }
      break;
    case 3:
      for (n = 0; n < ImageDimension; n++)
        {
      w = x[n] + 0.5 - (double)EvaluateIndex[n][2];
      w4 = (1.0 / 6.0) * w * w * w;
      w1 = (1.0 / 6.0) + 0.5 * w * (w - 1.0) - w4;
      w3 = w + w1 - 2.0 * w4;
      w2 = 1.0 - w1 - w3 - w4;

      weights[n][0] = 0.0 - w1;
      weights[n][1] = w1 - w2;
      weights[n][2] = w2 - w3;
      weights[n][3] = w3 - w4;
      weights[n][4] = w4;
        }
      break;
    case 4:
      for (n = 0; n < ImageDimension; n++)
        {
      w = x[n] + .5 - (double)EvaluateIndex[n][3];
      t2 = w * w;
      t = (1.0 / 6.0) * t2;
      w1 = 0.5 - w;
      w1 *= w1;
      w1 *= (1.0 / 24.0) * w1;
      t0 = w * (t - 11.0 / 24.0);
      t1 = 19.0 / 96.0 + t2 * (0.25 - t);
      w2 = t1 + t0;
      w4 = t1 - t0;
      w5 = w1 + t0 + 0.5 * w;
      w3 = 1.0 - w1 - w2 - w4 - w5;

      weights[n][0] = 0.0 - w1;
      weights[n][1] = w1 - w2;
      weights[n][2] = w2 - w3;
      weights[n][3] = w3 - w4;
      weights[n][4] = w4 - w5;
      weights[n][5] = w5;
        }
      break;
      
    default:
      // SplineOrder not implemented yet.
      //vtkErrorMacro(<<"BSplineInterpolateImageFunction : SplineOrder (for derivatives) must be between 1 and 5. Requested spline order has not been implemented yet." );
      break;
    }
    
}

// Generates m_PointsToIndex;
void vtkBSplineInterpolateImageFunction::GeneratePointsToIndex()
{
  // m_PointsToIndex is used to convert a sequential location to an N-dimension
  // index vector.  This is precomputed to save time during the interpolation routine.
  for ( int i = 0 ; i < ImageDimension ; i++ )
    m_PointsToIndex[i].resize(m_MaxNumberInterpolationPoints);

  unsigned long indexFactor[ImageDimension];
  for (unsigned int p = 0; p < m_MaxNumberInterpolationPoints; p++)
    {
      int pp = p;
      indexFactor[0] = 1;
      int j;
      for (j=1; j< ImageDimension; j++) {
    indexFactor[j] = indexFactor[j-1] * ( m_SplineOrder + 1 );
      }
      for (j = (ImageDimension - 1); j >= 0; j--) {
    m_PointsToIndex[j][p] = pp / indexFactor[j];
    pp = pp % indexFactor[j];
      }
    }
}

void vtkBSplineInterpolateImageFunction::DetermineRegionOfSupport( long *evaluateIndex[ImageDimension], vtkFloatingPointType x[], unsigned int splineOrder ) const
{ 
  long indx;

  // compute the interpolation indexes 
  for (unsigned int n = 0; n< ImageDimension; n++)
    {
      if (splineOrder & 1)     // Use this index calculation for odd splineOrder
    {
      indx = (long)floor(x[n]) - splineOrder / 2;
      for (unsigned int k = 0; k <= splineOrder; k++)
        {
          evaluateIndex[n][k] = indx++;
        }
    }
      else                       // Use this index calculation for even splineOrder
    { 
      indx = (long)floor(x[n] + 0.5) - splineOrder / 2;
      for (unsigned int k = 0; k <= splineOrder; k++)
        {
          evaluateIndex[n][k] = indx++;
        }
    }
    }
}

void vtkBSplineInterpolateImageFunction::ApplyMirrorBoundaryConditions(long *evaluateIndex[ImageDimension], unsigned int splineOrder) const
{
  for (unsigned int n = 0; n < ImageDimension; n++)
    {
      long dataLength2 = 2 * m_DataLength[n] - 2;

      // apply the mirror boundary conditions 
      // TODO:  We could implement other boundary options beside mirror
      if (m_DataLength[n] == 1)
    {
      for (unsigned int k = 0; k <= splineOrder; k++)
        {
          evaluateIndex[n][k] = 0;
        }
    }
      else
    {
      for (unsigned int k = 0; k <= splineOrder; k++)
        {
          // btw - Think about this couldn't this be replaced with a more elagent modulus method?
          evaluateIndex[n][k] = (evaluateIndex[n][k] < 0L) ? (-evaluateIndex[n][k] - dataLength2 * ((-evaluateIndex[n][k]) / dataLength2))
        : (evaluateIndex[n][k] - dataLength2 * (evaluateIndex[n][k] / dataLength2));
          if ((long) m_DataLength[n] <= evaluateIndex[n][k])
        {
          evaluateIndex[n][k] = dataLength2 - evaluateIndex[n][k];
        }
        }
    }  
    }
}

void vtkBSplineInterpolateImageFunction::PrintSelf(ostream& os, vtkIndent indent) {
  vtkImplicitFunction::PrintSelf( os, indent );
  os << indent << "Spline Order: " << m_SplineOrder << std::endl;
}

vtkFloatingPointType vtkBSplineInterpolateImageFunction::EvaluateFunction( vtkFloatingPointType *x )
{
  //  long EvaluateIndex[m_SplineOrder + 1][ImageDimension];
  //  double weights[m_SplineOrder + 1][ImageDimension];
  long * EvaluateIndex[ImageDimension];
  double * weights[ImageDimension];
  vtkFloatingPointType index[ImageDimension];
  int i;
  for ( i = 0 ; i < ImageDimension ; i++ ) {
    EvaluateIndex[i] = new long[m_SplineOrder + 1];
    weights[i] = new double[m_SplineOrder + 1];
    index[i] = (x[i] - Origin[i])/Spacing[i];
    if ( index[i] < ((vtkFloatingPointType)Extent[2*i]) || index[i] > ((vtkFloatingPointType)Extent[2*i+1]) ) {
      return 0.0;
    }
  }
  // compute the interpolation indexes 
  this->DetermineRegionOfSupport(EvaluateIndex, index, m_SplineOrder); 
  
  // Determine weights
  SetInterpolationWeights( index, EvaluateIndex, weights, m_SplineOrder );

  // Modify EvaluateIndex at the boundaries using mirror boundary conditions
  this->ApplyMirrorBoundaryConditions(EvaluateIndex, m_SplineOrder);

  // perform interpolation 
  double interpolated = 0.0;
  int coefficientIndex[ImageDimension];
  // Step through eachpoint in the N-dimensional interpolation cube.
  for (unsigned int p = 0; p < m_MaxNumberInterpolationPoints; p++)
    {
    // translate each step into the N-dimensional index.
    //      IndexType pointIndex = PointToIndex( p );

    double w = 1.0;
    for (unsigned int n = 0; n < ImageDimension; n++ )
      {
      w *= weights[n][ m_PointsToIndex[n][p] ];
      coefficientIndex[n] = EvaluateIndex[n][m_PointsToIndex[n][p]];  // Build up ND index for coefficients.
      }
      // Convert our step p to the appropriate point in ND space in the
      // m_Coefficients cube.
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 3)
      interpolated += w * m_Coefficients->GetScalarComponentAsDouble(coefficientIndex[0],coefficientIndex[1],coefficientIndex[2],0);
#else
      interpolated += w * m_Coefficients->GetScalarComponentAsFloat(coefficientIndex[0],coefficientIndex[1],coefficientIndex[2],0);
#endif
    }
  for ( i = 0 ; i < ImageDimension ; i++ ) {
    delete [] EvaluateIndex[i];
    delete [] weights[i];
  }
  return(interpolated);
    
}

void vtkBSplineInterpolateImageFunction::EvaluateGradient(vtkFloatingPointType *x, vtkFloatingPointType *derivativeValue )
{
  //  long EvaluateIndex[m_SplineOrder + 1][ImageDimension];
  //  double weights[m_SplineOrder + 1][ImageDimension];
  //  double weightsDerivative [m_SplineOrder + 1][ImageDimension];
  vtkFloatingPointType index[3];
  long * EvaluateIndex[ImageDimension];
  double * weights[ImageDimension];
  double * weightsDerivative[ImageDimension];
  int i;
  for ( i = 0 ; i < ImageDimension ; i++ ) {
    EvaluateIndex[i] = new long[m_SplineOrder + 1];
    weights[i] = new double[m_SplineOrder + 1];
    weightsDerivative[i]= new double[m_SplineOrder + 1];
    index[i] = (x[i] - Origin[i])/Spacing[i];
    if ( index[i] < Extent[2*i] || index[i] > Extent[2*i+1] )
      return;
  }
  // compute the interpolation indexes 
  // TODO: Do we need to revisit region of support for the derivatives?
  this->DetermineRegionOfSupport(EvaluateIndex, index, m_SplineOrder);

  // Determine weights
  SetInterpolationWeights( index, EvaluateIndex, weights, m_SplineOrder );

  SetDerivativeWeights( index, EvaluateIndex, weightsDerivative, ( m_SplineOrder ) );

  // Modify EvaluateIndex at the boundaries using mirror boundary conditions
  this->ApplyMirrorBoundaryConditions(EvaluateIndex, m_SplineOrder);
  
  // Calculate derivative
  double tempValue;
  int coefficientIndex[ImageDimension];
  for (unsigned int n = 0; n < ImageDimension; n++)
    {
    derivativeValue[n] = 0.0;
    for (unsigned int p = 0; p < m_MaxNumberInterpolationPoints; p++)
      {
      tempValue = 1.0 ; 
      for (unsigned int n1 = 0; n1 < ImageDimension; n1++)
        {
        //coefficientIndex[n1] = EvaluateIndex[n1][sp];
        coefficientIndex[n1] = EvaluateIndex[n1][m_PointsToIndex[n1][p]];

        if (n1 == n)
          {
          //w *= weights[n][ m_PointsToIndex[p][n] ];
          tempValue *= weightsDerivative[n1][ m_PointsToIndex[n1][p] ];
          }
        else
          {
          tempValue *= weights[n1][ m_PointsToIndex[n1][p] ];
          }
        }
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 3)
      derivativeValue[n] += m_Coefficients->GetScalarComponentAsDouble(coefficientIndex[0],coefficientIndex[1],coefficientIndex[2],0) * tempValue ;
#else
      derivativeValue[n] += m_Coefficients->GetScalarComponentAsFloat(coefficientIndex[0],coefficientIndex[1],coefficientIndex[2],0) * tempValue ;
#endif
      }
    }
  for ( i = 0 ; i < ImageDimension ; i++ ) {
    delete [] EvaluateIndex[i];
    delete [] weights[i];
    delete [] weightsDerivative[i];
  }
}

void vtkBSplineInterpolateImageFunction::SetInput(vtkImageData* dataset) {
  initialized = 1;
  m_Coefficients = dataset;
  dataset->GetDimensions(m_DataLength);
  Origin = dataset->GetOrigin();
  Spacing = dataset->GetSpacing();
  Extent = dataset->GetExtent();
}

void vtkBSplineInterpolateImageFunction::SetSplineOrder(unsigned int SplineOrder)
{
  if (initialized && SplineOrder == m_SplineOrder)
    {
    return;
    }
  m_SplineOrder = SplineOrder;

  m_MaxNumberInterpolationPoints = 1;
  for (unsigned int n=0; n < ImageDimension; n++)
    {
      m_MaxNumberInterpolationPoints *= ( m_SplineOrder + 1);
    } 
  this->GeneratePointsToIndex( );
}
