#include "vtkClusterTracts.h"

// for vtk objects we use here
#include "vtkObjectFactory.h"
#include "vtkCollection.h"

// classes for tract clustering
#include "vtkTractShapeFeatures.h"
#include "vtkNormalizedCuts.h"

// itk object for exception handling
#include "itkExceptionObject.h"

vtkCxxRevisionMacro(vtkClusterTracts, "$Revision: 1.3 $");
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
