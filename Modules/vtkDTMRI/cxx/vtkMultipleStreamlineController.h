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
// .NAME vtkMultipleStreamlineController - 
// .SECTION Description
// Creates and manages a vtkCollection of vtkHyperStreamlines.
//
// Individual streamlines can be started at a point, or 
// many can be started inside a region of interest.
// Subclasses of vtkHyperStreamline may be created instead
// of the default vtkHyperStreamline class.
// This class also creates collections of mappers and actors
// for the streamlines, and can control their visibility in the scene.
//

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
#include "vtkPreciseHyperStreamlinePoints.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkLookupTable.h"
#include "vtkMrmlTree.h"
#include "vtkShortArray.h"
#include "vtkDoubleArray.h"
#include "vtkPolyData.h"
#include "vtkIntArray.h"
#include "vtkClusterTracts.h"


#define USE_VTK_HYPERSTREAMLINE 0
#define USE_VTK_HYPERSTREAMLINE_POINTS 1
#define USE_VTK_PRECISE_HYPERSTREAMLINE_POINTS 2

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
  // Start a streamline from each voxel which has the values store in
  // the vtkShortArray InputMultipleROIValues
  // in the InputROI volume.  Streamlines are added to the vtkCollection
  // this->Streamlines.
  void SeedStreamlinesFromROIWithMultipleValues();

  // Description
  // Start a streamline from each voxel in ROI, keep those paths
  // that pass through ROIForIntersection.
  void SeedStreamlinesFromROIIntersectWithROI2();

  // Description
  // Seed each streamline, cause it to Update, save its info to disk
  // and then Delete it.  This is a way to seed in the whole brain
  // without running out of memory. Nothing is displayed in the renderers.
  // Both the models and the lines of points (text files) are saved.
  void SeedAndSaveStreamlinesFromROI(char *pointsFilename, char *modelFilename);
  // Description
  // Seed streamlines with a certain density within the ROI.
  // The ROI here is a mask, of the white matter for example, and the output
  // is a collection of streamlines that are approximately evenly spaced
  // in the mask.  During seeding in the mask, each streamline's path is 
  // marked ("colored in") in a volume with the same dimensions as the mask.
  // If the next voxel to seed already has been traversed by a user-specified
  // number of streamlines, seeding does not take place there.  The point
  // is to limit the number of streamlines while providing reasonably
  // complete coverage of anatomy.
  void SeedStreamlinesEvenlyInROI();


  // Description
  // One-shot function which clusters paths from ROI and displays using
  // one actor per color. After, streamlines can't be edited.  To
  // allow a high number of tracts without overhead of one actor per tract.
  void SeedStreamlinesFromROIClusterAndDisplay();

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
  // Get the internal index of the chosen actor, if it is a streamline
  // in the collection.
  int GetStreamlineIndexFromActor(vtkActor *pickedActor);

  // Description
  // Save streamlines as vtkPolyData models.
  // Streamlines are grouped into model files based on their color.
  // Files are saved as filename_0.vtk, filename_1.vtk, etc.
  // A MRML file is saved as filename.xml.
  // The MRML model names are name_0, name_1, etc.
  void SaveStreamlinesAsPolyData(char *filename, char *name);

  // Description
  // Save streamlines as text files (one streamline per file).
  // This can be replaced by a better file format.
  void SaveStreamlinesAsTextFiles(char *filename);

  // Description
  // Save all streamlines in one text file, save FA in another, and save
  // the class labels in a third.  Temporary function to write
  // matlab-readable data from the slicer.
  // This should be replaced by a better file format.
  void SaveTractClustersAsTextFiles(char *filename);

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
  
  
  //Description
  // Find the streamlines that pass through the set of ROI values
  // stored in InputMultipleROIValues. This operation is performed
  // by convolving  the streamline with the kernel ConvolutionKernel.
  void FindStreamlinesThatPassThroughROI();
  
  //Description
  // Convert Streamline from Points representation to PolyLines
  void ConvertStreamlinesToPolyLines();
  
  //Description
  // Get Streamlines as Polylines
  vtkGetObjectMacro(StreamlinesAsPolyLines,vtkPolyData);
  
  void HighlightStreamlinesPassTest();
  
  void DeleteStreamlinesNotPassTest();
  
  
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
  // within InputROI and pass through InputROIForIntersection
  // will be displayed).
  vtkSetObjectMacro(InputROIForIntersection, vtkImageData);
  vtkGetObjectMacro(InputROIForIntersection, vtkImageData);

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
  vtkGetObjectMacro(Mappers, vtkCollection);
  int GetNumberOfStreamlines() {return this->Streamlines->GetNumberOfItems();}

  // Description
  // Input: list of the renderers whose scenes will have streamlines
  // added.
  vtkSetObjectMacro(InputRenderers, vtkCollection);
  vtkGetObjectMacro(InputRenderers, vtkCollection);

  // Description
  // Convolution Kernel that is used to convolved the fiber with when
  // finding the ROIs that the fiber pass through
  vtkSetObjectMacro(ConvolutionKernel, vtkDoubleArray);
  vtkGetObjectMacro(ConvolutionKernel, vtkDoubleArray);

  // Description
  // Control actor properties of created streamlines by setting
  // them in this vtkProperty object.  Its parameters are copied
  // into the streamline actors.
  vtkSetObjectMacro(StreamlineProperty, vtkProperty);
  vtkGetObjectMacro(StreamlineProperty, vtkProperty);

  // Description
  // controls scalar visibility of actors created in this class
  void SetScalarVisibility(int);
  vtkGetMacro(ScalarVisibility,int);
  vtkBooleanMacro(ScalarVisibility,int);

  // Description
  // Lookup table for all displayed streamlines
  vtkSetObjectMacro(StreamlineLookupTable, vtkLookupTable);
  vtkGetObjectMacro(StreamlineLookupTable, vtkLookupTable);

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
  // Color tracts based on clustering.
  // Colors the paths that have already been created using this class.
  // The argument (int tmp) is only there because under windows the 
  // wrapping failed with no argument.
  void ClusterTracts(int tmp);

  // Description
  // Get object that performs clustering (to set parameters)
  vtkGetObjectMacro(TractClusterer,vtkClusterTracts);


  // Description
  // To do list:
  // Add Print function
  // Add save functions
  // Add observers for progress/implement progress updating
  // Add check on all inputs

 protected:
  vtkMultipleStreamlineController();
  ~vtkMultipleStreamlineController();

  // functions not accessible to the user
  void CreateGraphicsObjects();
  void ApplyUserSettingsToGraphicsObject(int index);
  void DeleteStreamline(int index);
  int PointWithinTensorData(double *point, double *pointw);
  vtkHyperStreamline *CreateHyperStreamline();
  void SaveStreamlineAsTextFile(ofstream &filePoints,ofstream &fileAttribs,
                                vtkHyperStreamlinePoints *currStreamline);
  // Remove 0-length streamlines before clustering.
  void CleanStreamlines();

  vtkTransform *ROIToWorld;
  vtkTransform *WorldToTensorScaledIJK;

  vtkImageData *InputTensorField;
  vtkImageData *InputROI;
  vtkImageData *InputROIForIntersection;
  vtkCollection *InputRenderers;
  int InputROIValue;
  vtkShortArray *InputMultipleROIValues;
  
  vtkDoubleArray *ConvolutionKernel;

  vtkCollection *Streamlines;
  vtkCollection *Mappers;
  vtkCollection *Actors;
  int NumberOfVisibleActors;

  vtkProperty *StreamlineProperty;
 
  vtkPolyData *StreamlinesAsPolyLines;
  vtkIntArray *StreamlineIdPassTest;
  
  int ScalarVisibility;
  vtkLookupTable *StreamlineLookupTable;

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

  // Description:
  // The error code contains a possible error that occured while
  // writing a file.
  vtkSetMacro( ErrorCode, unsigned long );

  vtkClusterTracts *TractClusterer;

  unsigned long ErrorCode;
};

#endif
