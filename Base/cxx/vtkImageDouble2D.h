/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
// .NAME vtkImageDouble2D -  Doubles size of input image
// .SECTION Description
// Used in the slicer to convert 256 by 256 size reformatted images
// into 512 by 512 size images.  Uses pixel duplication, not interpolation.

#ifndef __vtkImageDouble2D_h
#define __vtkImageDouble2D_h

#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageDouble2D : public vtkImageToImageFilter
{
public:    
  static vtkImageDouble2D *New();
  vtkTypeMacro(vtkImageDouble2D,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageDouble2D();
  ~vtkImageDouble2D(){};
  vtkImageDouble2D(const vtkImageDouble2D&) {};
  void operator=(const vtkImageDouble2D&) {};

  // Override this function since inExt != outExt
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  
  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
                       int extent[6], int id);
};

#endif



