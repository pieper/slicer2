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
// Displays a vtkCollection of vtkHyperStreamlines.
//
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
#include "vtkImplicitFunction.h"
#include "vtkHyperStreamline.h"
// for next vtk version:
//#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyDataSource.h"

class VTK_DTMRI_EXPORT vtkDisplayTracts : public vtkObject
{
 public:
  static vtkDisplayTracts *New();
  vtkTypeMacro(vtkDisplayTracts,vtkObject);

  // Description
  // Set the streamlines that we would like to visualize
  vtkGetObjectMacro(Streamlines, vtkCollection);
  vtkSetObjectMacro(Streamlines, vtkCollection);

  // Description
  // Number of sides of the tube displayed around each (hyper)streamline
  vtkGetMacro(TubeNumberOfSides, unsigned int);
  vtkSetMacro(TubeNumberOfSides, unsigned int);

  // Description
  // Radius of the tube displayed around each (hyper)streamline
  vtkGetMacro(TubeRadius, float);
  vtkSetMacro(TubeRadius, float);

  // Description
  // Update NumberOfSides and Radius in all tube filters.
  // The default behavior is just to apply radius/sides to newly created
  // tracts, so changes are only seen in new ones.  
  // Use this after changing TubeNumberOfSides or TubeRadius to apply 
  // to all existing tract paths.
  void UpdateAllTubeFiltersWithCurrentSettings();

  // Description
  // Transformation that was used in seeding streamlines.  Their start
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
  // Delete one streamline.
  void DeleteStreamline(int index);

  // Description
  // Delete all streamlines
  void DeleteAllStreamlines();

  // Description
  // Get the internal index of the chosen actor, if it is a streamline
  // in the collection.
  int GetStreamlineIndexFromActor(vtkActor *pickedActor);

  // Description
  // List of the output graphics objects
  vtkGetObjectMacro(Actors, vtkCollection);
  vtkGetObjectMacro(Mappers, vtkCollection);
  vtkGetObjectMacro(TubeFilters, vtkCollection);
  int GetNumberOfStreamlines() {return this->Streamlines->GetNumberOfItems();}

  // Description
  // Get modified streamlines
  vtkSetObjectMacro(ClippedStreamlines, vtkCollection);
  vtkGetObjectMacro(ClippedStreamlines, vtkCollection);

  // Description
  // Input: list of the renderers whose scenes will have streamlines
  // added.
  vtkSetObjectMacro(Renderers, vtkCollection);
  vtkGetObjectMacro(Renderers, vtkCollection);

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
  // controls clipping of tracts/streamlines
  void SetClipping(int);
  vtkGetMacro(Clipping,int);
  vtkBooleanMacro(Clipping,int);

  vtkSetObjectMacro(ClipFunction, vtkImplicitFunction );
  vtkGetObjectMacro(ClipFunction, vtkImplicitFunction );

  // Description
  // Lookup table for all displayed streamlines
  vtkSetObjectMacro(StreamlineLookupTable, vtkLookupTable);
  vtkGetObjectMacro(StreamlineLookupTable, vtkLookupTable);

  
 protected:
  vtkDisplayTracts();
  ~vtkDisplayTracts();

  // functions not accessible to the user
  void CreateGraphicsObjects();
  void ApplyUserSettingsToGraphicsObject(int index);
  vtkPolyDataSource *ClipStreamline(vtkHyperStreamline *streamline);

  vtkTransform *WorldToTensorScaledIJK;

  vtkCollection *Renderers;

  vtkCollection *Streamlines;
  vtkCollection *ClippedStreamlines;
  vtkCollection *Mappers;
  vtkCollection *TubeFilters;
  vtkCollection *Actors;
  int NumberOfVisibleActors;

  vtkProperty *StreamlineProperty;
 
  int ScalarVisibility;
  int Clipping;

  vtkLookupTable *StreamlineLookupTable;

  float TubeRadius;
  unsigned int TubeNumberOfSides;

  vtkImplicitFunction *ClipFunction;

};

#endif
