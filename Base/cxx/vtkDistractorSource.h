/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDistractorSource.h,v $
  Language:  C++
  Date:      $Date: 2000/07/25 17:56:56 $
  Version:   $Revision: 1.1 $


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
// .NAME vtkDistractorSource - generate a distractor
// .SECTION Description
// vtkDistractorSource creates a polygonal distractor

#ifndef __vtkDistractorSource_h
#define __vtkDistractorSource_h

#include "vtkPolyDataSource.h"

class VTK_EXPORT vtkDistractorSource : public vtkPolyDataSource 
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


