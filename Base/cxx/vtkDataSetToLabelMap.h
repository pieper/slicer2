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
/* author: Delphine Nain, delfin@ai.mit.edu
 */

// .NAME vtkDataSetToLabelMap - convert arbitrary vtkDataSet to a voxel representation with the following encoding (0 = outside voxel, 1 = surface voxel, 2 = inside voxel) 
// .SECTION Description
// author: Delphine Nain, delfin@ai.mit.edu
// vtkDataSetToLabelMap is a filter that converts an arbitrary data set to a
// structured point (i.e., voxel) representation with the following encoding (0 = outside voxel, 1 = surface voxel, 2 = inside voxel). The output Image has the dimensions and origin of the bounding box of the input DataSet. 
// .SECTION see also
// vtkImplicitModeller


#ifndef __vtkDataSetToLabelMap_h

#define __vtkDataSetToLabelMap_h

#include "vtkDataSetToStructuredPointsFilter.h"
#include "vtkShortArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkImageLogic.h"
#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

class VTK_SLICER_BASE_EXPORT vtkDataSetToLabelMap : public vtkDataSetToStructuredPointsFilter
{

public:
  static vtkDataSetToLabelMap *New();
  vtkTypeMacro(vtkDataSetToLabelMap,vtkDataSetToStructuredPointsFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Construct an instance of vtkDataSetToLabelMap with its sample dimensions
  // set to (50,50,50), and so that the model bounds are
  // automatically computed from its input. The maximum distance is set to 
  // examine the whole grid. This could be made much faster, and probably
  // will be in the future.
     
  // Description:
  // Compute the ModelBounds based on the input geometry.


void ComputeOutputParameters();
 
// Description:
// Set the i-j-k dimensions on which to sample the distance function.
 
void SetOutputSpacing(vtkFloatingPointType i, vtkFloatingPointType j, vtkFloatingPointType k);

  // Description:
  // Set the i-j-k dimensions on which to sample the distance function.

void SetOutputSpacing(vtkFloatingPointType dim[3]);

vtkGetVectorMacro(OutputDimensions,int,3);
vtkGetVectorMacro(OutputSpacing,vtkFloatingPointType,3);
vtkGetVectorMacro(OutputOrigin,vtkFloatingPointType,3);

 vtkSetMacro(UseBoundaryVoxels,int);
 vtkGetMacro(UseBoundaryVoxels,int);
 
vtkGetObjectMacro(InOutScalars,vtkShortArray);
vtkGetObjectMacro(BoundaryScalars,vtkShortArray);


  // Description:
  // Write the volume out to a specified filename.

  void Write(char *);

protected:

  vtkDataSetToLabelMap();

  ~vtkDataSetToLabelMap();

  vtkDataSetToLabelMap(const vtkDataSetToLabelMap&) {};

  void operator=(const vtkDataSetToLabelMap&) {};


  
  void Execute();
  int IsPointInside(vtkFloatingPointType s, vtkFloatingPointType t);
  vtkFloatingPointType ComputeStep(vtkFloatingPointType spacing[3],vtkFloatingPointType vertex0[3],vtkFloatingPointType vertex1[3]);
  void EvaluatePoint(vtkFloatingPointType vo[3], vtkFloatingPointType v1[3], vtkFloatingPointType v2[3], vtkFloatingPointType s, vtkFloatingPointType t,vtkFloatingPointType result[3]);
  void BoundaryFill(int i, int j, int k, vtkShortArray *scalars);
  
  vtkFloatingPointType OutputOrigin[3];
  int OutputDimensions[3];
  vtkFloatingPointType OutputSpacing[3];

  vtkShortArray *InOutScalars;
  vtkShortArray *BoundaryScalars;
  
  int UseBoundaryVoxels;
};



#endif


