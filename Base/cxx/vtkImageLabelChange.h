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
// .NAME vtkImageLabelChange -  Flexible threshold
// .SECTION Description
//  The output data type may be different than the output, but defaults
// to the same type.

#ifndef __vtkImageLabelChange_h
#define __vtkImageLabelChange_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageLabelChange : public vtkImageToImageFilter
{
public:
	static vtkImageLabelChange *New();
  vtkTypeMacro(vtkImageLabelChange,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetMacro(InputLabel, float);
	vtkGetMacro(InputLabel, float);

	vtkSetMacro(OutputLabel, float);
	vtkGetMacro(OutputLabel, float);

protected:
	vtkImageLabelChange();
	~vtkImageLabelChange() {};
	vtkImageLabelChange(const vtkImageLabelChange&) {};
	void operator=(const vtkImageLabelChange&) {};

	float InputLabel;
	float OutputLabel;

	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif

