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
// .NAME vtkImageResize - resize (scale) the input image
// .SECTION Description
// Currently just used in vtkMrmlVolume for the histogram, which
// died with the advent of vtk3.1.  The histogram needs to be 
// fixed.
// Kilian: Just workes if the input image is of larger extent the the output image !
//         otherwise use vtkImageResample !!!

#ifndef __vtkImageResize_h
#define __vtkImageResize_h

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageResize : public vtkImageToImageFilter
{
public:
  static vtkImageResize *New();
  vtkTypeMacro(vtkImageResize,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // The whole extent of the output has to be set explicitely.
  void SetOutputWholeExtent(int extent[6]);
  void SetOutputWholeExtent(int minX, int maxX, int minY, int maxY, 
                int minZ, int maxZ);
  void GetOutputWholeExtent(int extent[6]);
  int *GetOutputWholeExtent() {return this->OutputWholeExtent;}

  // Description:
  // The whole extent of the input has to be set explicitely.
  void SetInputClipExtent(int extent[6]);
  void SetInputClipExtent(int minX, int maxX, int minY, int maxY, 
                int minZ, int maxZ);
  void GetInputClipExtent(int extent[6]);
  int *GetInputClipExtent() {return this->InputClipExtent;}

protected:
  vtkImageResize();
  ~vtkImageResize() {};
  vtkImageResize(const vtkImageResize&) {};
  void operator=(const vtkImageResize&) {};

  int OutputWholeExtent[6];
  int InputClipExtent[6];

  int Initialized;
  
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
    int outExt[6], int id);
};

#endif
