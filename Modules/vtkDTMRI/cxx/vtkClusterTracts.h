h// .NAME vtkClusterTracts - Cluster paths obtained through tractography.

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

