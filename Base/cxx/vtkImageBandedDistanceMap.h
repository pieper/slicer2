/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
// .NAME vtkImageBandedDistanceMap -  Does a quick partial distance map
// .SECTION Description
// Will compute distances from an input region, up to the user-specified
// MaximumDistanceToCompute.  The output is a distance map in 
// a band around the input region of interest, and pixels
// are set to the max distance outside of this band.  The point of this
// filter is to quickly compute a good enough distance map for applications
// that don't care about "really distant" pixels.
//

#ifndef __vtkImageBandedDistanceMap_h
#define __vtkImageBandedDistanceMap_h

#include "vtkImageNeighborhoodFilter.h"

class VTK_EXPORT vtkImageBandedDistanceMap : public vtkImageNeighborhoodFilter
{
public:
  static vtkImageBandedDistanceMap *New();
  vtkTypeMacro(vtkImageBandedDistanceMap,vtkImageSpatialFilter);
	
  // Description: 
  // Background and foreground pixel values in the image.
  // Usually 0 and some label value, respectively.
  vtkSetMacro(Background, float);
  vtkGetMacro(Background, float);
  vtkSetMacro(Foreground, float);
  vtkGetMacro(Foreground, float);

  // Description: 
  // Defines size of neighborhood around contour where 
  // distances will be computed.
  vtkGetMacro(MaximumDistanceToCompute, int);
  void SetMaximumDistanceToCompute(int distance);

  // Description: 
  // Set to 3 for a 3-D kernel.  Else will use a 2D kernel
  // to save time.
  vtkSetMacro(Dimensionality, int);
  vtkGetMacro(Dimensionality, int);

protected:
  vtkImageBandedDistanceMap();
  ~vtkImageBandedDistanceMap();

  float Background;
  float Foreground;

  int MaximumDistanceToCompute;
  int Dimensionality;

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
    int extent[6], int id);
};

#endif

