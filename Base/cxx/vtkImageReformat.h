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
// .NAME vtkImageReformat -  Reformats a 2D image from a 3D volume.
// .SECTION Description
// vtkImageReformat allows interpolation or replication.
//


#ifndef __vtkImageReformat_h
#define __vtkImageReformat_h

#include "vtkImageToImageFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkIntArray.h"

class VTK_EXPORT vtkImageReformat : public vtkImageToImageFilter
{
public:
	static vtkImageReformat *New();
  vtkTypeMacro(vtkImageReformat,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkGetMacro(Interpolate, int);
	vtkSetMacro(Interpolate, int);
	vtkBooleanMacro(Interpolate, int);

	//Description: Wld stands for the world coordinates
	vtkGetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
	vtkSetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);

	vtkGetObjectMacro(ReformatMatrix, vtkMatrix4x4);
	vtkSetObjectMacro(ReformatMatrix, vtkMatrix4x4);

	//Description: reformatted image in pixels
	// more stuff
	vtkGetMacro(Resolution, int);
	vtkSetMacro(Resolution, int);
	
	//Description: plane in world space
	vtkGetMacro(FieldOfView, float);
	vtkSetMacro(FieldOfView, float);
	
	void SetPoint(int x, int y);
	vtkGetVector3Macro(WldPoint, float);
	vtkGetVector3Macro(IjkPoint, float);

	float YStep[3];
	float XStep[3];
	float Origin[3];
  
  // >> AT 11/07/01

  // Description
  // XY vector from center of image to center of panned image
  // comes from GUI (MainInteractorPan)
  vtkGetVector2Macro(OriginShift, float);
  vtkSetVector2Macro(OriginShift, float);

  // Description
  // Zoom factor coming from GUI (MainInteractorZoom)
  vtkGetMacro(Zoom, float);
  vtkSetMacro(Zoom, float);

  // Description
  // Pixel size of the reformatted image in mm
  vtkGetMacro(PanScale, float);
  // Description
  // For internal class use only
  vtkSetMacro(PanScale, float);

  // Description
  vtkGetObjectMacro(OriginShiftMtx, vtkMatrix4x4);
  //vtkSetObjectMacro(OriginShiftMtx, vtkMatrix4x4);
  // << AT 11/07/01

  vtkGetMacro(RunTime, int);
  vtkSetMacro(RunTime, int);
	
protected:
	vtkImageReformat();
	~vtkImageReformat();
	vtkImageReformat(const vtkImageReformat&) {};
	void operator=(const vtkImageReformat&) {};

  // >> AT 11/07/01
  float OriginShift[2];
  float Zoom;
  float PanScale;
  vtkMatrix4x4 *OriginShiftMtx;
  // << AT 11/07/01

	int RunTime;
	float IjkPoint[3];
	float WldPoint[3];
	int Resolution;
	float FieldOfView;
	int Interpolate;
	vtkMatrix4x4* ReformatMatrix;
	vtkMatrix4x4* WldToIjkMatrix;

	// Override this function since inExt != outExt
	void ComputeInputUpdateExtent(int inExt[6],int outExt[6]);
  
	void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



