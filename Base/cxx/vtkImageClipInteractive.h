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
// .NAME vtkImageClipInteractive -  Flexible threshold
// .SECTION Description
// vtkImageClipInteractive Can do binary or continous thresholding

#ifndef __vtkImageClipInteractive_h
#define __vtkImageClipInteractive_h

#include "vtkImageToImageFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageClipInteractive : public vtkImageToImageFilter
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

//    void Execute(vtkImageData *inData, vtkImageData *outData);
    void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
        int extent[6], int id);
};

#endif



