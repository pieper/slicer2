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
/*=========================================================================

  Program:   Samson Timoner TetraMesh Library
  Module:    $RCSfile: vtkResliceImage.h,v $
  Language:  C++
  Date:      $Date: 2002/05/08 18:57:51 $
  Version:   $Revision: 1.9 $
  
Copyright (c) 2001 Samson Timoner

This software is not to be edited, distributed, copied, moved, etc.
without express permission of the author. 

========================================================================= */
// .NAME vtkResliceImage - Reslices an image in another coordinate system
// .SECTION Description
// This filter takes in a volume, and resamples it in the coordinates of
// the next volume. A transform that takes the coordinates (mm) in the image
// whose coordinates we will use into the input image coordinates (mm) is 
// possible.
//
// Please note that this program (by Samson Timoner) is also in his
// personal CVS tree. If you make changes, please let him know.
//

#ifndef __vtkDeformImage_h
#define __vtkDeformImage_h

class vtkMatrix4x4;
class vtkUnstructuredGrid;

#include "vtkImageToImageFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkResliceImage : public vtkImageToImageFilter
{
public:
  // Description:
  // Default:: No transform between the coordinate systems
  // Output Extent of 1 pixel, spacing of (1,1,1), and origin of (0,0,0)
  static vtkResliceImage *New();
  vtkTypeMacro(vtkResliceImage,vtkImageToImageFilter);

 // Description:
  // The output image will have the same spacing and origin and extent
  // as this Volume.
  void SetOutputImageParam(vtkImageData *VolumeToCopyParam);

  void PrintSelf(ostream& os, vtkIndent indent);


  // Description:
  // Set the transformation between the first and final images
  // This transform takes points in the output volume's continuous coordinates
  // (mm) and maps them to the input volume's continuous coordinates (mm).
  // If set to NULL, the transform is the identity.
  vtkSetObjectMacro(TransformOutputToInput,vtkMatrix4x4);
  vtkGetObjectMacro(TransformOutputToInput,vtkMatrix4x4);
 

  // Description:
  // Set the background scalar to use if there is no 
  // information in the first image in the area specified.
  vtkSetMacro(Background,float);
  vtkGetMacro(Background,float);

  // Helper Functions:
  //

  //BTX
  static void FindInputIJK(float OtherIJK[4],
                           vtkMatrix4x4 *IJKtoIJK,
                           int i, int j, int k);

  static vtkMatrix4x4 *GetIJKtoIJKMatrix(float Spacing2[3],
                                         float Origin2[3],
                                         vtkMatrix4x4 *MM2toMM1,
                                         float Spacing1[3],
                                         float Origin1[3]);
protected:

  vtkResliceImage();
  ~vtkResliceImage() {};
  vtkResliceImage(const vtkResliceImage&) {};
  void operator=(const vtkResliceImage&) {};

  vtkMatrix4x4     *TransformOutputToInput;
  vtkMatrix4x4     *IJKtoIJK;
  float OutSpacing[3];
  float OutOrigin[3];
  int OutExtent[6];
  float Background;

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
               int outExt[6], int id);
  //ETX
};
#endif /* DeformImage_h */
