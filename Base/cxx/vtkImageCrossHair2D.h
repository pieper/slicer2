/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
// .NAME vtkImageCrossHair2D -- draws CrossHair2Ds with tic marks on 2D window. 
// .SECTION Description
// vtkImageCrossHair2D draws cross hairs with optional hash marks on a 2D window.
// 

#ifndef __vtkImageCrossHair2D_h
#define __vtkImageCrossHair2D_h

#include "vtkImageInPlaceFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageCrossHair2D : public vtkImageInPlaceFilter
{
public:
    static vtkImageCrossHair2D *New();
  vtkTypeMacro(vtkImageCrossHair2D,vtkImageInPlaceFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the RGB CursorColor
    vtkSetVector3Macro(CursorColor, float);
    vtkGetVectorMacro(CursorColor, float, 3);

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
    vtkGetMacro(HashGap, float);
    vtkSetMacro(HashGap, float);

  // Description:
  // Get/Set the Length of a hash mark in mm.
    vtkGetMacro(HashLength, float);
    vtkSetMacro(HashLength, float);

  // Description:
  // Get/Set the Magnification
  // NOTE: This should not be used.  Instead, specify the magnification
  // implicitly in the spacing.
    vtkGetMacro(Magnification, float);
    vtkSetMacro(Magnification, float);

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
    float CursorColor[3];
    float Magnification;
    float HashGap;
    float HashLength;
    int BullsEye;
    int BullsEyeWidth;

    void DrawCursor(vtkImageData *outData, int outExt[6]);

    // Not threaded because its too simple of a filter
    void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif

