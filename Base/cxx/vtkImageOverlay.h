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
// .NAME vtkImageOverlay - Overlay Images
// .SECTION Description
// vtkImageOverlay takes multiple inputs images and merges
// them into one output. All inputs must have the same extent,
// scalar type, and number of components.

#ifndef __vtkImageOverlay_h
#define __vtkImageOverlay_h

#include "vtkImageMultipleInputFilter.h"

class VTK_EXPORT vtkImageOverlay : public vtkImageMultipleInputFilter
{
public:
  static vtkImageOverlay *New();
  vtkTypeMacro(vtkImageOverlay,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Sets the opacity used to overlay this layer on the others
  double GetOpacity(int layer);
  void SetOpacity(int layer, double opacity);

  // Sets whether to fade out the background even when the 
  // foreground is clearn
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
