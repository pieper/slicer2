#include "vtkBSplineInterpolateImageFunction.h"

vtkCxxRevisionMacro(vtkBSplineInterpolateImageFunction, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkBSplineInterpolateImageFunction);

void vtkBSplineInterpolateImageFunction::SetInterpolationWeights( float *x, const long EvaluateIndex[][ImageDimension], double weights[][ImageDimension], unsigned int splineOrder ) const
{
  // For speed improvements we could make each case a separate function and use
  // function pointers to reference the correct weight order.
  // Left as is for now for readability.
  double w, w2, w4, t, t0, t1;
  
  switch (splineOrder)
    {
    case 3:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
        w = x[n] - (double) EvaluateIndex[1][n];
        weights[3][n] = (1.0 / 6.0) * w * w * w;
        weights[0][n] = (1.0 / 6.0) + 0.5 * w * (w - 1.0) - weights[3][n];
        weights[2][n] = w + weights[0][n] - 2.0 * weights[3][n];
        weights[1][n] = 1.0 - weights[0][n] - weights[2][n] - weights[3][n];
        }
      break;
    case 0:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
        weights[0][n] = 1; // implements nearest neighbor
        }
      break;
    case 1:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
        w = x[n] - (double) EvaluateIndex[0][n];
        weights[1][n] = w;
        weights[0][n] = 1.0 - w;
        }
      break;
    case 2:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
        /* x */
        w = x[n] - (double)EvaluateIndex[1][n];
        weights[1][n] = 0.75 - w * w;
        weights[2][n] = 0.5 * (w - weights[1][n] + 1.0);
        weights[0][n] = 1.0 - weights[1][n] - weights[2][n];
        }
      break;
    case 4:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
        /* x */
        w = x[n] - (double)EvaluateIndex[2][n];
        w2 = w * w;
        t = (1.0 / 6.0) * w2;
        weights[0][n] = 0.5 - w;
        weights[0][n] *= weights[0][n];
        weights[0][n] *= (1.0 / 24.0) * weights[0][n];
        t0 = w * (t - 11.0 / 24.0);
        t1 = 19.0 / 96.0 + w2 * (0.25 - t);
        weights[1][n] = t1 + t0;
        weights[3][n] = t1 - t0;
        weights[4][n] = weights[0][n] + t0 + 0.5 * w;
        weights[2][n] = 1.0 - weights[0][n] - weights[1][n] - weights[3][n] - weights[4][n];
        }
      break;
    case 5:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
        /* x */
        w = x[n] - (double)EvaluateIndex[2][n];
        w2 = w * w;
        weights[5][n] = (1.0 / 120.0) * w * w2 * w2;
        w2 -= w;
        w4 = w2 * w2;
        w -= 0.5;
        t = w2 * (w2 - 3.0);
        weights[0][n] = (1.0 / 24.0) * (1.0 / 5.0 + w2 + w4) - weights[5][n];
        t0 = (1.0 / 24.0) * (w2 * (w2 - 5.0) + 46.0 / 5.0);
        t1 = (-1.0 / 12.0) * w * (t + 4.0);
        weights[2][n] = t0 + t1;
        weights[3][n] = t0 - t1;
        t0 = (1.0 / 16.0) * (9.0 / 5.0 - t);
        t1 = (1.0 / 24.0) * w * (w4 - w2 - 5.0);
        weights[1][n] = t0 + t1;
        weights[4][n] = t0 - t1;
        }
      break;
    default:
      // SplineOrder not implemented yet.
      //vtkErrorMacro(<<"BSplineInterpolateImageFunction : SplineOrder must be between 0 and 5. Requested spline order has not been implemented yet." );
      break;
    }
    
}

void vtkBSplineInterpolateImageFunction::SetDerivativeWeights( float *x, const long EvaluateIndex[][ImageDimension], double weights[][ImageDimension], unsigned int splineOrder ) const
{
  // For speed improvements we could make each case a separate function and use
  // function pointers to reference the correct weight order.
  // Another possiblity would be to loop inside the case statement (reducing the number
  // of switch statement executions to one per routine call.
  // Left as is for now for readability.
  double w, w1, w2, w3, w4, w5, t, t0, t1, t2;
  int derivativeSplineOrder = (int) splineOrder -1;
  
  switch (derivativeSplineOrder)
    {
    
      // Calculates B(splineOrder) ( (x + 1/2) - xi) - B(splineOrder -1) ( (x - 1/2) - xi)
    case -1:
      // Why would we want to do this?
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
      weights[0][n] = 0.0;
        }
      break;
    case 0:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
      weights[0][n] = -1.0;
      weights[1][n] =  1.0;
        }
      break;
    case 1:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
      w = x[n] + 0.5 - (double)EvaluateIndex[1][n];
      // w2 = w;
      w1 = 1.0 - w;

      weights[0][n] = 0.0 - w1;
      weights[1][n] = w1 - w;
      weights[2][n] = w; 
        }
      break;
    case 2:
      
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
      w = x[n] + .5 - (double)EvaluateIndex[2][n];
      w2 = 0.75 - w * w;
      w3 = 0.5 * (w - w2 + 1.0);
      w1 = 1.0 - w2 - w3;

      weights[0][n] = 0.0 - w1;
      weights[1][n] = w1 - w2;
      weights[2][n] = w2 - w3;
      weights[3][n] = w3; 
        }
      break;
    case 3:
      
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
      w = x[n] + 0.5 - (double)EvaluateIndex[2][n];
      w4 = (1.0 / 6.0) * w * w * w;
      w1 = (1.0 / 6.0) + 0.5 * w * (w - 1.0) - w4;
      w3 = w + w1 - 2.0 * w4;
      w2 = 1.0 - w1 - w3 - w4;

      weights[0][n] = 0.0 - w1;
      weights[1][n] = w1 - w2;
      weights[2][n] = w2 - w3;
      weights[3][n] = w3 - w4;
      weights[4][n] = w4;
        }
      break;
    case 4:
      for (unsigned int n = 0; n < ImageDimension; n++)
        {
      w = x[n] + .5 - (double)EvaluateIndex[3][n];
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

      weights[0][n] = 0.0 - w1;
      weights[1][n] = w1 - w2;
      weights[2][n] = w2 - w3;
      weights[3][n] = w3 - w4;
      weights[4][n] = w4 - w5;
      weights[5][n] = w5;
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
      for (int j=1; j< ImageDimension; j++) {
    indexFactor[j] = indexFactor[j-1] * ( m_SplineOrder + 1 );
      }
      for (int j = (ImageDimension - 1); j >= 0; j--) {
    m_PointsToIndex[j][p] = pp / indexFactor[j];
    pp = pp % indexFactor[j];
      }
    }
}

