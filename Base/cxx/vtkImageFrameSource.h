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
// .NAME vtkImageFrameSource - Pulls data from a RenderWindow and outputs it.
// .SECTION Description
// Used to make a matching window in the Twin slicer module.
//
#ifndef __vtkImageFrameSource_h
#define __vtkImageFrameSource_h

#include "vtkImageSource.h"
#include "vtkRenderWindow.h"

class VTK_EXPORT vtkImageFrameSource : public vtkImageSource 
{
public:
    static vtkImageFrameSource *New();
  vtkTypeMacro(vtkImageFrameSource,vtkImageSource);
     void PrintSelf(ostream& os, vtkIndent indent);

    void SetExtent(int xMin, int xMax, int yMin, int yMax);

    void ExecuteInformation();

    vtkSetObjectMacro(RenderWindow, vtkRenderWindow);
    vtkGetObjectMacro(RenderWindow, vtkRenderWindow);

protected:
    vtkImageFrameSource();
    ~vtkImageFrameSource();
  vtkImageFrameSource(const vtkImageFrameSource&) {};
  void operator=(const vtkImageFrameSource&) {};

    int WholeExtent[6];
    vtkRenderWindow *RenderWindow;

    void Execute(vtkImageData *data);
};


#endif

  
