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
// .NAME vtkImageClipper -  Flexible threshold
// .SECTION Description
// vtkImageClipper Can do binary or continous thresholding
// for lower, upper or a range of data.
//  The output data type may be different than the output, but defaults
// to the same type.

#ifndef __vtkImageClipper_h
#define __vtkImageClipper_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageClipper : public vtkImageToImageFilter
{
public:
	static vtkImageClipper *New();
  vtkTypeMacro(vtkImageClipper,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  void SetOutputWholeExtent(int extent[6]);
  void SetOutputWholeExtent(int minX, int maxX, int minY, int maxY, 
			    int minZ, int maxZ);
  vtkGetVectorMacro(OutputWholeExtent, int, 6);

        // Description:
	// Determines whether to replace the pixel in range with InValue
	vtkSetMacro(ReplaceIn, int);
	vtkGetMacro(ReplaceIn, int);
	vtkBooleanMacro(ReplaceIn, int);

	// Description:
	// Replace the in range pixels with this value.
	vtkSetMacro(InValue, float);
	vtkGetMacro(InValue, float);
  
	// Description:
	// Determines whether to replace the pixel out of range with OutValue
	vtkSetMacro(ReplaceOut, int);
	vtkGetMacro(ReplaceOut, int);
	vtkBooleanMacro(ReplaceOut, int);

	// Description:
	// Replace the in range pixels with this value.
	vtkSetMacro(OutValue, float);
	vtkGetMacro(OutValue, float);
  
protected:
	vtkImageClipper();
	~vtkImageClipper(){};
	vtkImageClipper(const vtkImageClipper&) {};
	void operator=(const vtkImageClipper&) {};

	int ReplaceIn;
	float InValue;
	int ReplaceOut;
	float OutValue;
	int OutputWholeExtent[6];

	void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



