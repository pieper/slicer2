/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageReformat -  Flexible threshold
// .SECTION Description
// vtkImageReformat Can do binary or continous thresholding

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

  vtkGetMacro(RunTime, int);
  vtkSetMacro(RunTime, int);
	
protected:
	vtkImageReformat();
	~vtkImageReformat();
	vtkImageReformat(const vtkImageReformat&) {};
	void operator=(const vtkImageReformat&) {};

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



