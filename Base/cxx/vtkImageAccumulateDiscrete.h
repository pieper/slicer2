/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
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
// .NAME vtkImageAccumulateDiscrete - Generalized histograms upto 4 dimensions.
// .SECTION Description
// vtkImageAccumulateDiscrete - This filter divides component space into
// discrete bins.  It then counts the number of pixels associated
// with each bin.  The output is this "scatter plot".
// The input can be any type, but the output is always int.


#ifndef __vtkImageAccumulateDiscrete_h
#define __vtkImageAccumulateDiscrete_h


#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageAccumulateDiscrete : public vtkImageToImageFilter
{
public:
  static vtkImageAccumulateDiscrete *New();
  const char *GetClassName() {return "vtkImageAccumulateDiscrete";};
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageAccumulateDiscrete();
  ~vtkImageAccumulateDiscrete() {};
  vtkImageAccumulateDiscrete(const vtkImageAccumulateDiscrete&) {};
  void operator=(const vtkImageAccumulateDiscrete&) {};

  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
  void ComputeRequiredInputUpdateExtent(int inExt[6], int outExt[6]);
  void Execute(vtkImageData *inData, vtkImageData *outData);

  // Description:
  // Generate more than requested.  Called by the superclass before
  // an execute, and before output memory is allocated.
  void ModifyOutputUpdateExtent();

};

#endif



