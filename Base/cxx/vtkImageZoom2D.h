/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
// .NAME vtkImageZoom2D -  zooms (magnifies) a 2D image
// .SECTION Description
// vtkImageZoom2D 
//

#ifndef __vtkImageZoom2D_h
#define __vtkImageZoom2D_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageZoom2D : public vtkImageToImageFilter
{
public:
	static vtkImageZoom2D *New();
  vtkTypeMacro(vtkImageZoom2D,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the Magnification
	vtkSetMacro(Magnification, float);
	vtkGetMacro(Magnification, float);

  // Description:
  // If AutoCenter is turned on,
  // Zoom in on the Center of the input image
	vtkGetMacro(AutoCenter, int);
	vtkSetMacro(AutoCenter, int);
	vtkBooleanMacro(AutoCenter, int);

  // Description:
  // Set the Zoom Point to be some point
  // Once you do that, OrigPoint should be the original point in the image.
  // It looks to me like this function does the calculation wrong.
	void SetZoomPoint(int x, int y);
	vtkGetVector2Macro(ZoomPoint, int);
	vtkGetVector2Macro(OrigPoint, int);

  // Description:
  // Set Center of the region on which we zoom in.
	vtkSetVector2Macro(Center, float);
	vtkGetVector2Macro(Center, float);

  // Description:
  // Set to be 1/magnification in each direction.
  // NEVER USE THIS.
	vtkSetVector2Macro(Step, float);
	vtkGetVector2Macro(Step, float);

  // Description:
  // Set/Get Upper Left hand corner of zoom window.
  // NEVER USE THIS.
	vtkSetVector2Macro(Origin, float);
	vtkGetVector2Macro(Origin, float);

protected:
	vtkImageZoom2D();
	~vtkImageZoom2D(){};
	vtkImageZoom2D(const vtkImageZoom2D&) {};
	void operator=(const vtkImageZoom2D&) {};

  // Length of 1 Pixel in Zoom Window in the Original Image
  float Step[2];
  // Upper Left hand corner of Zoom Window
	float Origin[2];

	float Magnification;
  int AutoCenter;
  float Center[2];
	int OrigPoint[2];
	int ZoomPoint[2];

  void ExecuteInformation(vtkImageData *inData, 
    vtkImageData *outData);
	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



