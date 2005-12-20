/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSeedTracts.h,v $
  Date:      $Date: 2005/12/20 22:55:08 $
  Version:   $Revision: 1.5.2.1 $

=========================================================================auto=*/
// .NAME vtkSeedTracts - 
// .SECTION Description
//
// Individual streamlines can be started at a point, or 
// many can be started inside a region of interest.
//

#ifndef __vtkSeedTracts_h
#define __vtkSeedTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkCollection.h"
#include "vtkShortArray.h"

#include "vtkHyperStreamline.h"
#include "vtkHyperStreamlinePoints.h"
#include "vtkPreciseHyperStreamlinePoints.h"

#define USE_VTK_HYPERSTREAMLINE 0
#define USE_VTK_HYPERSTREAMLINE_POINTS 1
#define USE_VTK_PRECISE_HYPERSTREAMLINE_POINTS 2

class VTK_DTMRI_EXPORT vtkSeedTracts : public vtkObject
{
 public:
  static vtkSeedTracts *New();
  vtkTypeMacro(vtkSeedTracts,vtkObject);

  // Description
  // Start a streamline from each voxel which has the value InputROIValue
  // in the InputROI volume.  Streamlines are added to the vtkCollection
  // this->Streamlines.
  void SeedStreamlinesInROI();
  
  // Description
  // Start a streamline from each voxel which has the values stored in
  // the vtkShortArray InputMultipleROIValues
  // in the InputROI volume.  Streamlines are added to the vtkCollection
  // this->Streamlines.
  void SeedStreamlinesInROIWithMultipleValues();

  // Description
  // Start a streamline from each voxel in ROI, keep those paths
  // that pass through ROI2.
  void SeedStreamlinesFromROIIntersectWithROI2();

  // Description
  // Seed each streamline, cause it to Update, save its info to disk
  // and then Delete it.  This is a way to seed in the whole brain
  // without running out of memory. Nothing is displayed in the renderers.
  // Both the models and the lines of points (text files) are saved.
  void SeedAndSaveStreamlinesInROI(char *pointsFilename, char *modelFilename);

  // Description
  // Start a streamline from the input point.
  // The point should be in the world coordinates of the scene.
  void SeedStreamlineFromPoint(double x, double y, double z);

  // Description
  // Input tensor field in which to seed streamlines
  vtkSetObjectMacro(InputTensorField, vtkImageData);
  vtkGetObjectMacro(InputTensorField, vtkImageData);

  // Description
  // Streamlines will be started at locations with this value in the InputROI.
  // The value must be greater than 0. A 0 value is not allowed because it
  // would allow users to accidentally start streamlines outside of their
  // ROI.
  vtkSetClampMacro(InputROIValue, int, 1, VTK_SHORT_MAX);
  vtkGetMacro(InputROIValue, int);
  vtkSetClampMacro(InputROI2Value, int, 1, VTK_SHORT_MAX);
  vtkGetMacro(InputROI2Value, int);

  // Description
  // Streamlines will be started at locations with these values in the InputROI.
  // The value must be greater than 0. A 0 value is not allowed because it
  // would allow users to accidentally start streamlines outside of their
  // ROI.
  vtkSetObjectMacro(InputMultipleROIValues,vtkShortArray);
  vtkGetObjectMacro(InputMultipleROIValues,vtkShortArray);

  // Description
  // Input ROI volume describing where to start streamlines
  vtkSetObjectMacro(InputROI, vtkImageData);
  vtkGetObjectMacro(InputROI, vtkImageData);

  // Description
  // Input ROI volume to select streamlines (those that begin
  // within InputROI and pass through InputROI2
  // will be displayed).
  vtkSetObjectMacro(InputROI2, vtkImageData);
  vtkGetObjectMacro(InputROI2, vtkImageData);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform the ijk coordinates of the ROI to world coordinates.
  vtkSetObjectMacro(ROIToWorld, vtkTransform);
  vtkGetObjectMacro(ROIToWorld, vtkTransform);

  // Description
  // Transformation used in seeding streamlines.  This is for the 
  // second ROI volume.
  vtkSetObjectMacro(ROI2ToWorld, vtkTransform);
  vtkGetObjectMacro(ROI2ToWorld, vtkTransform);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform world coordinates into scaled ijk of the tensor field.
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

  // Description
  // List of the output vtkHyperStreamlines (or subclasses)
  vtkSetObjectMacro(Streamlines, vtkCollection);
  vtkGetObjectMacro(Streamlines, vtkCollection);

  // Description
  // Type of vtkHyperStreamline subclass to create.
  // Use standard VTK class.
  void UseVtkHyperStreamline()
    {
      this->TypeOfHyperStreamline=USE_VTK_HYPERSTREAMLINE;
    }

  // Description
  // Type of vtkHyperStreamline subclass to create.
  // Use our subclass that returns points on the streamline.
  void UseVtkHyperStreamlinePoints()
    {
      this->TypeOfHyperStreamline=USE_VTK_HYPERSTREAMLINE_POINTS;
    }

  // Description
  // Type of vtkHyperStreamline subclass to create.
  // Use our subclass that returns points on the streamline
  // and interpolates using BSplines.
  void UseVtkPreciseHyperStreamlinePoints()
    {
      this->TypeOfHyperStreamline=USE_VTK_PRECISE_HYPERSTREAMLINE_POINTS;
    }

  // Description
  // Example objects whose settings will be used in creation
  // of vtkHyperStreamline subclasses of that type.
  // This is an alternative to duplicating the parameters of 
  // these classes as parameters of this class.
  vtkSetObjectMacro(VtkHyperStreamlinePointsSettings,vtkHyperStreamlinePoints);
  vtkGetObjectMacro(VtkHyperStreamlinePointsSettings,vtkHyperStreamlinePoints);
  vtkSetObjectMacro(VtkPreciseHyperStreamlinePointsSettings,
                    vtkPreciseHyperStreamlinePoints);
  vtkGetObjectMacro(VtkPreciseHyperStreamlinePointsSettings,
                    vtkPreciseHyperStreamlinePoints);

 // Description
 // Update all tracts with current parameters.
 // Loop through all of the hyperstreamline objects and set their
 // parameters according to the current vtkHyperStreamline*Settings object
 // which the user can modify. 
 void UpdateAllHyperStreamlineSettings();

 protected:
  vtkSeedTracts();
  ~vtkSeedTracts();

  void SaveStreamlineAsTextFile(ofstream &filePoints,
                                vtkHyperStreamlinePoints *currStreamline);

  vtkHyperStreamline *CreateHyperStreamline();

  vtkCollection *Streamlines;

  vtkTransform *ROIToWorld;
  vtkTransform *ROI2ToWorld;
  vtkTransform *WorldToTensorScaledIJK;

  vtkImageData *InputTensorField;
  vtkImageData *InputROI;
  vtkImageData *InputROI2;

  int InputROIValue;
  int InputROI2Value;
  vtkShortArray *InputMultipleROIValues;
  
  int PointWithinTensorData(double *point, double *pointw);
  
  int TypeOfHyperStreamline;

  // Here we have a representative accessible object 
  // of each type, so that the user can modify it.
  // We copy its settings to each new created streamline.
  vtkHyperStreamline *VtkHyperStreamlineSettings;
  vtkHyperStreamlinePoints *VtkHyperStreamlinePointsSettings;
  vtkPreciseHyperStreamlinePoints *VtkPreciseHyperStreamlinePointsSettings;

  // Since only integrating both directions makes sense in the application,
  // this is the default of this class. This prevents the objects above
  // from changing the integration direction.
  int IntegrationDirection;


  void UpdateHyperStreamlinePointsSettings( vtkHyperStreamlinePoints *currHSP);

};

#endif
