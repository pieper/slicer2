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
// .NAME vtkDisplayTracts - 
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

#ifndef __vtkDisplayTracts_h
#define __vtkDisplayTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkCollection.h"
#include "vtkTransform.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkLookupTable.h"

// to be moved
#include "vtkClusterTracts.h"
#include "vtkDoubleArray.h"

class VTK_DTMRI_EXPORT vtkDisplayTracts : public vtkObject
{
 public:
  static vtkDisplayTracts *New();
  vtkTypeMacro(vtkDisplayTracts,vtkObject);

  // Description
  // Transformation used in seeding streamlines.  Their start
  // points are specified in the coordinate system of the ROI volume.
  // Transform world coordinates into scaled ijk of the tensor field.
  // This transform is needed to display streamlines in world
  // coordinates (thogh they are calculated in tensor scaled ijk)
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

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
  // List of the output vtkHyperStreamlines (or subclasses)
  vtkGetObjectMacro(Streamlines, vtkCollection);
  vtkGetObjectMacro(Actors, vtkCollection);
  vtkGetObjectMacro(Mappers, vtkCollection);
  vtkGetObjectMacro(TubeFilters, vtkCollection);
  int GetNumberOfStreamlines() {return this->Streamlines->GetNumberOfItems();}

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
  void SetScalarVisibility(int);
  vtkGetMacro(ScalarVisibility,int);
  vtkBooleanMacro(ScalarVisibility,int);

  // Description
  // Lookup table for all displayed streamlines
  vtkSetObjectMacro(StreamlineLookupTable, vtkLookupTable);
  vtkGetObjectMacro(StreamlineLookupTable, vtkLookupTable);

  // Description
  // This should be moved.
  // Color tracts based on clustering.
  // Colors the paths that have already been created using this class.
  // The argument (int tmp) is only there because under windows the 
  // wrapping failed with no argument.
  void ClusterTracts(int tmp);

  // Description
  // Get object that performs clustering (to set parameters)
  vtkGetObjectMacro(TractClusterer,vtkClusterTracts);

  
  //Description
  // This should be moved.
  // Find the streamlines that pass through the set of ROI values
  // stored in InputMultipleROIValues. This operation is performed
  // by convolving  the streamline with the kernel ConvolutionKernel.
  void FindStreamlinesThatPassThroughROI();
  void HighlightStreamlinesPassTest();
  void DeleteStreamlinesNotPassTest();
  // Description
  // Convolution Kernel that is used to convolve the fiber with when
  // finding the ROIs that the fiber passes through
  vtkSetObjectMacro(ConvolutionKernel, vtkDoubleArray);
  vtkGetObjectMacro(ConvolutionKernel, vtkDoubleArray);

 protected:
  vtkDisplayTracts();
  ~vtkDisplayTracts();

  // functions not accessible to the user
  void CreateGraphicsObjects();
  void ApplyUserSettingsToGraphicsObject(int index);
  void DeleteStreamline(int index);

  // To be moved
  vtkClusterTracts *TractClusterer;
  // Remove 0-length streamlines before clustering.
  void CleanStreamlines();
  // To be moved
  vtkDoubleArray *ConvolutionKernel;

  vtkTransform *WorldToTensorScaledIJK;

  vtkCollection *InputRenderers;

  vtkCollection *Streamlines;
  vtkCollection *Mappers;
  vtkCollection *TubeFilters;
  vtkCollection *Actors;
  int NumberOfVisibleActors;

  vtkProperty *StreamlineProperty;
 
  int ScalarVisibility;

  vtkLookupTable *StreamlineLookupTable;


};

#endif
