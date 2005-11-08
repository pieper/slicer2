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
// .NAME vtkSaveTracts - 
// .SECTION Description
// Handles save functionality for objects representing tracts.
// Takes as input collections of streamline objects.
// Can use SaveForAnalysis mode (saves tensors at each point
// of the input polydata, for this an InputTensorField is needed).
// Otherwise just saves the input polydata.
// Input is grouped by color (according to color from input actor
// collection) and each color is saved as a separate polydata file.

#ifndef __vtkSaveTracts_h
#define __vtkSaveTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkCollection.h"
#include "vtkTransform.h"
#include "vtkMrmlTree.h"
#include "vtkImageData.h"



class VTK_DTMRI_EXPORT vtkSaveTracts : public vtkObject
{
 public:
  static vtkSaveTracts *New();
  vtkTypeMacro(vtkSaveTracts,vtkObject);

  // Description
  // Save streamlines as vtkPolyData models.
  // Streamlines are grouped into model files based on their color.
  // Files are saved as filename_0.vtk, filename_1.vtk, etc.
  // A MRML file is saved as filename.xml.
  // The MRML model names are name_0, name_1, etc.
  void SaveStreamlinesAsPolyData(char *filename, char *name);

  // Description
  // Save streamlines as vtkPolyData models.
  // Streamlines are grouped into model files based on their color.
  // Files are saved as filename_0.vtk, filename_1.vtk, etc.
  // A MRML file is saved as filename.xml.
  // The MRML model names are name_0, name_1, etc.  
  // The optional colorTree argument lets us find and save in MRML
  // the text names of the colors of each streamline.
  void SaveStreamlinesAsPolyData(char *filename, char *name, 
                                 vtkMrmlTree *colorTree);

  // Description
  // Input to this class (things we may save)
  vtkSetObjectMacro(Streamlines, vtkCollection);

  // Description
  // Input to this class (things we may save)
  vtkSetObjectMacro(TubeFilters, vtkCollection);

  // Description
  // Input to this class (to save tensors along tract path)
  vtkSetObjectMacro(InputTensorField, vtkImageData);

  // Description
  // Input to this class (for grabbing colors). This 
  // may change to a colorID array.
  vtkSetObjectMacro(Actors, vtkCollection);

  // Example usage is as follows:
  // 1) If tensors are to be saved in a coordinate system
  //    that is not IJK (array-based), and the whole volume is
  //    being rotated, each tensor needs also to be rotated.
  //    First find the matrix that positions your tensor volume.
  //    This is how the entire volume is positioned, not 
  //    the matrix that positions an arbitrary reformatted slice.
  // 2) Remove scaling and translation from this matrix; we
  //    just need to rotate each tensor.
  // 3) Set TensorRotationMatrix to this rotation matrix.
  //
  vtkSetObjectMacro(TensorRotationMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(TensorRotationMatrix, vtkMatrix4x4);

  // Description
  // Transformation used to place streamlines in scene 
  // (actually inverse of this transform). Needed to save
  // paths in world coordinates.
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

  // Description
  // Coordinate system in which to save tracts.
  void SetOutputCoordinateSystemToWorld(){
   this->SetOutputCoordinateSystem(1);}

  // Description
  // Coordinate system in which to save tracts.
  // This is IJK (array) coordinates with voxel scaling.
  void SetOutputCoordinateSystemToScaledIJK(){
   this->SetOutputCoordinateSystem(2);}

  // Description
  // Coordinate system in which to save tracts.
  // This is IJK (array) coordinates with voxel scaling,
  // and the center of the original tensor volume is at the origin.
  // This is useful when the tensor image data has been registered
  // in this coordinate system.
  void SetOutputCoordinateSystemToCenteredScaledIJK(){
   this->SetOutputCoordinateSystem(3);}

  // Description
  // Coordinate system in which to save tracts.
  vtkSetMacro(OutputCoordinateSystem, int);
  vtkGetMacro(OutputCoordinateSystem, int);

  // Description
  // Used to "center" the scaled IJK coords. This is 
  // used when the coordinate system is CenteredScaledIJK.
  // This is the extent of the original tensor volume.
  vtkSetVector6Macro(ExtentForCenteredScaledIJK, int);
  vtkGetVector6Macro(ExtentForCenteredScaledIJK, int);

  // Description
  // Used to "center" the scaled IJK coords. This is 
  // used when the coordinate system is CenteredScaledIJK.
  // This is the voxel scaling (size in mm) of the original tensor volume.
  vtkSetVector3Macro(ScalingForCenteredScaledIJK, float);
  vtkGetVector3Macro(ScalingForCenteredScaledIJK, float);

  // Description
  // Save for Analysis == 1 means save polylines with tensors.
  // Otherwise save tube vtk polydata models.
  vtkSetMacro(SaveForAnalysis,int);
  vtkGetMacro(SaveForAnalysis,int);
  vtkBooleanMacro(SaveForAnalysis,int);

 protected:
  vtkSaveTracts();
  ~vtkSaveTracts();

  vtkImageData *InputTensorField;

  vtkTransform *WorldToTensorScaledIJK;
  vtkMatrix4x4 *TensorRotationMatrix;

  vtkCollection *Streamlines;
  vtkCollection *TubeFilters;
  vtkCollection *Actors;


  int SaveForAnalysis;
  int OutputCoordinateSystem;
  int ExtentForCenteredScaledIJK[6];
  float ScalingForCenteredScaledIJK[3];
};


#endif
