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
#ifndef vtkImageMIReg_H
#define vtkImageMIReg_H

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkRasToIjkTransform.h"
#include "vtkTimeStamp.h"
#include "vtkMath.h"
#include "vtkVector3.h"
#include "vtkQuaternion.h"
#include "vtkPose.h"

class VTK_EXPORT vtkImageMIReg : public vtkObject
{
public:
  // VTK requisites
  static vtkImageMIReg *New();
  vtkTypeMacro(vtkImageMIReg,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Inputs/Outputs
  vtkGetObjectMacro(Reference, vtkImageData);
  vtkSetObjectMacro(Reference, vtkImageData);

  vtkGetObjectMacro(Subject, vtkImageData);
  vtkSetObjectMacro(Subject, vtkImageData);

  vtkGetObjectMacro(RefTrans, vtkRasToIjkTransform);
  vtkSetObjectMacro(RefTrans, vtkRasToIjkTransform);

  vtkGetObjectMacro(SubTrans, vtkRasToIjkTransform);
  vtkSetObjectMacro(SubTrans, vtkRasToIjkTransform);

  vtkGetObjectMacro(InitialPose, vtkPose);
  vtkSetObjectMacro(InitialPose, vtkPose);

  vtkGetObjectMacro(CurrentPose, vtkPose);
  vtkGetObjectMacro(FinalPose, vtkPose);

  // Parameters
  vtkGetMacro(SampleSize, int);
  vtkSetMacro(SampleSize, int);
  vtkGetMacro(SigmaUU, float);
  vtkSetMacro(SigmaUU, float);
  vtkGetMacro(SigmaVV, float);
  vtkSetMacro(SigmaVV, float);
  vtkGetMacro(SigmaV, float);
  vtkSetMacro(SigmaV, float);
  vtkGetMacro(PMin, float);
  vtkSetMacro(PMin, float);
  vtkGetMacro(UpdateIterations, int);
  vtkSetMacro(UpdateIterations, int);

  // Parameters per resolution
  vtkSetVector4Macro(NumIterations, int);
  vtkGetVector4Macro(NumIterations, int);
  vtkSetVector4Macro(LambdaDisplacement, float);
  vtkGetVector4Macro(LambdaDisplacement, float);
  vtkSetVector4Macro(LambdaRotation, float);
  vtkGetVector4Macro(LambdaRotation, float);

  // Downsampled images (made accessible for interactive rendering)
  vtkImageData *GetRef(int res) {return this->Refs[res];};
  vtkImageData *GetSub(int res) {return this->Subs[res];};
  vtkRasToIjkTransform *GetRefRasToIjk(int res) {return this->RefRasToIjk[res];};
  vtkRasToIjkTransform *GetSubRasToIjk(int res) {return this->SubRasToIjk[res];};

  // Pipeline
  void Update();

  // Status reporting
  vtkGetMacro(InProgress, int);
  int GetResolution();
  int GetIteration();
  vtkSetMacro(RunTime, int);
  vtkGetMacro(RunTime, int);

protected:
  // Constructor/Destructor
  vtkImageMIReg();
    ~vtkImageMIReg();

  vtkImageData *Reference;
  vtkImageData *Subject;
  vtkRasToIjkTransform *RefTrans;
  vtkRasToIjkTransform *SubTrans;
  vtkPose *InitialPose;
  vtkPose *FinalPose;

  vtkImageData *Subs[4];
  vtkImageData *Refs[4];
  vtkRasToIjkTransform *RefRasToIjk[4];
  vtkRasToIjkTransform *SubRasToIjk[4];
  vtkPose *CurrentPose;

  float LambdaDisplacement[4];
  float LambdaRotation[4];
  int NumIterations[4];
  int CurIteration[4];

  vtkTimeStamp UTime;
  int RunTime;
  int SampleSize;
  float PMin;
  float SigmaUU;
  float SigmaVV;
  float SigmaV;
  int InProgress;
  int UpdateIterations;

  // Pipeline
  void Reset();
  int Initialize();
  void Execute();
  void Cleanup();

  // Helpers
  double GetGradientAndInterpolation(vtkVector3 *rasGrad, 
    vtkImageData *data, vtkRasToIjkTransform *rasToIjk, vtkVector3 *ras);
};
#endif
