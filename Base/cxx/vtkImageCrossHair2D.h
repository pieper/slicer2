/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkImageCrossHair2D -- draws CrossHair2Ds with tic marks on 2D window. 
// .SECTION Description
// vtkImageCrossHair2D draws cross hairs with optional hash marks on a 2D window.
// 

#ifndef __vtkImageCrossHair2D_h
#define __vtkImageCrossHair2D_h

#include "vtkImageData.h"
#include "vtkImageInPlaceFilter.h"
#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

class VTK_SLICER_BASE_EXPORT vtkImageCrossHair2D : public vtkImageInPlaceFilter
{
public:
    static vtkImageCrossHair2D *New();
  vtkTypeMacro(vtkImageCrossHair2D,vtkImageInPlaceFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the RGB CursorColor
    vtkSetVector3Macro(CursorColor, vtkFloatingPointType);
    vtkGetVectorMacro(CursorColor, vtkFloatingPointType, 3);

  // Description:
  // Get/Set the Number of Hash marks on the Cross Hair
    vtkGetMacro(NumHashes, int);
    vtkSetMacro(NumHashes, int);

  // Description:
  // Get/Set the BullsEyeWidth in pixels.
    vtkGetMacro(BullsEyeWidth, int);
    vtkSetMacro(BullsEyeWidth, int);

  // Description:
  // Turn the BullsEye on and off
    vtkGetMacro(BullsEye, int);
    vtkSetMacro(BullsEye, int);
    vtkBooleanMacro(BullsEye, int);

  // Description:
  // Get/Set the Spacing between Hash Marks in mm.
    vtkGetMacro(HashGap, vtkFloatingPointType);
    vtkSetMacro(HashGap, vtkFloatingPointType);

  // Description:
  // Get/Set the Length of a hash mark in mm.
    vtkGetMacro(HashLength, vtkFloatingPointType);
    vtkSetMacro(HashLength, vtkFloatingPointType);

  // Description:
  // Get/Set the Magnification
  // NOTE: This should not be used.  Instead, specify the magnification
  // implicitly in the spacing.
    vtkGetMacro(Magnification, vtkFloatingPointType);
    vtkSetMacro(Magnification, vtkFloatingPointType);

  // Description:
  // Set whether or not the cursor should be shown
  // If not, this filter does nothing.
    vtkGetMacro(ShowCursor, int);
    vtkSetMacro(ShowCursor, int);
    vtkBooleanMacro(ShowCursor, int);

  // Description 
  // Set the cross to intersect or not. 
  // If not, the result is perpendicular lines
  // with their intersection removed.
    vtkGetMacro(IntersectCross, int);
    vtkSetMacro(IntersectCross, int);
    vtkBooleanMacro(IntersectCross, int);

  // Description 
  // Get/Set The Cursor Position.
    vtkSetVector2Macro(Cursor, int);
    vtkGetVectorMacro(Cursor, int, 2);

protected:
    vtkImageCrossHair2D();
    ~vtkImageCrossHair2D() {};

  vtkImageCrossHair2D(const vtkImageCrossHair2D&) {};
  void operator=(const vtkImageCrossHair2D&) {};

    int ShowCursor;
    int NumHashes;
    int IntersectCross;
    int Cursor[2];
    vtkFloatingPointType CursorColor[3];
    vtkFloatingPointType Magnification;
    vtkFloatingPointType HashGap;
    vtkFloatingPointType HashLength;
    int BullsEye;
    int BullsEyeWidth;

    void DrawCursor(vtkImageData *outData, int outExt[6]);

    // Not threaded because its too simple of a filter
    void ExecuteData(vtkDataObject *);
};

#endif

