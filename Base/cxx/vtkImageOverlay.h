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
// .NAME vtkImageOverlay - Overlay Images
// .SECTION Description
// vtkImageOverlay takes multiple inputs images and merges
// them into one output. All inputs must have the same extent,
// scalar type, and number of components.

#ifndef __vtkImageOverlay_h
#define __vtkImageOverlay_h

#include "vtkImageData.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageOverlay : public vtkImageMultipleInputFilter
{
public:
  static vtkImageOverlay *New();
  vtkTypeMacro(vtkImageOverlay,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Sets the opacity used to overlay this layer on the others
  double GetOpacity(int layer);
  void SetOpacity(int layer, double opacity);

  // Sets whether to fade out the background even when the 
  // foreground is clear
  int GetFade(int layer);
  void SetFade(int layer, int fade);

protected:
  vtkImageOverlay();
  ~vtkImageOverlay();
  vtkImageOverlay(const vtkImageOverlay&) {};
  void operator=(const vtkImageOverlay&) {};
  double *Opacity;
  int nOpacity;
  int *Fade;
  int nFade;

  void UpdateForNumberOfInputs();
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
    int extent[6], int id);  
};

#endif
