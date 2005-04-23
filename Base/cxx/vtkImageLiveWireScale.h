/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
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
// .NAME vtkImageLiveWireScale - General scaling of images for input to LiveWire
// .SECTION Description
// This class outputs images whose values range from 0 to 1. (or
// from 0 to this->ScaleFactor if it is set).  This is needed to 
// control magnitude of input images to livewire.
//
// Can scale in the following ways:
// divide input by max value (after shifting input to begin at value 0)
// do the above, then pass through a function
//
// Can use the input as is, or ignore input values less than LowerCutoff
// or greater than UpperCutoff.
//

#ifndef __vtkImageLiveWireScale_h
#define __vtkImageLiveWireScale_h


#include "vtkImageToImageFilter.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

class VTK_SLICER_BASE_EXPORT vtkImageLiveWireScale : public vtkImageToImageFilter
{
  public:
  static vtkImageLiveWireScale *New();
  vtkTypeMacro(vtkImageLiveWireScale,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // scale factor to multiply image by 
  // (so output is <=ScaleFactor instead of <=1)
  vtkSetMacro(ScaleFactor,int);
  vtkGetMacro(ScaleFactor,int);

  vtkSetMacro(UseTransformationFunction,int);
  vtkGetMacro(UseTransformationFunction,int);

  vtkSetMacro(TransformationFunctionNumber,int);
  vtkGetMacro(TransformationFunctionNumber,int);

#define INVERSE_LINEAR_RAMP 1
#define ONE_OVER_X 2

  void SetTransformationFunctionToOneOverX() {
    this->TransformationFunctionNumber = ONE_OVER_X;
    this->UseTransformationFunction = 1;
  };
  void SetTransformationFunctionToInverseLinearRamp() {
    this->TransformationFunctionNumber = INVERSE_LINEAR_RAMP;    
    this->UseTransformationFunction = 1;
  };
  void SetUpperCutoff(vtkFloatingPointType num) {
    this->UpperCutoff = num;    
    this->UseUpperCutoff = 1;
    this->Modified();
  };
  void SetLowerCutoff(vtkFloatingPointType num) {
    this->LowerCutoff = num;    
    this->UseLowerCutoff = 1;
    this->Modified();
  };
  // just here for access from Execute.
  vtkFloatingPointType TransformationFunction(vtkFloatingPointType intensity, vtkFloatingPointType max, vtkFloatingPointType min);

  protected:
  vtkImageLiveWireScale();
  ~vtkImageLiveWireScale();
  vtkImageLiveWireScale(const vtkImageLiveWireScale&) {};
  void operator=(const vtkImageLiveWireScale&) {};
  
  int ScaleFactor;
  vtkFloatingPointType UpperCutoff;
  vtkFloatingPointType LowerCutoff;
  int UseUpperCutoff;
  int UseLowerCutoff;

  int UseTransformationFunction;
  int TransformationFunctionNumber;

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void UpdateData(vtkDataObject *data);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
                       int ext[6], int id);
};

#endif



