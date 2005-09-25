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
// .NAME vtkColorROIFromTracts - 
// .SECTION Description
//

#ifndef __vtkColorROIFromTracts_h
#define __vtkColorROIFromTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkCollection.h"

class VTK_DTMRI_EXPORT vtkColorROIFromTracts : public vtkObject
{
 public:
  static vtkColorROIFromTracts *New();
  vtkTypeMacro(vtkColorROIFromTracts,vtkObject);

  void ColorROIFromStreamlines();
  
  // Description
  // Input ROI volume to color with the ID of the streamlines through the ROI
  vtkSetObjectMacro(InputROIForColoring, vtkImageData);
  vtkGetObjectMacro(InputROIForColoring, vtkImageData);

  // Description
  // Output ROI volume, colored with the ID of the streamlines through the ROI
  vtkGetObjectMacro(OutputROIForColoring, vtkImageData);

  // Description
  // Input to this class (for grabbing colors). This 
  // may change to a colorID array.
  vtkSetObjectMacro(Actors, vtkCollection);

  // Description
  // Input to this class (tractographic paths)
  vtkSetObjectMacro(Streamlines, vtkCollection);

  // Description
  // Transformation used to place streamlines in scene 
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform the ijk coordinates of the ROI to world coordinates.
  vtkSetObjectMacro(ROIToWorld, vtkTransform);
  vtkGetObjectMacro(ROIToWorld, vtkTransform);

 protected:
  vtkColorROIFromTracts();
  ~vtkColorROIFromTracts();

  vtkImageData *InputROIForColoring;
  vtkImageData *OutputROIForColoring;

  vtkTransform *ROIToWorld;
  vtkTransform *WorldToTensorScaledIJK;

  vtkCollection *Streamlines;
  vtkCollection *Actors;

};

#endif
