/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
// .NAME vtkImageWeightedSum -  adds any number of images, weighting
// each according to the weight set using this->SetWeightForInput(i,w).
//
// .SECTION Description
// All weights are normalized so they will sum to 1.
// Images must have the same extents. Output is always type float.
//  
//
#ifndef __vtkImageWeightedSum_h
#define __vtkImageWeightedSum_h

#include "vtkImageData.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkFloatArray.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageWeightedSum : public vtkImageMultipleInputFilter
{
  public:
  static vtkImageWeightedSum *New();
  vtkTypeMacro(vtkImageWeightedSum,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // The weights control the contribution of each input to the sum.
  // They will be normalized to sum to 1 before filter execution.
  float GetWeightForInput(int i);
  float GetNormalizedWeightForInput(int i);
  void SetWeightForInput(int i, float w);

  // Description:
  // This function is called by vtkImageWeightedSumExecute.
  // It makes sure a weight exists for each input image
  // and normalizes the weights.
  void CheckWeights();

  protected:

  vtkImageWeightedSum();
  ~vtkImageWeightedSum();
  
  vtkFloatArray * Weights;

  void NormalizeWeights();

  void ExecuteInformation(vtkImageData **inputs, vtkImageData *output); 
  void ExecuteInformation(){this->vtkImageMultipleInputFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData, 
               int extent[6], int id);
};

#endif

