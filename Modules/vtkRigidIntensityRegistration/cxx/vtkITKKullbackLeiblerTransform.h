/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#ifndef __vtkITKKLTransform_h
#define __vtkITKKLTransform_h
// .NAME vtkITKKLTransform - a linear transform specified by two images
// .SECTION Description
// KLTransform computes a transformation that will align
// the source image with the target image.  The transform can be
// initialized with a vtkLinearTransform or reset with the Identity
// method.
// The algorithm is described in the paper: Viola, P. and Wells III,
// W. (1997).  "Alignment by Maximization of Mutual Information"
// International Journal of Computer Vision, 24(2):137-154
//
// This class was adopted by a transform first written by
// Steve Pieper. It was also strongly derived from one of
// the ITK application Examples: the MultiResolutionMIRegistration.
//
// .Section Dealing with Flips
//  The registration algorithm is based on quaternions, so any registration
//  that requires an inversion (flip), rotation and translation cannot be
//  represented. Thus, sometimes we deal with flips by flipping the image first
//
//  When the matrix is initialized, it is checked for negative
//  determinant If it has negative determinant, the target image is
//  flipped along the z-axis. The matrix is then modified
//  appropriately, and then determined from source to flipped target
//  image. The resulting matrix is then modified appropriately and returned.
//  (Note, there is an assumption that the images are centered....)
//
// .SECTION see also
// vtkLinearTransform
//
// .SECTION Thanks
// Thanks to Samson Timoner who wrote this class.

#include "vtkLinearTransform.h"

#include "vtkITKRigidRegistrationTransformBase.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

class VTK_RIGIDINTENSITYREGISTRATION_EXPORT vtkITKKullbackLeiblerTransform : public vtkITKRigidRegistrationTransformBase
{
public:
  static vtkITKKullbackLeiblerTransform *New();

  vtkTypeMacro(vtkITKKullbackLeiblerTransform,vtkITKRigidRegistrationTransformBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the images that are already aligned
  vtkSetObjectMacro(TrainingSourceImage, vtkImageData);
  vtkSetObjectMacro(TrainingTargetImage, vtkImageData);
  vtkGetObjectMacro(TrainingSourceImage, vtkImageData);
  vtkGetObjectMacro(TrainingTargetImage, vtkImageData);

  // Description:
  // Specify the Training transform between the Training Images
  vtkSetObjectMacro(TrainingTransform, vtkMatrix4x4);
  vtkGetObjectMacro(TrainingTransform, vtkMatrix4x4);

  // Description:
  // Set the size of the histogram for the metric
  // Default is 32 by 32
  vtkSetMacro(HistSizeSource, int);
  vtkGetMacro(HistSizeSource, int);
  vtkSetMacro(HistSizeTarget, int);
  vtkGetMacro(HistSizeTarget, int);

  // Description:
  // Set the histogram frequency to use if the frequency is 0
  // This number should be very small compared with the number
  // of bins in the histogram. 1e-12 is the default.
  vtkSetMacro(HistEpsilon, double);
  vtkGetMacro(HistEpsilon, double);

  // Description:
  // Get the MTime.
  unsigned long GetMTime();

protected:
  vtkITKKullbackLeiblerTransform();
  ~vtkITKKullbackLeiblerTransform();

  // Update the matrix from the quaternion.
  void InternalUpdate();

  // Description:
  // Make another transform of the same type.
  vtkAbstractTransform *MakeTransform();

  // Description:
  // This method does no type checking, use DeepCopy instead.
  void InternalDeepCopy(vtkAbstractTransform *transform);

  vtkImageData *TrainingSourceImage;
  vtkImageData *TrainingTargetImage;
  vtkMatrix4x4 *TrainingTransform;

  int HistSizeSource;
  int HistSizeTarget;
  double HistEpsilon;
  void *TrainingHistogram;

private:
  vtkITKKullbackLeiblerTransform(const vtkITKKullbackLeiblerTransform&);  // Not implemented.
  void operator=(const vtkITKKullbackLeiblerTransform&);  // Not implemented.
};
  
#endif
