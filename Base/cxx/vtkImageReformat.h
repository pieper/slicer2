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
// .NAME vtkImageReformat -  Reformats a 2D image from a 3D volume.
// .SECTION Description
// vtkImageReformat allows interpolation or replication.
//


#ifndef __vtkImageReformat_h
#define __vtkImageReformat_h

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkTensor.h"
#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

class VTK_SLICER_BASE_EXPORT vtkImageReformat : public vtkImageToImageFilter
{
  public:
    static vtkImageReformat *New();
  vtkTypeMacro(vtkImageReformat,vtkImageToImageFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkGetMacro(Interpolate, int);
    vtkSetMacro(Interpolate, int);
    vtkBooleanMacro(Interpolate, int);

    //Description: Wld stands for the world coordinates
    vtkGetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
    vtkSetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);

    vtkGetObjectMacro(ReformatMatrix, vtkMatrix4x4);
    vtkSetObjectMacro(ReformatMatrix, vtkMatrix4x4);

    //Description: reformatted image in pixels
    // more stuff
    vtkGetMacro(Resolution, int);
    vtkSetMacro(Resolution, int);
    
    //Description: plane in world space
    vtkGetMacro(FieldOfView, vtkFloatingPointType);
    vtkSetMacro(FieldOfView, vtkFloatingPointType);
    
    // >>Karl Krissian - added following 4 functions for new draw ---------------
    //void vtkImageReformat::CrossProduct(float* v1, float* v2, float* v3);
    void CrossProduct(vtkFloatingPointType* v1, vtkFloatingPointType* v2, vtkFloatingPointType* v3);
    void SetPoint(int x, int y);
    void Slice2IJK(int slice_x, int slice_y,float& x, float& y,float& z);
    void IJK2Slice( float x, float y, float z, int& slice_x, int& slice_y);    
    // << Karl --------------------------------------------------------------
    
    vtkGetVector3Macro(WldPoint, vtkFloatingPointType);
    vtkGetVector3Macro(IjkPoint, vtkFloatingPointType);

    vtkFloatingPointType YStep[3];
    vtkFloatingPointType XStep[3];
    vtkFloatingPointType Origin[3];
  
  // >> AT 11/07/01

  // Description
  // XY vector from center of image to center of panned image
  // comes from GUI (MainInteractorPan)
  vtkGetVector2Macro(OriginShift, vtkFloatingPointType);
  vtkSetVector2Macro(OriginShift, vtkFloatingPointType);

  // Description
  // Zoom factor coming from GUI (MainInteractorZoom)
  vtkGetMacro(Zoom, vtkFloatingPointType);
  vtkSetMacro(Zoom, vtkFloatingPointType);

  // Description
  // Pixel size of the reformatted image in mm
  vtkGetMacro(PanScale, vtkFloatingPointType);
  // Description
  // For internal class use only
  vtkSetMacro(PanScale, vtkFloatingPointType);

  // Description
  vtkGetObjectMacro(OriginShiftMtx, vtkMatrix4x4);
  //vtkSetObjectMacro(OriginShiftMtx, vtkMatrix4x4);
  // << AT 11/07/01

  vtkGetMacro(RunTime, int);
  vtkSetMacro(RunTime, int);

  // Description:
  // When determining the modified time of the filter, 
  // this checks the modified time of the matrices.
  unsigned long GetMTime();

protected:
    vtkImageReformat();
    ~vtkImageReformat();
    vtkImageReformat(const vtkImageReformat&) {};
    void operator=(const vtkImageReformat&) {};

  // >> AT 11/07/01
  vtkFloatingPointType OriginShift[2];
  vtkFloatingPointType Zoom;
  vtkFloatingPointType PanScale;
  vtkMatrix4x4 *OriginShiftMtx;
  // << AT 11/07/01

    int RunTime;
    vtkFloatingPointType IjkPoint[3];
    vtkFloatingPointType WldPoint[3];
    int Resolution;
    vtkFloatingPointType FieldOfView;
    int Interpolate;
    vtkMatrix4x4* ReformatMatrix;
    vtkMatrix4x4* WldToIjkMatrix;

    // Override this function since inExt != outExt
    void ComputeInputUpdateExtent(int inExt[6],int outExt[6]);
  
    void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
    void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
        int extent[6], int id);

  // We override this in order to allocate output tensors
  // before threading happens.  This replaces the superclass 
  // vtkImageToImageFilter's Execute function.
  //void Execute();
  void ExecuteData(vtkDataObject *out);

};

#endif



