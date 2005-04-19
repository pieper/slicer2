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
// .NAME vtkClusterTracts - Cluster paths obtained through tractography.

// .SECTION Description
// Wrapper around the classes vtkTractShapeFeatures and vtkNormalizedCuts.
// Outputs class labels from clustering input tract paths.

// .SECTION See Also
// vtkTractShapeFeatures vtkNormalizedCuts 

#ifndef __vtkClusterTracts_h
#define __vtkClusterTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "vtkNormalizedCuts.h"
#include "vtkTractShapeFeatures.h"

// Forward declarations to avoid including header files here.
// Goes along with use of new vtkCxxSetObjectMacro
class vtkCollection;


class VTK_DTMRI_EXPORT vtkClusterTracts : public vtkObject
{
 public:
  // Description
  // Construct
  static vtkClusterTracts *New();

  vtkTypeRevisionMacro(vtkClusterTracts,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);


  // Description
  // Set/Get input to this class: a collection of vtkHyperStreamlinePoints
  virtual void SetInputStreamlines(vtkCollection*);
  vtkGetObjectMacro(InputStreamlines, vtkCollection);

  // Description
  // Compute the output. Call this before requesting the output.
  void ComputeClusters();

  // Use ETX/BTX to exclude ITK objects from VTK-style wrapping
  //BTX
  typedef vtkNormalizedCuts::OutputClassifierType::OutputType OutputType;

  // Description
  // Get the output (note, can't be accessed from tcl).
  // This gives a list in which each input tract is assigned a class.
  // If there is an error in computation or ComputeClusters has not been
  // called, this will return a NULL pointer.
  OutputType *GetOutputMembershipSample()
    {
      if (this->NormalizedCuts->GetOutputClassifier())
    {
      return(this->NormalizedCuts->GetOutputClassifier()->GetOutput());
    }
      else
    {
      return(NULL);
    }
    }
  //ETX

  // Description
  // Access the underlying classes who perform the clustering function.
  // For setting parameters of these classes.
  vtkGetObjectMacro(NormalizedCuts, vtkNormalizedCuts);
  vtkGetObjectMacro(TractShapeFeatures, vtkTractShapeFeatures);
  
 protected:
  vtkClusterTracts();
  ~vtkClusterTracts();

  vtkCollection *InputStreamlines;
  vtkNormalizedCuts *NormalizedCuts;
  vtkTractShapeFeatures *TractShapeFeatures;

 private:
  vtkClusterTracts(const vtkClusterTracts&); // Not implemented.
  void operator=(const vtkClusterTracts&); // Not implemented.
};

#endif

