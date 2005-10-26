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
#include "vtkDoubleArray.h"
#include "vtkCollection.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkMultipleStreamlineController.h"

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
  // Convolution Kernel that is used to convolve the fiber with when
  // finding the ROIs that the fiber passes through
  vtkSetObjectMacro(ConvolutionKernel, vtkDoubleArray);
  vtkGetObjectMacro(ConvolutionKernel, vtkDoubleArray);

  // Description
  // Get Streamlines as Polylines
  vtkGetObjectMacro(StreamlinesAsPolyLines,vtkPolyData);
   
  // Description
  // Class that controls streamlines creation and display
  void SetStreamlineController(vtkMultipleStreamlineController *controller) {
     StreamlineController = controller;
     Streamlines = controller->GetStreamlines();
     Actors = controller->GetDisplayTracts()->GetActors();
  };   
     
  // Description
  // Streamlines will be started at locations with this value in the InputROI.
  // The value must be greater than 0. A 0 value is not allowed because it
  // would allow users to accidentally start streamlines outside of their
  // ROI.
  vtkSetClampMacro(InputROIValue, int, 1, VTK_SHORT_MAX);
  vtkGetMacro(InputROIValue, int);

  // Description
  // ROI labels that fibers will pass through (AND operation).
  vtkSetObjectMacro(InputANDROIValues,vtkShortArray);
  vtkGetObjectMacro(InputANDROIValues,vtkShortArray);
  
  // Description
  // ROI labels that fibers will not pass through (NOT operation).
  vtkSetObjectMacro(InputNOTROIValues,vtkShortArray);
  vtkGetObjectMacro(InputNOTROIValues,vtkShortArray);
  
  
  // Description
  // Threshold to pass the test and define that a streamline pass a given
  // set of ROI's define in the ADN list and NOT list.
  vtkSetMacro(PassThreshold,int);
  vtkGetMacro(PassThreshold,int);
  
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
  
  void ResetStreamlinesPassTest();

 protected:
  vtkROISelectTracts();
  ~vtkROISelectTracts();

  vtkMultipleStreamlineController *StreamlineController;
  vtkCollection *Streamlines;
  vtkCollection *Actors;

  vtkImageData *InputROI;
  vtkImageData *InputROI2;

  int InputROIValue;
  int InputROI2Value;
 
  vtkTransform *ROIToWorld;
 
  vtkShortArray *InputANDROIValues;
  vtkShortArray *InputNOTROIValues;
  int PassThreshold;
  
  vtkDoubleArray *ConvolutionKernel;
  
  vtkPolyData *StreamlinesAsPolyLines;
  vtkIntArray *StreamlineIdPassTest;
  
  vtkDoubleArray *ColorActors;
  
};

#endif
