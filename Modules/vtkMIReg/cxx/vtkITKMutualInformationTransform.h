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
#ifndef __vtkITKMutualInformationTransform_h
#define __vtkITKITKMutualInformationTransform_h
// .NAME vtkITKMutualInformationTransform - a linear transform specified by two images
// .SECTION Description
// MutualInformationTransform computes a transformation that will align
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
// .SECTION see also
// vtkLinearTransform
//
// .SECTION Thanks
// Thanks to Samson Timoner who wrote this class.

#include "vtkLinearTransform.h"

class vtkImageData;

class VTK_EXPORT vtkITKMutualInformationTransform : public vtkLinearTransform
{
public:
  static vtkITKMutualInformationTransform *New();

  vtkTypeRevisionMacro(vtkITKMutualInformationTransform,vtkLinearTransform);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the source and target images. The two images must have
  // the same scalar type.
  // Otherwise, the images can differ in scaling, resolution, etc.
  void SetSourceImage(vtkImageData *source);
  void SetTargetImage(vtkImageData *target);
  vtkGetObjectMacro(SourceImage, vtkImageData);
  vtkGetObjectMacro(TargetImage, vtkImageData);

  // Description:
  // Set the learning rate for the algorithm.
  vtkSetMacro(LearningRate, double);
  vtkGetMacro(LearningRate, double);

  // Description:
  // Set the standard deviations of the parzen window density estimators.
  vtkSetMacro(SourceStandardDeviation, double);
  vtkGetMacro(SourceStandardDeviation, double);
  vtkSetMacro(TargetStandardDeviation, double);
  vtkGetMacro(TargetStandardDeviation, double);

  // Description:
  // Set the number of iterations
  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);

  // Description:
  // Set the number of sample points for density estimation
  vtkSetMacro(NumberOfSamples, int);
  vtkGetMacro(NumberOfSamples, int);

  // Description:
  // Set the translation scale factor.
  vtkSetMacro(TranslateScale, double);
  vtkGetMacro(TranslateScale, double);

  // Description:
  // Get the value of the last metric calculation
  vtkSetMacro(MetricValue, double);
  vtkGetMacro(MetricValue, double);

  // Description:
  // Invert the transformation.  This is done by switching the
  // source and target images.
  void Inverse();

  // Description:
  // Set the transformation to the Identity matrix.
  void Identity();

  // Description:
  // Initialize the transformation to a linear transform.
  void Initialize(vtkLinearTransform *);

  // Description:
  // Get the MTime.
  unsigned long GetMTime();


protected:
  vtkITKMutualInformationTransform();
  ~vtkITKMutualInformationTransform();

  // Update the matrix from the quaternion.
  void InternalUpdate();

  // Description:
  // Make another transform of the same type.
  vtkAbstractTransform *MakeTransform();

  // Description:
  // This method does no type checking, use DeepCopy instead.
  void InternalDeepCopy(vtkAbstractTransform *transform);

  vtkImageData *SourceImage;
  vtkImageData *TargetImage;
 
  double LearningRate;
  double SourceStandardDeviation;
  double TargetStandardDeviation;
  double TranslateScale;
  
  int NumberOfIterations;
  int NumberOfSamples;

  double MetricValue;
  
private:
  vtkITKMutualInformationTransform(const vtkITKMutualInformationTransform&); // Not implemented.
  void operator=(const vtkITKMutualInformationTransform&);  // Not implemented.
};
  
#endif
