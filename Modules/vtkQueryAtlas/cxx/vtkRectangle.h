/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkRectangle.h,v $
  Date:      $Date: 2005/12/20 22:56:15 $
  Version:   $Revision: 1.1.8.1 $

=========================================================================auto=*/
/*===========================================================

3D Slicer Software Licence Agreement(c)

Copyright 2003-2004 Massachusetts Institute of Technology 
(MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and 
Women's Hospital, Inc. on behalf of the copyright holders 
and contributors. Permission is hereby granted, without 
payment, to copy, modify, display and distribute this 
software and its documentation, if any, for research 
purposes only, provided that (1) the above copyright notice
and the following four paragraphs appear on all copies of 
this software, and (2) that source code to any 
modifications to this software be made publicly available 
under terms no more restrictive than those in this License 
Agreement. Use of this software constitutes acceptance of 
these terms and conditions.

3D Slicer Software has not been reviewed or approved by 
the Food and Drug Administration, and is for non-clinical, 
IRB-approved Research Use Only. In no event shall data or 
images generated through the use of 3D Slicer Software be 
used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE 
USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE 
COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF 
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY 
DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS." THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, 
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Authors: Michael McKenna, David Small, Steve Pieper.
    Small Design Firm, Inc.,
    in association with Isomics, Inc.,
    and Partners Health Care.  MA, USA.
    June, 2004

===========================================================*/


// .NAME vtkRectangle - create a basic rectangle polygon in a plane
// .SECTION Description


#ifndef __vtkRectangle_h
#define __vtkRectangle_h

#include "vtkPolyDataSource.h"
//#include "vtkFloatArray.h"

#include <vtkQueryAtlasConfigure.h>

class vtkTextureText;


class VTK_QUERYATLAS_EXPORT vtkRectangle : public vtkPolyDataSource 
{
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeRevisionMacro(vtkRectangle,vtkPolyDataSource);

    // Description:
    // Construct a rectangular polygon perpendicular to z-axis, used to make a 
    // surrounding box around a vtkTextureType object.
    static vtkRectangle *New();

    vtkGetMacro(Width, vtkFloatingPointType);
    vtkSetMacro(Width, vtkFloatingPointType);
    vtkGetMacro(Height, vtkFloatingPointType);
    vtkSetMacro(Height, vtkFloatingPointType);

    vtkGetMacro(Left, vtkFloatingPointType);
    vtkSetMacro(Left, vtkFloatingPointType);
    vtkGetMacro(Bottom, vtkFloatingPointType);
    vtkSetMacro(Bottom, vtkFloatingPointType);

    vtkGetMacro(ZOffset, vtkFloatingPointType);
    vtkSetMacro(ZOffset, vtkFloatingPointType);

    vtkGetMacro(TextureTurn, int);
    vtkSetMacro(TextureTurn, int);

//    void SetScalePoints(vtkFloatingPointType x, vtkFloatingPointType y);
    void SetPositionOffset(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
    void GetPositionOffset(vtkFloatingPointType pos[3]);

    void AddPositionOffset(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
    void AddPositionOffset(vtkFloatingPointType pos[3]);

protected:
  vtkRectangle();
  ~vtkRectangle() {};

  void Execute();

  vtkFloatingPointType Normal[3];
  vtkFloatingPointType Width;
  vtkFloatingPointType Height;
  vtkFloatingPointType Left;
  vtkFloatingPointType Bottom;
  vtkFloatingPointType ZOffset;

  // The TextureTurn count is the # of 90 degree clockwise turns to make for the texture coords
  int TextureTurn;

private:
  vtkRectangle(const vtkRectangle&);  // Not implemented.
  void operator=(const vtkRectangle&);  // Not implemented.
};

#endif


