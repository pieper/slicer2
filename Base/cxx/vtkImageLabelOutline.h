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
// .NAME vtkImageLabelOutline -  Flexible threshold
// .SECTION Description
//  The output data type may be different than the output, but defaults
// to the same type.

#ifndef __vtkImageLabelOutline_h
#define __vtkImageLabelOutline_h

#include "vtkImageSpatialFilter.h"

class VTK_EXPORT vtkImageLabelOutline : public vtkImageSpatialFilter
{
public:
	static vtkImageLabelOutline *New();
	const char *GetClassName() {return "vtkImageLabelOutline";};
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetMacro(Background, float);
	vtkGetMacro(Background, float);

	vtkSetMacro(Outline, int);
	vtkGetMacro(Outline, int);

	unsigned char *GetMaskPointer() {return mask;}
	void SetNeighborTo8();
	void SetNeighborTo4();
	vtkGetMacro(Neighbor, float);

protected:
	vtkImageLabelOutline();
	~vtkImageLabelOutline();
	void SetKernelSize(int size0, int size1, int size2);
	float Background;
	int Neighbor;
	int Outline;
	unsigned char *mask;

	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



