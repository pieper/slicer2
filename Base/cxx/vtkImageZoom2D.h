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
// .NAME vtkImageZoom2D -  Flexible threshold
// .SECTION Description
// vtkImageZoom2D Can do binary or continous thresholding

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



