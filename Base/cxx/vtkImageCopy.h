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
//.NAME vtkImageCopy copies an image
//.SECTION Description 
// vtkImageCopy takes an image as input and produces a copy of the image
// as output. By setting the clear variable, the output data can be all zeros.
// instead of a copy.

#ifndef __vtkImageCopy_h
#define __vtkImageCopy_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageCopy : public vtkImageToImageFilter
{
public:
  static vtkImageCopy *New();
  vtkTypeMacro(vtkImageCopy,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // If Clear is set to 1, the output image is all zeros.
  // If Clear is set to 0, the output image is a copy of the input.
  vtkSetMacro(Clear, int);
  vtkGetMacro(Clear, int);
  vtkBooleanMacro(Clear, int);

protected:
  vtkImageCopy();
  ~vtkImageCopy() {};
  vtkImageCopy(const vtkImageCopy&) {};
  void operator=(const vtkImageCopy&) {};

  int Clear;
  
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		       int extent[6], int id);
};

#endif

