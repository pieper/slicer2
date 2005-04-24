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
#include "vtkClusterTracts.h"

// for vtk objects we use here
#include "vtkObjectFactory.h"
#include "vtkCollection.h"

// classes for tract clustering
#include "vtkTractShapeFeatures.h"
#include "vtkNormalizedCuts.h"

// itk object for exception handling
#include "itkExceptionObject.h"

vtkCxxRevisionMacro(vtkClusterTracts, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkClusterTracts);

vtkCxxSetObjectMacro(vtkClusterTracts, InputStreamlines, vtkCollection);

vtkClusterTracts::vtkClusterTracts()
{
  this->NormalizedCuts = vtkNormalizedCuts::New();
  this->TractShapeFeatures = vtkTractShapeFeatures::New();

  this->InputStreamlines = NULL;

}

vtkClusterTracts::~vtkClusterTracts()
{
  this->NormalizedCuts->Delete();
  this->TractShapeFeatures->Delete();
  if (this->InputStreamlines)
    this->InputStreamlines->Delete();
}


void vtkClusterTracts::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  this->NormalizedCuts->PrintSelf(os,indent);
  this->TractShapeFeatures->PrintSelf(os,indent);
  if (this->InputStreamlines)
    this->InputStreamlines->PrintSelf(os,indent);
}

void vtkClusterTracts::ComputeClusters()
{

  // test we have a streamline collection.
  if (this->InputStreamlines == NULL)
    {
      vtkErrorMacro("The InputStreamlines collection must be set first.");
      return;      
    }

  // Make sure we have at least twice as many streamlines as the number of
  // eigenvectors we are using (really we need at least one more than
  // this number to manage to calculate the eigenvectors, but double
  // is more reasonable).
  if (this->InputStreamlines->GetNumberOfItems() <  2*this->NormalizedCuts->GetNumberOfEigenvectors())
    {
      vtkErrorMacro("At least " << 
                    2*this->NormalizedCuts->GetNumberOfEigenvectors()  
                    << " tract paths are needed for clustering");
      return;      

    }    

  this->TractShapeFeatures->SetInputStreamlines(this->InputStreamlines);
  //this->TractShapeFeatures->DebugOn();

  try {
    this->TractShapeFeatures->ComputeFeatures();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in vtkTractShapeFeatures->ComputeFeatures: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in vtkTractShapeFeatures:ComputeFeatures");
    return;
  }

  //this->NormalizedCuts->DebugOn();
  this->NormalizedCuts->SetInputWeightMatrix(this->TractShapeFeatures->GetOutputSimilarityMatrix());

  try {
    this->NormalizedCuts->ComputeClusters();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in vtkNormalizedCuts->ComputeClusters: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in vtkNormalizedCuts:ComputeClusters");
    return;
  }
}
