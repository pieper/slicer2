/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
// .NAME vtkImageNeighborhoodFilter -  Augments vtkImageSpatialFilter with
// a mask to allow neighborhoods of any shape.
// .SECTION Description
// 4 or 8 neighbor connectivity, or subclasses may define their own neighborhood
// shapes. The mask is basically a binary (0/1) filter kernel to define
// neighbors of interest.
//
#ifndef __vtkImageNeighborhoodFilter_h
#define __vtkImageNeighborhoodFilter_h

#include "vtkImageSpatialFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageNeighborhoodFilter : public vtkImageSpatialFilter
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
  // This is in 3D
  void SetNeighborTo8();
  void SetNeighborTo4();

  // Description: 
  // Get the neighborhood size
  // (for erode class)
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

