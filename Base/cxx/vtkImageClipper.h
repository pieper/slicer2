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



