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
// .NAME vtkDistractorSource - generate a distractor
// .SECTION Description
// vtkDistractorSource creates a polygonal distractor

#ifndef __vtkDistractorSource_h
#define __vtkDistractorSource_h

#include "vtkPolyDataSource.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkDistractorSource : public vtkPolyDataSource 
{
public:
  vtkDistractorSource(int res=6);
  static vtkDistractorSource *New() {return new vtkDistractorSource;};
  const char *GetClassName() {return "vtkDistractorSource";};
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the angle of rotation
  vtkSetClampMacro(Angle,float,0.0,VTK_LARGE_FLOAT)
  vtkGetMacro(Angle,float);

  // Description:
  // Set the distance of translation
  vtkSetClampMacro(Distance,float,0.0,VTK_LARGE_FLOAT)
  vtkGetMacro(Distance,float);

  // Description:
  // Set the width
  vtkSetClampMacro(Width,float,0.0,VTK_LARGE_FLOAT)
  vtkGetMacro(Width,float);

  // Description:
  // Set the footprint size
  vtkSetClampMacro(FootWidth,float,0.0,VTK_LARGE_FLOAT)
  vtkGetMacro(FootWidth,float);

  // Description:
  // Set/Get rotation center
  vtkSetVector3Macro(Center,float);
  vtkGetVectorMacro(Center,float,3);

  // Description:
  // Set/Get distractor rotation axis
  vtkSetVector3Macro(Axis,float);
  vtkGetVectorMacro(Axis,float,3);

  // Description:
  // Set/Get distractor start point
  vtkSetVector3Macro(Start,float);
  vtkGetVectorMacro(Start,float,3);

  // Description:
  // Set the number of facets used to define distractor.
  vtkSetClampMacro(Resolution,int,0,VTK_CELL_SIZE)
  vtkGetMacro(Resolution,int);

  // Description:
  // Set/Get foot normal
  vtkSetVector3Macro(FootNormal,float);
  vtkGetVectorMacro(FootNormal,float,3);

protected:
  void Execute();
  float Angle;
  float Distance;
  float Width;
  float FootWidth;
  float Center[3];
  float Axis[3];
  float Start[3];
  float FootNormal[3];
  int Resolution;

};

#endif


