/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageClipInteractive -  Flexible threshold
// .SECTION Description
// vtkImageClipInteractive Can do binary or continous thresholding

#ifndef __vtkImageClipInteractive_h
#define __vtkImageClipInteractive_h

#include "vtkImageToImageFilter.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkImageClipInteractive : public vtkImageToImageFilter
{
public:
	static vtkImageClipInteractive *New();
  vtkTypeMacro(vtkImageClipInteractive,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkGetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
	vtkSetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);

	vtkGetObjectMacro(ReformatMatrix, vtkMatrix4x4);
	vtkSetObjectMacro(ReformatMatrix, vtkMatrix4x4);

	vtkGetMacro(FieldOfView, float);
	vtkSetMacro(FieldOfView, float);	
	float FieldOfView;

	vtkSetVectorMacro(ClipExtent, int, 6);
	vtkGetVectorMacro(ClipExtent, int, 6);

protected:
	vtkImageClipInteractive();
	~vtkImageClipInteractive();
	vtkImageClipInteractive(const vtkImageClipInteractive&) {};
	void operator=(const vtkImageClipInteractive&) {};

  vtkMatrix4x4* ReformatMatrix;
	vtkMatrix4x4* WldToIjkMatrix;

  int ClipExtent[6];

	// Override this function since inExt != outExt
	void ComputeInputUpdateExtent(int inExt[6],int outExt[6]);
  
	void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);

//	void Execute(vtkImageData *inData, vtkImageData *outData);
	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