void vtkBSplineInterpolateImageFunction::DetermineRegionOfSupport( long evaluateIndex[][ImageDimension], float x[], unsigned int splineOrder ) const
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
          evaluateIndex[k][n] = indx++;
        }
    }
      else                       // Use this index calculation for even splineOrder
    { 
      indx = (long)floor(x[n] + 0.5) - splineOrder / 2;
      for (unsigned int k = 0; k <= splineOrder; k++)
        {
          evaluateIndex[k][n] = indx++;
        }
    }
    }
}

void vtkBSplineInterpolateImageFunction::ApplyMirrorBoundaryConditions(long evaluateIndex[][ImageDimension], unsigned int splineOrder) const
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
          evaluateIndex[k][n] = 0;
        }
    }
      else
    {
      for (unsigned int k = 0; k <= splineOrder; k++)
        {
          // btw - Think about this couldn't this be replaced with a more elagent modulus method?
          evaluateIndex[k][n] = (evaluateIndex[k][n] < 0L) ? (-evaluateIndex[k][n] - dataLength2 * ((-evaluateIndex[k][n]) / dataLength2))
        : (evaluateIndex[k][n] - dataLength2 * (evaluateIndex[k][n] / dataLength2));
          if ((long) m_DataLength[n] <= evaluateIndex[k][n])
        {
          evaluateIndex[k][n] = dataLength2 - evaluateIndex[k][n];
        }
        }
    }  
    }
}

void vtkBSplineInterpolateImageFunction::PrintSelf(ostream& os, vtkIndent indent) {
  vtkImplicitFunction::PrintSelf( os, indent );
  os << indent << "Spline Order: " << m_SplineOrder << std::endl;
}

float vtkBSplineInterpolateImageFunction::EvaluateFunction( float *x )
{
  long EvaluateIndex[m_SplineOrder + 1][ImageDimension];
  float index[ImageDimension];
  for ( int i = 0 ; i < ImageDimension ; i++ ) {
    index[i] = (x[i] - Origin[i])/Spacing[i];
    if ( index[i] < ((float)Extent[2*i]) || index[i] > ((float)Extent[2*i+1]) ) {
      return 0.0;
    }
  }
  // compute the interpolation indexes 
  this->DetermineRegionOfSupport(EvaluateIndex, index, m_SplineOrder); 
  
  // Determine weights
  double weights[m_SplineOrder + 1][ImageDimension];
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
      w *= weights[ m_PointsToIndex[n][p] ][n];
      coefficientIndex[n] = EvaluateIndex[m_PointsToIndex[n][p]][n];  // Build up ND index for coefficients.
      }
      // Convert our step p to the appropriate point in ND space in the
      // m_Coefficients cube.
      interpolated += w * m_Coefficients->GetScalarComponentAsFloat(coefficientIndex[0],coefficientIndex[1],coefficientIndex[2],0);
    }
    
  return(interpolated);
    
}

void vtkBSplineInterpolateImageFunction::EvaluateGradient(float *x, float *derivativeValue )
{
  long EvaluateIndex[m_SplineOrder + 1][ImageDimension];
  float index[3];
  for ( int i = 0 ; i < ImageDimension ; i++ ) {
    index[i] = (x[i] - Origin[i])/Spacing[i];
    if ( index[i] < Extent[2*i] || index[i] > Extent[2*i+1] )
      return;
  }
  // compute the interpolation indexes 
  // TODO: Do we need to revisit region of support for the derivatives?
  this->DetermineRegionOfSupport(EvaluateIndex, index, m_SplineOrder);

  // Determine weights
  double weights[m_SplineOrder + 1][ImageDimension];
  SetInterpolationWeights( index, EvaluateIndex, weights, m_SplineOrder );

  double weightsDerivative [m_SplineOrder + 1][ImageDimension];
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
        coefficientIndex[n1] = EvaluateIndex[m_PointsToIndex[n1][p]][n1];

        if (n1 == n)
          {
          //w *= weights[n][ m_PointsToIndex[p][n] ];
          tempValue *= weightsDerivative[ m_PointsToIndex[n1][p] ][n1];
          }
        else
          {
          tempValue *= weights[ m_PointsToIndex[n1][p] ][n1];
          }
        }
      derivativeValue[n] += m_Coefficients->GetScalarComponentAsFloat(coefficientIndex[0],coefficientIndex[1],coefficientIndex[2],0) * tempValue ;
      }
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
