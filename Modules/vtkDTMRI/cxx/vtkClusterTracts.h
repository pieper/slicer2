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
// Wrapper around the classes vtkTractShapeFeatures and itkSpectralClustering
// Outputs class labels from clustering input tract paths.

// .SECTION See Also
// vtkTractShapeFeatures itkSpectralClustering 

#ifndef __vtkClusterTracts_h
#define __vtkClusterTracts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "itkSpectralClustering.h"
#include "vtkTractShapeFeatures.h"
#include "vtkUnsignedIntArray.h"

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

  //BTX
  typedef vtkUnsignedIntArray OutputType;
  //ETX

  // Description
  // User interface parameter to itk clustering class:
  // Number of output clusters
  vtkSetMacro(NumberOfClusters,unsigned int);
  vtkGetMacro(NumberOfClusters,unsigned int);

  // Description
  // User interface parameter to itk clustering class:
  // Number of eigenvectors to use in embedding
  vtkSetMacro(NumberOfEigenvectors,unsigned int);
  vtkGetMacro(NumberOfEigenvectors,unsigned int);

  // Description
  // User interface parameter to itk clustering class:
  // Type of normalization of embedding vectors
  int GetEmbeddingNormalization()
    {
      return this->ClusteringAlgorithm->GetEmbeddingNormalization();
    }

  // Description
  // User interface parameter to itk clustering class:
  // Type of normalization of embedding vectors:
  // Normalized cuts normalization of embedding vectors
  void SetEmbeddingNormalizationToRowSum()
    {
      this->ClusteringAlgorithm->SetEmbeddingNormalizationToRowSum();
    };

  // Description
  // User interface parameter to itk clustering class:
  // Type of normalization of embedding vectors:
  // Spectral clustering normalization of embedding vectors
  void SetEmbeddingNormalizationToLengthOne()
    {
      this->ClusteringAlgorithm->SetEmbeddingNormalizationToLengthOne();
    };

  // Description
  // User interface parameter to itk clustering class:
  // Type of normalization of embedding vectors:
  // No normalization of embedding vectors 
  void SetEmbeddingNormalizationToNone()
    {
      this->ClusteringAlgorithm->SetEmbeddingNormalizationToNone();
    };

  // Description
  // Set/Get input to this class: a collection of vtkHyperStreamlinePoints
  virtual void SetInputStreamlines(vtkCollection*);
  vtkGetObjectMacro(InputStreamlines, vtkCollection);


  // Description
  // For direct access to the tract affinity matrix calculation class,
  // to set parameters from the user interface.
  vtkGetObjectMacro(TractAffinityCalculator, vtkTractShapeFeatures);

  // Description
  // Compute the output. Call this before requesting the output.
  void ComputeClusters();


  // Description
  // This gives a list in which each input tract is assigned a class.
  // If there is an error in computation or ComputeClusters has not been
  // called, this will return a NULL pointer.
  vtkGetObjectMacro(OutputClusterLabels,vtkUnsignedIntArray);
  vtkUnsignedIntArray *GetOutput()
    {
      return(this->GetOutputClusterLabels());
    }

 protected:
  vtkClusterTracts();
  ~vtkClusterTracts();

  vtkCollection *InputStreamlines;

  //BTX
  // It is important to use the special pointer or the
  // object will delete itself.
  itk::SpectralClustering::Pointer ClusteringAlgorithm;
  //ETX

  vtkTractShapeFeatures *TractAffinityCalculator;

  vtkUnsignedIntArray *OutputClusterLabels;

  unsigned int NumberOfClusters;
  unsigned int NumberOfEigenvectors;
  int          EmbeddingNormalization;

 private:
  vtkClusterTracts(const vtkClusterTracts&); // Not implemented.
  void operator=(const vtkClusterTracts&); // Not implemented.
};

#endif

