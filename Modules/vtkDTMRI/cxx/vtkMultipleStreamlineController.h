// .NAME vtkImageDiffusionTensor - 
// .SECTION Description

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


class VTK_DTMRI_EXPORT vtkMultipleStreamlineController : public vtkObject
{
 public:
  static vtkMultipleStreamlineController *New();
  vtkTypeMacro(vtkMultipleStreamlineController,vtkObject);

  void SeedStreamlinesFromROI();

  // Description
  // Input tensor field in which to seed streamlines
  vtkSetObjectMacro(InputTensorField, vtkImageData);
  vtkGetObjectMacro(InputTensorField, vtkImageData);

  // Description
  // Input ROI volume describing where to see streamlines
  vtkSetMacro(InputROIValue, int);
  vtkGetMacro(InputROIValue, int);

  // Description
  // Input ROI volume describing where to see streamlines
  vtkSetObjectMacro(InputROI, vtkImageData);
  vtkGetObjectMacro(InputROI, vtkImageData);

  // Description
  // Transformation used in seeding streamlines, whose start
  // points are specified in the coordinate system of the ROI volume.
  // Transform the ijk coordinates of the ROI to world coordinates.
  vtkSetObjectMacro(ROIToWorld, vtkTransform);
  vtkGetObjectMacro(ROIToWorld, vtkTransform);

  // Description
  // Transformation used in seeding streamlines, whose start
  // points are specified in the coordinate system of the ROI volume.
  // Transform world coordinates into scaled ijk of the tensor field.
  vtkSetObjectMacro(WorldToTensorScaledIJK, vtkTransform);
  vtkGetObjectMacro(WorldToTensorScaledIJK, vtkTransform);

  // Description
  // List of the output vtkHyperStreamlines (or subclasses)
  vtkSetObjectMacro(Streamlines, vtkCollection);
  vtkGetObjectMacro(Streamlines, vtkCollection);

 protected:
  vtkMultipleStreamlineController();
  ~vtkMultipleStreamlineController();

  vtkTransform *ROIToWorld;
  vtkTransform *WorldToTensorScaledIJK;

  vtkImageData *InputTensorField;
  vtkImageData *InputROI;

  int InputROIValue;
  vtkCollection *Streamlines;
};

#endif
