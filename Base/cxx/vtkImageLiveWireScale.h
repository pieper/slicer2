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
// .NAME vtkImageLiveWireScale - General scaling of images for input to LiveWire
// .SECTION Description
// This class outputs images whose values range from 0 to 1. (or
// from 0 to this->ScaleFactor if it is set).  This is needed to 
// control magnitude of input images to livewire.
//
// Can scale in the following ways:
// divide input by max value (after shifting input to begin at value 0)
// do the above, then pass through a function
// use a lookup table to convert input values to output
//
// also has some support for training to create the lookup table
// this is under construction
//
// Can use the input as is, or ignore input values less than LowerCutoff
// or greater than UpperCutoff.
//

#ifndef __vtkImageLiveWireScale_h
#define __vtkImageLiveWireScale_h


#include "vtkImageToImageFilter.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"

class VTK_EXPORT vtkImageLiveWireScale : public vtkImageToImageFilter
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

  vtkSetMacro(UseLookupTable,int);
  vtkGetMacro(UseLookupTable,int);
  vtkSetMacro(UseGaussianLookup,int);
  vtkGetMacro(UseGaussianLookup,int);

  vtkSetMacro(UseTransformationFunction,int);
  vtkGetMacro(UseTransformationFunction,int);

  vtkSetMacro(TransformationFunctionNumber,int);
  vtkGetMacro(TransformationFunctionNumber,int);

  vtkSetObjectMacro(LookupPoints,vtkPoints);

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
  void SetUpperCutoff(float num) {
    this->UpperCutoff = num;    
    this->UseUpperCutoff = 1;
    this->Modified();
  };
  void SetLowerCutoff(float num) {
    this->LowerCutoff = num;    
    this->UseLowerCutoff = 1;
    this->Modified();
  };
  // just here for access from Execute.
  float TransformationFunction(float intensity, float max, float min);
  float TableLookup(float intensity, float max, float min);
  float GaussianLookup(float intensity, float max, float min);
  //vtkPriorityQueue * GetLookupTable(){return this->LookupTable;};
  vtkFloatArray * GetLookupTable(){return this->LookupTable;};
  vtkPoints * GetLookupPoints(){return this->LookupPoints;};
  vtkSetMacro(TotalPointsInLookupTable,int);
  vtkGetMacro(TotalPointsInLookupTable,int);
  vtkSetMacro(MaxPointsInLookupTableBin,int);
  vtkGetMacro(MaxPointsInLookupTableBin,int);
  vtkSetMacro(MinimumBin,int);
  vtkGetMacro(MinimumBin,int);
  vtkSetMacro(MaximumBin,int);
  vtkGetMacro(MaximumBin,int);

  vtkSetMacro(MeanForGaussianModel,int);
  vtkGetMacro(MeanForGaussianModel,int);
  vtkSetMacro(VarianceForGaussianModel,int);
  vtkGetMacro(VarianceForGaussianModel,int);

  protected:
  vtkImageLiveWireScale();
  ~vtkImageLiveWireScale();
  vtkImageLiveWireScale(const vtkImageLiveWireScale&) {};
  void operator=(const vtkImageLiveWireScale&) {};
  
  int ScaleFactor;
  float UpperCutoff;
  float LowerCutoff;
  int UseUpperCutoff;
  int UseLowerCutoff;

  int UseLookupTable;
  int UseGaussianLookup;
  int MeanForGaussianModel;
  int VarianceForGaussianModel;

  int UseTransformationFunction;
  int TransformationFunctionNumber;

  //vtkPriorityQueue *LookupTable;
  vtkFloatArray *LookupTable;
  vtkPoints *LookupPoints;
  int TotalPointsInLookupTable;
  int MaxPointsInLookupTableBin;
  int MinimumBin;
  int MaximumBin;

  void Execute(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(vtkImageData *vtkNotUsed(input), 
              vtkImageData *output);
  // Description:
  // Generate more than requested.  Called by the superclass before
  // an execute, and before output memory is allocated.
  void EnlargeOutputUpdateExtents( vtkDataObject *data );
  
};

#endif



