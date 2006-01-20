/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkHyperStreamlineDTMRI.h,v $
  Date:      $Date: 2006/01/20 03:05:02 $
  Version:   $Revision: 1.8 $

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


  // If StoppingMode criterion becomes smaller than this number, 
  // tracking stops.       
  vtkGetMacro(StoppingThreshold,vtkFloatingPointType);
  vtkSetMacro(StoppingThreshold,vtkFloatingPointType);

protected:
  vtkHyperStreamlineDTMRI();
  ~vtkHyperStreamlineDTMRI();

  // Integrate data
  void Execute();
  void BuildLines();

  vtkFloatingPointType MaxCurvature;
  int StoppingMode;
  vtkFloatingPointType StoppingThreshold;

private:
  vtkHyperStreamlineDTMRI(const vtkHyperStreamlineDTMRI&);  // Not implemented.
  void operator=(const vtkHyperStreamlineDTMRI&);  // Not implemented.
};

#endif


