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
#include "vtkMrmlVolumeNode.h"
#include "vtkTimeStamp.h"
#include "vtkMath.h"

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

  vtkGetObjectMacro(RefNode, vtkMrmlVolumeNode);
  vtkSetObjectMacro(RefNode, vtkMrmlVolumeNode);

  vtkGetObjectMacro(SubNode, vtkMrmlVolumeNode);
  vtkSetObjectMacro(SubNode, vtkMrmlVolumeNode);

  vtkGetObjectMacro(InitialPose, vtkMatrix4x4);
  vtkSetObjectMacro(InitialPose, vtkMatrix4x4);

  vtkGetObjectMacro(CurrentPose, vtkMatrix4x4);
  vtkGetObjectMacro(FinalPose, vtkMatrix4x4);

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
  vtkMatrix4x4 *GetRefRasToIjk(int res) {return this->RefRasToIjk[res];};
  vtkMatrix4x4 *GetSubRasToIjk(int res) {return this->SubRasToIjk[res];};

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
  vtkMrmlVolumeNode *RefNode;
  vtkMrmlVolumeNode *SubNode;
  vtkMatrix4x4 *InitialPose;
  vtkMatrix4x4 *FinalPose;

  vtkImageData *Subs[4];
  vtkImageData *Refs[4];
  vtkMatrix4x4 *RefIjkToRas[4];
  vtkMatrix4x4 *RefRasToIjk[4];
  vtkMatrix4x4 *SubRasToIjk[4];
  vtkMatrix4x4 *CurrentPose;

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
  double GetGradientAndInterpolation(double *rasGrad, 
    vtkImageData *data, vtkMatrix4x4 *rasToIjk, double *ras, char *sliceOrder);
};

#endif
