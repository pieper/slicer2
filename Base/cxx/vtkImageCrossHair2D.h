/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageCrossHair2D -- draws CrossHair2Ds with tic marks on 2D window. 
// .SECTION Description
// vtkImageCrossHair2D draws cross hairs with optional hash marks on a 2D window.
// 

#ifndef __vtkImageCrossHair2D_h
#define __vtkImageCrossHair2D_h

#include "vtkImageInPlaceFilter.h"

class VTK_EXPORT vtkImageCrossHair2D : public vtkImageInPlaceFilter
{
public:
	static vtkImageCrossHair2D *New();
	const char *GetClassName() {return "vtkImageCrossHair2D";};
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

