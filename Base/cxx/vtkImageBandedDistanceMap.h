/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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

