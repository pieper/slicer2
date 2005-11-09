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
// .NAME vtkHyperStreamlineDTMRI - generate hyperstreamline in arbitrary dataset
// .SECTION Description
// vtkHyperStreamlineDTMRI is a filter that integrates through a tensor field to 
// generate a hyperstreamline. The integration is along the maximum eigenvector
// and the cross section of the hyperstreamline is defined by the two other
// eigenvectors. Thus the shape of the hyperstreamline is "tube-like", with 
// the cross section being elliptical. Hyperstreamlines are used to visualize
// tensor fields.
//
// The starting point of a hyperstreamline can be defined in one of two ways. 
// First, you may specify an initial position. This is a x-y-z global 
// coordinate. The second option is to specify a starting location. This is 
// cellId, subId, and  cell parametric coordinates.
//
// The integration of the hyperstreamline occurs through the major eigenvector 
// field. IntegrationStepLength controls the step length within each cell 
// (i.e., this is the fraction of the cell length). The length of the 
// hyperstreamline is controlled by MaximumPropagationDistance. This parameter
// is the length of the hyperstreamline in units of distance. The tube itself 
// is composed of many small sub-tubes - NumberOfSides controls the number of 
// sides in the tube, and StepLength controls the length of the sub-tubes.
//
// Because hyperstreamlines are often created near regions of singularities, it
// is possible to control the scaling of the tube cross section by using a 
// logarithmic scale. Use LogScalingOn to turn this capability on. The Radius 
// value controls the initial radius of the tube.

// .SECTION See Also
// vtkTensorGlyph vtkStreamer

#ifndef __vtkHyperStreamlineDTMRI_h
#define __vtkHyperStreamlineDTMRI_h

#include "vtkTensorUtilConfigure.h"
#include "vtkHyperStreamline.h"
#include "vtkHyperPointandArray.h"
#include "vtkTensorMathematics.h"
#include "vtkFloatArray.h"

//class VTK_DTMRI_EXPORT vtkHyperStreamlineDTMRI : public vtkHyperStreamline 
class VTK_TENSORUTIL_EXPORT vtkHyperStreamlineDTMRI : public vtkHyperStreamline 
{
public:
  vtkTypeRevisionMacro(vtkHyperStreamlineDTMRI,vtkHyperStreamline);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with initial starting position (0,0,0); integration
  // step length 0.2; step length 0.01; forward integration; terminal
  // eigenvalue 0.0; number of sides 6; radius 0.5; and logarithmic scaling
  // off.
  static vtkHyperStreamlineDTMRI *New();

  // Description:
  // If degree curvature becomes larger than this number, tracking stops.
  vtkGetMacro(MaxCurvature,vtkFloatingPointType);
  vtkSetMacro(MaxCurvature,vtkFloatingPointType);


  vtkGetMacro(StoppingMode,int);
  vtkSetMacro(StoppingMode,int);
  void SetStoppingModeToFractionalAnisotropy() 
    {this->SetStoppingMode(VTK_TENS_FRACTIONAL_ANISOTROPY);};
  void SetStoppingModeToLinearMeasure() 
    {this->SetStoppingMode(VTK_TENS_LINEAR_MEASURE);};
  void SetStoppingModeToPlanarMeasure() 
    {this->SetStoppingMode(VTK_TENS_PLANAR_MEASURE);};
  void SetStoppingModeToSphericalMeasure() 
    {this->SetStoppingMode(VTK_TENS_SPHERICAL_MEASURE);};


  // If FA becomes smaller than this number, tracking stops.       
  vtkGetMacro(StoppingThreshold,vtkFloatingPointType);
  vtkSetMacro(StoppingThreshold,vtkFloatingPointType);

  // Description:                                                             
  // FA at points along the line (wherever the tensor was interpolated)    
  vtkGetObjectMacro(FractionalAnisotropy0,vtkFloatArray);
  vtkGetObjectMacro(FractionalAnisotropy1,vtkFloatArray);

protected:
  vtkHyperStreamlineDTMRI();
  ~vtkHyperStreamlineDTMRI();

  // Integrate data
  void Execute();
  void BuildLines();

  vtkFloatingPointType MaxCurvature;
  int StoppingMode;
  vtkFloatingPointType StoppingThreshold;

  vtkFloatArray *FractionalAnisotropy[2];
  vtkFloatArray *FractionalAnisotropy0;
  vtkFloatArray *FractionalAnisotropy1;

private:
  vtkHyperStreamlineDTMRI(const vtkHyperStreamlineDTMRI&);  // Not implemented.
  void operator=(const vtkHyperStreamlineDTMRI&);  // Not implemented.
};

#endif


