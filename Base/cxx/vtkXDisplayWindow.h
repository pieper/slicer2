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
// .NAME vtkXDisplayWindow - Used to display a vtk window on an X display.
// .SECTION Description
// Can display on any screen (display) number.
//

#ifndef __vtkXDisplayWindow_h
#define __vtkXDisplayWindow_h

#include <stdio.h>
#include "vtkRenderWindow.h"
#include "vtkImageWindow.h"
#include "vtkObject.h"

#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkXDisplayWindow : public vtkObject
{
public:
  static vtkXDisplayWindow *New();
  vtkTypeMacro(vtkXDisplayWindow, vtkObject);

  vtkRenderWindow* GetRenderWindow(int screen);
  vtkImageWindow* GetImageWindow(int screen);

protected:
  vtkXDisplayWindow();
  ~vtkXDisplayWindow();
  vtkRenderWindow *RenderWindow;
  vtkImageWindow *ImageWindow;
};

#endif


