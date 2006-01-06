/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSuperquadricTensorGlyph.h,v $
  Date:      $Date: 2006/01/06 17:58:06 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkSuperquadricTensorGlyph - scale and orient glyph according to tensor eigenvalues and eigenvectors
// .SECTION Description
// vtkSuperquadricTensorGlyph is a filter that copies a geometric representation (specified
// as polygonal data) to every input point. The geometric representation, or 
// glyph, can be scaled and/or rotated according to the tensor at the input 
// point. Scaling and rotation is controlled by the eigenvalues/eigenvectors
// of the tensor as follows. For each tensor, the eigenvalues (and associated
// eigenvectors) are sorted to determine the major, medium, and minor 
// eigenvalues/eigenvectors. The major eigenvalue scales the glyph in the 
// x-direction, the medium in the y-direction, and the minor in the 
// z-direction. Then, the glyph is rotated so that the glyph's local x-axis 
// lies along the major eigenvector, y-axis along the medium eigenvector, and
// z-axis along the minor.
//
// A scale factor is provided to control the amount of scaling. Also, you 
// can turn off scaling completely if desired. The boolean variable 
// ClampScaling controls the maximum scaling (in conjunction with
// MaxScaleFactor.) This is useful in certain applications where 
// singularities or large order of magnitude differences exist in 
// the eigenvalues.
//
// Another instance variable, ExtractEigenvalues, has been provided to 
// control extraction of eigenvalues/eigenvectors. If this boolean is false,
// then eigenvalues/eigenvectors are not extracted, and the columns of the
// tensor are taken as the eigenvectors (norm of column is eigenvalue). 
// This allows additional capability over the vtkGlyph3D object. That is, the
// glyph can be oriented in three directions instead of one.
//
// To accomodate display in another coordinate system besides the ijk
// coordinate system of the volume array, two matrices are provided.
// The VolumePosition matrix positions each tensor in the correct location
// to account for voxel translation and rotation of the volume.
// (Scaling is already handled well in vtk ImageData.)
// The TensorRotationMatrix rotates each tensor into the new
// coordinate system.

// .SECTION See Also
// vtkTensorGlyph vtkGlyph3D vtkPointLoad vtkHyperStreamline

#ifndef __vtkSuperquadricTensorGlyph_h
#define __vtkSuperquadricTensorGlyph_h

#include "vtkTensorUtilConfigure.h"
#include "vtkTensorGlyph.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"

class VTK_TENSORUTIL_EXPORT vtkSuperquadricTensorGlyph : public vtkTensorGlyph
{
public:
  vtkTypeMacro(vtkSuperquadricTensorGlyph,vtkTensorGlyph);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Construct object with  defaults from superclass:
  // scaling on and scale factor 1.0. Eigenvalues are 
  // extracted, glyphs are colored with calculated features
  // and logarithmic scaling is turned off.
  static vtkSuperquadricTensorGlyph *New();

  // Description
  // Input scalars are a binary mask: 0 prevents display
  // of polydata at that point
  vtkBooleanMacro(MaskGlyphsWithScalars, int);
  vtkSetMacro(MaskGlyphsWithScalars, int);
  vtkGetMacro(MaskGlyphsWithScalars, int);


  vtkSetObjectMacro(ScalarMask, vtkImageData);

#define VTK_LINEAR_MEASURE 1
#define VTK_SPHERICAL_MEASURE 2
#define VTK_PLANAR_MEASURE 3
#define VTK_MAX_EIGENVAL_MEASURE 4
#define VTK_MIDDLE_EIGENVAL_MEASURE 5
#define VTK_MIN_EIGENVAL_MEASURE 6
#define VTK_EIGENVAL_DIFFERENCE_MAX_MID_MEASURE 7
#define VTK_DIRECTION_MEASURE 8
#define VTK_RELATIVE_ANISOTROPY_MEASURE  9
#define VTK_FRACTIONAL_ANISOTROPY_MEASURE  10

  void ColorGlyphsWithLinearMeasure();
  void ColorGlyphsWithSphericalMeasure();
  void ColorGlyphsWithPlanarMeasure();
  void ColorGlyphsWithMaxEigenvalue();
  void ColorGlyphsWithMiddleEigenvalue();
  void ColorGlyphsWithMinEigenvalue();
  void ColorGlyphsWithMaxMinusMidEigenvalue();
  void ColorGlyphsWithDirection();
  void ColorGlyphsWithRelativeAnisotropy();
  void ColorGlyphsWithFractionalAnisotropy();

  // Description
  // Transform output glyph locations (not orientations!) 
  // by this matrix.
  //
  // Example usage is as follows:
  // 1) Reformat a slice through a tensor volume.
  // 2) Set VolumePositionMatrix to the reformat matrix.
  //    This is analogous to setting the actor's UserMatrix
  //    to this matrix, which only works for scalar data.
  // 3) The output glyphs are positioned correctly without
  //    incorrectly rotating the tensors, as would be the 
  //    case if positioning the scene's actor with this matrix.
  // 
  vtkSetObjectMacro(VolumePositionMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(VolumePositionMatrix, vtkMatrix4x4);


  // Description
  // Transform output glyph orientations
  // by this matrix.
  //
  // Example usage is as follows:
  // 1) If tensors are to be displayed in a coordinate system
  //    that is not IJK (array-based), and the whole volume is
  //    being rotated, each tensor needs also to be rotated.
  //    First find the matrix that positions your volume.
  //    This is how the entire volume is positioned, not 
  //    the matrix that positions an arbitrary reformatted slice.
  // 2) Remove scaling and translation from this matrix; we
  //    just need to rotate each tensor.
  // 3) Set TensorRotationMatrix to this rotation matrix.
  //
  vtkSetObjectMacro(TensorRotationMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(TensorRotationMatrix, vtkMatrix4x4);

  // Description:
  // Resolution of the output glyphs. This parameter is a integer value
  // that set the number of points that are skipped before render one glyphs.
  // 1 is the finer level meaning that every input point a glyph is rendered.
  void SetResolution(int value){
    if(value<=0) {
      vtkWarningMacro("Resolution cannot be lower than 1.");
      value = 1;
    }
    this->Resolution=value;  
    this->Modified();
  };  
  vtkGetMacro(Resolution,int);


  vtkSetMacro(Gamma,double);
  vtkGetMacro(Gamma,double);
  
  vtkSetMacro(ThetaResolution,int);
  vtkGetMacro(ThetaResolution,int);
  vtkSetMacro(PhiResolution,int);
  vtkGetMacro(PhiResolution,int);
  
  // Description:
  // When determining the modified time of the filter, 
  // this checks the modified time of the mask input,
  // if it exists.
  unsigned long int GetMTime();

protected:
  vtkSuperquadricTensorGlyph();
  ~vtkSuperquadricTensorGlyph();
  vtkSuperquadricTensorGlyph(const vtkSuperquadricTensorGlyph&) {};
  void operator=(const vtkSuperquadricTensorGlyph&) {};

  void Execute();

  void ColorGlyphsWith(int measure);
  int ColorGlyphsWithAnisotropy;
  int ScalarMeasure;
  int MaskGlyphsWithScalars;
  int Resolution;
  
  double Gamma;
  int ThetaResolution;
  int PhiResolution;

  vtkMatrix4x4 *VolumePositionMatrix;
  vtkMatrix4x4 *TensorRotationMatrix;

  vtkImageData *ScalarMask;
};

#endif
