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
// .NAME vtkImageReformatIJK -  Flexible threshold
// .SECTION Description
// vtkImageReformatIJK Can do binary or continous thresholding

#ifndef __vtkImageReformatIJK_h
#define __vtkImageReformatIJK_h

#include "vtkImageToImageFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkIntArray.h"

class VTK_EXPORT vtkImageReformatIJK : public vtkImageToImageFilter
{
public:
	static vtkImageReformatIJK *New();
  vtkTypeMacro(vtkImageReformatIJK,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // 
	vtkGetVector4Macro(XStep, float);
	vtkGetVector4Macro(YStep, float);
	vtkGetVector4Macro(ZStep, float);
	vtkGetVector4Macro(Origin, float);

  // Description:
  // 
	vtkSetMacro(Slice, int);
	vtkGetMacro(Slice, int);

  // Description:
  // 
  vtkSetMacro(InputOrder, int);
  vtkGetMacro(InputOrder, int);
  vtkSetMacro(OutputOrder, int);
  vtkGetMacro(OutputOrder, int);
  
  // Description:
  // 
  void SetInputOrderString(char *str);
  void SetOutputOrderString(char *str);

	vtkGetObjectMacro(Indices, vtkIntArray);

	vtkGetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
	vtkSetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);

  void ComputeReformatMatrix(vtkMatrix4x4 *ref);

  void SetIJKPoint(int i, int j, int k);
  vtkGetVectorMacro(XYPoint, int, 2);

	vtkMatrix4x4* WldToIjkMatrix;
  int NumSlices;
  vtkGetMacro(NumSlices, int);
  float XStep[4];
  float YStep[4];
  float ZStep[4];
  float Origin[4];
  int IJKPoint[3];
  int XYPoint[2];
  int Slice;
  vtkTransform *tran;
  int InputOrder;
  int OutputOrder;
  vtkIntArray *Indices;
  void ComputeTransform();
  void ComputeOutputExtent();

protected:
  vtkImageReformatIJK();
  ~vtkImageReformatIJK();
	vtkImageReformatIJK(const vtkImageReformatIJK&) {};
	void operator=(const vtkImageReformatIJK&) {};

  vtkTimeStamp TransformTime;
  int OutputExtent[6];

	// Override this function since inExt != outExt
	void ComputeInputUpdateExtent(int inExt[6],int outExt[6]);
	void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);

	void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif



