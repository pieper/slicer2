// .NAME vtkImageDiffusionTensor - 
// .SECTION Description
// Creates a vtkCollection of vtkHyperStreamlines.

#ifndef __vtkMultipleStreamlineController_h
#define __vtkMultipleStreamlineController_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkCollection.h"
#include "vtkTransform.h"
#include "vtkHyperStreamline.h"
#include "vtkHyperStreamlinePoints.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkLookupTable.h"


class VTK_DTMRI_EXPORT vtkMultipleStreamlineController : public vtkObject
{
 public:
  static vtkMultipleStreamlineController *New();
  vtkTypeMacro(vtkMultipleStreamlineController,vtkObject);

  // Description
  // Start a streamline from each voxel which has the value InputROIValue
  // in the InputROI volume.  Streamlines are added to the vtkCollection
  // this->Streamlines.
  void SeedStreamlinesFromROI();

  // Description
  // Start a streamline from the input point.
  // The point should be in the world coordinates of the scene.
  void SeedStreamlineFromPoint(double x, double y, double z);

  // Description
  // Make all of the streamlines visible in the renderer.
  void AddStreamlinesToScene();

  // Description
  // Hide all streamlines (turn off their visibility);
  void RemoveStreamlinesFromScene();

  // Description
  // Delete one streamline.  The input is a pointer to the actor you
  // wish to delete.  All associated objects are deleted and removed 
  // from the collections.
  void DeleteStreamline(vtkActor *pickedActor);

  // Description
  // Delete all streamlines
  void DeleteAllStreamlines();

  // Description
  // Input tensor field in which to seed streamlines
  vtkSetObjectMacro(InputTensorField, vtkImageData);
  vtkGetObjectMacro(InputTensorField, vtkImageData);

  // Description
  // Streamlines will be started at locations with this value in InputROI
  vtkSetMacro(InputROIValue, int);
  vtkGetMacro(InputROIValue, int);

  // Description
  // Input ROI volume describing where to start streamlines
  vtkSetObjectMacro(InputROI, vtkImageData);
  vtkGetObjectMacro(InputROI, vtkImageData);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform the ijk coordinates of the ROI to world coordinates.
  vtkSetObjectMacro(ROIToWorld, vtkTransform);
  vtkGetObjectMacro(ROIToWorld, vtkTransform);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform world coordinates into scaled ijk of the tensor field.
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

  // Description
  // List of the output vtkHyperStreamlines (or subclasses)
  vtkGetObjectMacro(Streamlines, vtkCollection);
  vtkGetObjectMacro(Actors, vtkCollection);
  //vtkGetObjectMacro(LookupTables, vtkCollection);
  vtkGetObjectMacro(Mappers, vtkCollection);

  // Description
  // Input: list of the renderers whose scenes will have streamlines
  // added.
  vtkSetObjectMacro(InputRenderers, vtkCollection);
  vtkGetObjectMacro(InputRenderers, vtkCollection);


  // Description
  // Control actor properties of created streamlines by setting
  // them in this vtkProperty object.  Its parameters are copied
  // into the streamline actors.
  vtkSetObjectMacro(StreamlineProperty, vtkProperty);
  vtkGetObjectMacro(StreamlineProperty, vtkProperty);

  // Description
  // controls scalar visibility of actors created in this class
  vtkSetMacro(ScalarVisibility,int);
  vtkGetMacro(ScalarVisibility,int);
  vtkBooleanMacro(ScalarVisibility,int);

  // Description
  //  lookup table  for displayed streamlines
  vtkSetObjectMacro(StreamlineLookupTable, vtkLookupTable);
  vtkGetObjectMacro(StreamlineLookupTable, vtkLookupTable);

  // Description
  // To do list:
  // Add access to parameters of all created streamlines.
  // Add Print function
  // Add option to create new/old streamline classes
  // Add save function
  // Add observers for progress/implement progress updating
  // Add check on all inputs

 protected:
  vtkMultipleStreamlineController();
  ~vtkMultipleStreamlineController();

  void CreateGraphicsObjects();
  void ApplyUserSettingsToGraphicsObject(int index);
  void DeleteStreamline(int index);
  int PointWithinTensorData(double *point, double *pointw);

  vtkTransform *ROIToWorld;
  vtkTransform *WorldToTensorScaledIJK;

  vtkImageData *InputTensorField;
  vtkImageData *InputROI;
  vtkCollection *InputRenderers;
  int InputROIValue;

  vtkCollection *Streamlines;
  //vtkCollection *LookupTables;
  vtkCollection *Mappers;
  vtkCollection *Actors;

  vtkProperty *StreamlineProperty;

  int ScalarVisibility;
  vtkLookupTable *StreamlineLookupTable;

  // Add Parameters of standard streamlines
  //MaximumPropagationDistance IntegrationStepLength 
  //StepLength Radius  NumberOfSides IntegrationDirection
  int IntegrationDirection;

  // Add parameters of precise streamlines

  // Or perhaps add a representative accessible object 
  // of each type, that the user can
  // modify, then copy it to each new created streamline.

};

#endif
