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
// .NAME vtkImageWeightedSum -  adds any number of images, weighting
// each according to the weight given in this->SetWeightForInput(i,w).
//
// .SECTION Description
// All weights are normalized so they will sum to 1.
// Images must have the same extents.
//  
//
#ifndef __vtkImageWeightedSum_h
#define __vtkImageWeightedSum_h

#include "vtkImageMultipleInputFilter.h"
#include "vtkFloatArray.h"

class VTK_EXPORT vtkImageWeightedSum : public vtkImageMultipleInputFilter
{
  public:
  static vtkImageWeightedSum *New();
  vtkTypeMacro(vtkImageWeightedSum,vtkImageMultipleInputFilter);
  
  // Description:
  // weights control contributiotn of each input to the sum
  // they will be normalized
  // set these for each input else they will be automatically set to 1
  float GetWeightForInput(int i);
  float GetNormalizedWeightForInput(int i);
  void SetWeightForInput(int i, float w);

  // Description:
  // called by execute; makes sure a weight exists for each input image
  void CheckWeights();

  protected:
  vtkImageWeightedSum();
  ~vtkImageWeightedSum();
  
  vtkFloatArray * Weights;

  void NormalizeWeights();

  void ExecuteInformation(vtkImageData **inputs, vtkImageData *output); 

  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData, 
		       int extent[6], int id);
};

#endif

