/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageNeighborhoodFilter -  Augments vtkImageSpatialFilter with
// a mask to allow neighborhoods of any shape.
// .SECTION Description
// 4 or 8 neighbor connectivity, or subclasses may define their own neighborhood
// shapes.

#ifndef __vtkImageNeighborhoodFilter_h
#define __vtkImageNeighborhoodFilter_h

#include "vtkImageSpatialFilter.h"

class VTK_EXPORT vtkImageNeighborhoodFilter : public vtkImageSpatialFilter
{
public:
  static vtkImageNeighborhoodFilter *New();
  vtkTypeMacro(vtkImageNeighborhoodFilter,vtkImageSpatialFilter);
	
  // Description:
  // Mask that defines area of interest in the neighborhood.
  // Value is 1 for voxels of interest, 0 otherwise.
  // This should be used when looping through neighborhood.
  unsigned char *GetMaskPointer() {return Mask;}

  // Description:
  // Use 4 or 8 neighbor connectivity in neighborhood around pixel
  // (This is 2-D connectivity though the kernel is 3D... from erode.)
  void SetNeighborTo8();
  void SetNeighborTo4();

  // Description: 
  // Get the neighborhood size
  // leftover from erode.
  vtkGetMacro(Neighbor, int);

  // Description: 
  // Get the neighborhood extent, where the max/min numbers are
  // offsets from the current voxel.  (So these numbers may be negative.)
  // For use when looping through the neighborhood.
  void GetRelativeHoodExtent(int &hoodMin0, int &hoodMax0, int &hoodMin1,
			     int &hoodMax1, int &hoodMin2, int &hoodMax2);

  // Description: 
  // Get the increments to use when looping through the mask.
  void GetMaskIncrements(int &maskInc0, int &maskInc1, int &maskInc2);

  // Description:
  // Lauren test
  void SetNeighborhoodToLine(int length);

protected:
  vtkImageNeighborhoodFilter();
  ~vtkImageNeighborhoodFilter();

  // Description: 
  // set size of neighborhood of each pixel
  void SetKernelSize(int size0, int size1, int size2);

  int Neighbor;
  unsigned char *Mask;
};

#endif

