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
// .NAME vtkROISelectTracts - 
// .SECTION Description
//
// Select tractographic paths based on intersection with ROI(s)
//

#ifndef __vtkROISelectTracts_h
#define __vtkROISelectTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkTransform.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkShortArray.h"
#include "vtkCollection.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"

class VTK_DTMRI_EXPORT vtkROISelectTracts : public vtkObject
{
 public:
  static vtkROISelectTracts *New();
  vtkTypeMacro(vtkROISelectTracts,vtkObject);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform the ijk coordinates of the ROI to world coordinates.
  vtkSetObjectMacro(ROIToWorld, vtkTransform);
  vtkGetObjectMacro(ROIToWorld, vtkTransform);

  // Description
  // Transformation used in seeding streamlines.  This is for the 
  // second ROI volume.
  vtkSetObjectMacro(ROI2ToWorld, vtkTransform);
  vtkGetObjectMacro(ROI2ToWorld, vtkTransform);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform world coordinates into scaled ijk of the tensor field.
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

  // Description
  // Convolution Kernel that is used to convolve the fiber with when
  // finding the ROIs that the fiber passes through
  vtkSetObjectMacro(ConvolutionKernel, vtkDoubleArray);
  vtkGetObjectMacro(ConvolutionKernel, vtkDoubleArray);

  // Description
  // Get Streamlines as Polylines
  vtkGetObjectMacro(StreamlinesAsPolyLines,vtkPolyData);
  
  // Description
  // List of the input vtkHyperStreamlines (or subclasses)
  vtkSetObjectMacro(Streamlines, vtkCollection);
  vtkGetObjectMacro(Streamlines, vtkCollection);

  // Description
  // Input to this class (for setting colors/transparency). This 
  // may change to a colorID array.
  vtkSetObjectMacro(Actors, vtkCollection);

  // Description
  // Streamlines will be started at locations with this value in the InputROI.
  // The value must be greater than 0. A 0 value is not allowed because it
  // would allow users to accidentally start streamlines outside of their
  // ROI.
  vtkSetClampMacro(InputROIValue, int, 1, VTK_SHORT_MAX);
  vtkGetMacro(InputROIValue, int);

  // Description
  // Streamlines will be started at locations with these values in the InputROI.
  // The value must be greater than 0. A 0 value is not allowed because it
  // would allow users to accidentally start streamlines outside of their
  // ROI.
  vtkSetObjectMacro(InputMultipleROIValues,vtkShortArray);
  vtkGetObjectMacro(InputMultipleROIValues,vtkShortArray);

  // Description
  // Input ROI volume for testing tract/ROI intersection
  vtkSetObjectMacro(InputROI, vtkImageData);
  vtkGetObjectMacro(InputROI, vtkImageData);

    //Description
  // Find the streamlines that pass through the set of ROI values
  // stored in InputMultipleROIValues. This operation is performed
  // by convolving  the streamline with the kernel ConvolutionKernel.
  void FindStreamlinesThatPassThroughROI();
  
  // Description
  // Convert Streamline from Points representation to PolyLines
  void ConvertStreamlinesToPolyLines();
  
  void HighlightStreamlinesPassTest();
  
  void DeleteStreamlinesNotPassTest();

 protected:
  vtkROISelectTracts();
  ~vtkROISelectTracts();

  vtkCollection *Streamlines;
  vtkCollection *Actors;

  vtkImageData *InputROI;
  vtkImageData *InputROI2;

  int InputROIValue;
  int InputROI2Value;
  vtkShortArray *InputMultipleROIValues;
  
  vtkTransform *ROIToWorld;
  vtkTransform *ROI2ToWorld;
  vtkTransform *WorldToTensorScaledIJK;

  vtkDoubleArray *ConvolutionKernel;
  
  vtkPolyData *StreamlinesAsPolyLines;
  vtkIntArray *StreamlineIdPassTest;
  

};

#endif
