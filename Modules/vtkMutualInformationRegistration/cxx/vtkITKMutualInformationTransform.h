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
// Thanks to Steve Pieper who wrote the initial version of the class.

#include "vtkLinearTransform.h"
#include "vtkMutualInformationRegistrationConfigure.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkUnsignedIntArray.h"
class vtkMatrix4x4;
class vtkImageFlip;

class VTK_MUTUALINFORMATIONREGISTRATION_EXPORT vtkITKMutualInformationTransform : public vtkLinearTransform
{
public:
  static vtkITKMutualInformationTransform *New();

  vtkTypeRevisionMacro(vtkITKMutualInformationTransform,vtkLinearTransform);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the source and target images. The two images must have
  // the same scalar type. Otherwise, the images can differ in scaling, 
  // resolution, etc.
  vtkSetObjectMacro(SourceImage, vtkImageData);
  vtkGetObjectMacro(SourceImage, vtkImageData);

  vtkSetObjectMacro(TargetImage, vtkImageData);
  vtkGetObjectMacro(TargetImage, vtkImageData);

  // Description:
  // Set the standard deviations of the parzen window density estimators.
  vtkSetMacro(SourceStandardDeviation, double);
  vtkGetMacro(SourceStandardDeviation, double);
  vtkSetMacro(TargetStandardDeviation, double);
  vtkGetMacro(TargetStandardDeviation, double);

  // Description:
  // Set the number of sample points for density estimation
  vtkSetMacro(NumberOfSamples, int);
  vtkGetMacro(NumberOfSamples, int);

  // Description:
  // Set the translation scale factor.
  vtkSetMacro(TranslateScale, double);
  vtkGetMacro(TranslateScale, double);

  // Description:
  // Did the last run finish with an error?
  // Set to 0 if no error, 1 otherwise.
  vtkSetMacro(Error, int);
  vtkGetMacro(Error, int);

  // Description:
  // Set the shrink factors for pyramid schemes.
  // Default is 1 1 1 
  void SetSourceShrinkFactors(unsigned int i, 
                              unsigned int j, unsigned int k);
  void SetTargetShrinkFactors(unsigned int i, 
                              unsigned int j, unsigned int k);
  unsigned int GetSourceShrinkFactors(const int &dir)
    { return SourceShrink[dir]; }
  unsigned int GetTargetShrinkFactors(const int &dir)
    { return TargetShrink[dir]; }

  // Description:
  // Reset the Multiresolution Settings
  // It blanks the LearningRate and NumberOfIterations
  void ResetMultiResolutionSettings() 
    { LearningRate->Reset(); MaxNumberOfIterations->Reset(); };

  // Description:
  // Set the learning rate for the algorithm.
  // Generally between 0 and 1, most often 1e-4 or below
  // Must set the same number of Learning Rates as Iterations
  void SetNextLearningRate(const double rate);

  // Description:
  // Set the max number of iterations at each level
  // Generally less than 5000, 2500 is OK.
  // Must set the same number of Learning Rates as Iterations
  void SetNextMaxNumberOfIterations(const int num);

  // Description
  // The Max Number of Iterations at each multi-resolution level.
  vtkSetObjectMacro(MaxNumberOfIterations,vtkUnsignedIntArray);
  vtkGetObjectMacro(MaxNumberOfIterations,vtkUnsignedIntArray);

  // Description
  // The Learning Rates at each multi-resolution level.
  vtkSetObjectMacro(LearningRate,vtkDoubleArray);
  vtkGetObjectMacro(LearningRate,vtkDoubleArray);

  // Description:
  // Get the value of the last metric calculation
  // (Set is for internal use only).
  vtkSetMacro(MetricValue, double);
  vtkGetMacro(MetricValue, double);

  // Description:
  // Invert the transformation.  This is done by switching the
  // source and target images. This is does not work because the
  // transform is not touched.
  void Inverse();

  // Description:
  // Initialize the transformation to a Matrix
  void Initialize(vtkMatrix4x4 *mat);

  // Description:
  // Get the resulting found matrix
  vtkMatrix4x4 *GetOutputMatrix();

  // Description:
  // Get the MTime.
  unsigned long GetMTime();

  // Description:
  // Should we flip the Target Z Axis
  // Used during execute
  vtkGetMacro(FlipTargetZAxis, double);

  // Description:
  // Get the ImageFlip()
  // Called during the execute function
  vtkGetObjectMacro(ImageFlip, vtkImageFlip);

  // Description:
  // Test Initialization of the Matrix, which is a pure rotation/translation
  // returns 0 on success. Also calls MIRegistration->TestParamToMatrix.
  int TestMatrixInitialize(vtkMatrix4x4 *aMat);

protected:
  vtkITKMutualInformationTransform();
  ~vtkITKMutualInformationTransform();

  // Description:
  // Do not use this routine
  // This is not the correct matrix when a z-flip exists.
  vtkMatrix4x4 *GetMatrix()
    { return this->vtkLinearTransform::GetMatrix(); }

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

  int FlipTargetZAxis;     // 1 if flipped z-axis on target
  vtkImageFlip *ImageFlip;
  vtkMatrix4x4 *ZFlipMat;
  vtkMatrix4x4 *OutputMatrix;

  double SourceStandardDeviation;
  double TargetStandardDeviation;
  double TranslateScale;
  int NumberOfSamples;

  double MetricValue;

  unsigned int SourceShrink[3];
  unsigned int TargetShrink[3];

  int Error;

  vtkUnsignedIntArray  *MaxNumberOfIterations;
  vtkDoubleArray       *LearningRate;
  
private:
  vtkITKMutualInformationTransform(const vtkITKMutualInformationTransform&);  // Not implemented.
  void operator=(const vtkITKMutualInformationTransform&);  // Not implemented.
};
  
#endif
