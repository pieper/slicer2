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
// .NAME vtkImageReformatIJK -  
// .SECTION Description
// non-oblique reformatting (axial, sagittal, coronal orientations only).
//
// Description added by odonnell 4/02:
// This class is never used for actual reformatting (i.e. image generation).
// It is just used in two places in vtkMrmlSlicer.cxx.
// It is used to compute the reformat matrix for IJK-based reformatting
// and also for converting points to IJK for manual editing.
// The sequence of events (from vtkMrmlSlicer.cxx) is:
//
//  vtkImageReformatIJK *ijk = this->ReformatIJK;
//  ijk->SetWldToIjkMatrix(node->GetWldToIjk());
//  ijk->SetInput(vol->GetOutput());
//  ijk->SetInputOrderString(node->GetScanOrder());
//  ijk->SetOutputOrderString(orderString);
//  ijk->SetSlice(offset);
//  ijk->ComputeTransform();
//  ijk->ComputeOutputExtent();
//  ijk->ComputeReformatMatrix(ref);
//
//
//  So this class just encapsulates the logic needed to calculate this matrix.
//
//  Note the reformat matrix is the matrix that would move a standard
//  axial plane through the origin into the location where the slice
//  is desired.  So this is defined in world space in the slicer, NOT
//  IJK.  So this file basically converts the IJK notion of a slice
//  through the array into the appropriate world-space matrix, using
//  knowledge of the volume's location/scale from this->WldToIjkMatrix
//  and using the extent of the image data to see where this slice
//  should be placed in world space.
//

#ifndef __vtkImageReformatIJK_h
#define __vtkImageReformatIJK_h

#include "vtkImageToImageFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkIntArray.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageReformatIJK : public vtkImageToImageFilter
{
public:
    static vtkImageReformatIJK *New();
  vtkTypeMacro(vtkImageReformatIJK,vtkImageToImageFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // 
    vtkGetVector4Macro(XStep, float);
    vtkGetVector4Macro(YStep, float);
    vtkGetVector4Macro(ZStep, float);
    vtkGetVector4Macro(Origin, float);

  // Description:
  // 
    vtkSetMacro(Slice, int);
    vtkGetMacro(Slice, int);

  // Description:
  // 
  vtkSetMacro(InputOrder, int);
  vtkGetMacro(InputOrder, int);
  vtkSetMacro(OutputOrder, int);
  vtkGetMacro(OutputOrder, int);
  
  // Description:
  // 
  void SetInputOrderString(char *str);
  void SetOutputOrderString(char *str);

    vtkGetObjectMacro(Indices, vtkIntArray);

    vtkGetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
    vtkSetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);

  void ComputeReformatMatrix(vtkMatrix4x4 *ref);

  void SetIJKPoint(int i, int j, int k);
  vtkGetVectorMacro(XYPoint, int, 2);

    vtkMatrix4x4* WldToIjkMatrix;
  int NumSlices;
  vtkGetMacro(NumSlices, int);
  float XStep[4];
  float YStep[4];
  float ZStep[4];
  float Origin[4];
  int IJKPoint[3];
  int XYPoint[2];
  int Slice;
  vtkTransform *tran;
  int InputOrder;
  int OutputOrder;
  vtkIntArray *Indices;

  void ComputeTransform();
  // test (wrap problem: the above fcn is not getting wrapped)
  void ComputeTransform2(){this->ComputeTransform();};

  void ComputeOutputExtent();

protected:
  vtkImageReformatIJK();
  ~vtkImageReformatIJK();
    vtkImageReformatIJK(const vtkImageReformatIJK&) {};
    void operator=(const vtkImageReformatIJK&) {};

  vtkTimeStamp TransformTime;
  int OutputExtent[6];

    // Override this function since inExt != outExt
    void ComputeInputUpdateExtent(int inExt[6],int outExt[6]);
    void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);

    void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif



