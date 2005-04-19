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
#ifndef __vtkODFGlyph_h
#define __vtkODFGlyph_h

#include "vtkDTMRIConfigure.h"
#include <vtkStructuredPointsToPolyDataFilter.h>
#include "vtkImageReformat.h"
#include "vtkImageExtractComponents.h"
#include <vtkMatrix4x4.h>

class vtkLookupTable;
class vtkPolyData;



class VTK_DTMRI_EXPORT vtkODFGlyph : public vtkStructuredPointsToPolyDataFilter {

 public:
  vtkTypeMacro(vtkODFGlyph,vtkStructuredPointsToPolyDataFilter);
  //void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Construct object
  static vtkODFGlyph *New();

  // Description:
  // Get/Set factor by which to scale each odf
  vtkSetMacro(ScaleFactor,float);
  vtkGetMacro(ScaleFactor,float);
  
  vtkSetMacro(MinODF,double);
  vtkGetMacro(MinODF,double);
  
  vtkSetMacro(MaxODF,double);
  vtkGetMacro(MaxODF,double);


  // Description:
  // Get/Set vtkLookupTable which holds color values for current output
  //vtkSetObjectMacro(ColorTable,vtkLookupTable);
  vtkGetObjectMacro(ColorTable,vtkLookupTable);

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

  //Description:
  //WldToIjkMatrix: matrix that position the volume in the RAS system
  vtkSetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(WldToIjkMatrix, vtkMatrix4x4);
  
  // Methods to set up the reformatter
  //Description:
  // Field of View
  vtkSetMacro(FieldOfView,int);
  vtkGetMacro(FieldOfView,int);

  // Description:
  // When determining the modified time of the filter, 
  // this checks the modified time of the mask input,
  // if it exists.
  unsigned long int GetMTime();

protected:
  vtkODFGlyph();
  ~vtkODFGlyph();
  vtkODFGlyph(const vtkODFGlyph&) {};  // Not implemented.
  void operator=(const vtkODFGlyph&) {};  // Not implemented.

  void Execute();

  float ScaleFactor; // Factor by which to scale each odf
  int FieldOfView;
  double MinODF;
  double MaxODF;

  int BrightnessLevels; // # of sets of NUM_SPHERE_POINTS values in ColorTable. Each set at a different brightness gradation.
  vtkLookupTable *ColorTable; // color table for current output. indeces match
                              // scalars of output's pointdata
                  
  vtkMatrix4x4 *VolumePositionMatrix;
  vtkMatrix4x4 *TensorRotationMatrix;
  vtkMatrix4x4 *WldToIjkMatrix;                  
  
  vtkImageExtractComponents **ImageExtract;
  vtkImageReformat **ImageReformat;
  int NumberOfInputComponents;
  
private:
//BTX
  static const int ODF_SIZE = 752;
  static const int NUM_SPHERE_POINTS = ODF_SIZE;
  static const double SPHERE_POINTS[NUM_SPHERE_POINTS][3];
//ETX
};

#endif
