#include "vtkClusterTracts.h"

// for vtk objects we use here
#include "vtkObjectFactory.h"
#include "vtkCollection.h"

// classes for tract clustering
#include "vtkTractShapeFeatures.h"
#include "vtkNormalizedCuts.h"


vtkCxxRevisionMacro(vtkClusterTracts, "$Revision: 1.2 $");
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
  this->TractShapeFeatures->ComputeFeatures();

  //this->NormalizedCuts->DebugOn();
  this->NormalizedCuts->SetInputWeightMatrix(this->TractShapeFeatures->GetOutputSimilarityMatrix());
  this->NormalizedCuts->ComputeClusters();

}
