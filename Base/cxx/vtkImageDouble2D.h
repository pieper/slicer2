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
// .NAME vtkImageDouble2D -  Flexible threshold
// .SECTION Description
// vtkImageDouble2D Can do binary or continous thresholding

#ifndef __vtkImageDouble2D_h
#define __vtkImageDouble2D_h

#include "vtkImageFilter.h"

class VTK_EXPORT vtkImageDouble2D : public vtkImageFilter
{
public:	
	static vtkImageDouble2D *New();
	const char *GetClassName() {return "vtkImageDouble2D";};
	void PrintSelf(ostream& os, vtkIndent indent);

protected:
	vtkImageDouble2D();
	~vtkImageDouble2D(){};
	vtkImageDouble2D(const vtkImageDouble2D&) {};
	void operator=(const vtkImageDouble2D&) {};

	// Override this function since inExt != outExt
	void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  
	void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);

	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



