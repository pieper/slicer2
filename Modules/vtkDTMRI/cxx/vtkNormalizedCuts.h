// .NAME vtkNormalizedCuts - Cluster N items using input NxN weight matrix.

// .SECTION Description
// Implementation of the Normalized Cuts algorithm for spectral clustering,
// as described in:
// Fowlkes et al., Spectral Grouping Using the Nystrom Method
// IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, 
// VOL. 26, NO. 2, FEBRUARY 2004
// The output is an itk classifier object, with labels for each of 
// the N input items.

// .SECTION See Also
// vtkClusterTracts vtkTractShapeFeatures

#ifndef __vtkNormalizedCuts_h
#define __vtkNormalizedCuts_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
// use itk numerics lib (vnl)
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
// ITK objects
#include "itkListSample.h"
#include "itkVector.h"
#include "itkSampleClassifier.h"

// Forward declarations to avoid including header files here.
// Goes along with use of new vtkCxxSetObjectMacro
class vtkImageData;

class VTK_DTMRI_EXPORT vtkNormalizedCuts : public vtkObject
{
 public:
  // Description
  // Construct
  static vtkNormalizedCuts *New();

  vtkTypeRevisionMacro(vtkNormalizedCuts,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Compute the output
  void ComputeClusters();

  // Description
  // Use the matrix format provided by vnl for the input similarity matrix
  //BTX
  typedef vnl_matrix<double> InputType;
  // TEST increase this, see comment in cxx file
  const static int InternalNumberOfEigenvectors = 2;

  typedef itk::Vector< double, InternalNumberOfEigenvectors > EmbedVectorType;
  typedef itk::Statistics::ListSample< EmbedVectorType > EmbedSampleType;
  typedef itk::Statistics::SampleClassifier< EmbedSampleType > OutputClassifierType;

  //ETX

  // Description
  // Set/Get input to this class
  void SetInputWeightMatrix(InputType* matrix)
    {
      // Don't delete this here if it is non-NULL, because the
      // class that created it is responsible for deleting it.
      // (It is not a VTK reference-counted object.)
      this->InputWeightMatrix = matrix;
    };
  
  InputType *GetInputWeightMatrix()
    {
      return this->InputWeightMatrix;
    };

  //BTX
  // Description
  // Get the classifier which is output from this class
  OutputClassifierType::Pointer GetOutputClassifier() 
    {
      return this->OutputClassifier;
    }
  //ETX

  // Description
  // Number of clusters to output
  vtkSetMacro(NumberOfClusters,int);
  vtkGetMacro(NumberOfClusters,int);

  // Description
  // Number of eigenvectors to use in the embedding
  // NOTE not currently implemented due to fixed-length itk vector
  //vtkSetMacro(NumberOfEigenvectors,int);
  vtkGetMacro(NumberOfEigenvectors,int);

  // Description
  // Get the intermediate computations of this class as images 
  // for visualization
  vtkImageData *GetNormalizedWeightMatrixImage();
  vtkImageData *GetEigenvectorsImage();

  // Description
  // Normalized cuts normalization of embedding vectors
  void SetEmbeddingNormalizationToRowSum()
    {
      this->SetEmbeddingNormalization(ROW_SUM);
    };
  // Description
  // Spectral clustering normalization of embedding vectors
  void SetEmbeddingNormalizationToLengthOne()
    {
      this->SetEmbeddingNormalization(LENGTH_ONE);
    };
  // Description
  // No normalization of embedding vectors
  void SetEmbeddingNormalizationToNone()
    {
      this->SetEmbeddingNormalization(NONE);
    };
  vtkGetMacro(EmbeddingNormalization,int);

  // Description
  // Make a vtk image to visualize contents of a vnl matrix
  vtkImageData *ConvertVNLMatrixToVTKImage(InputType *matrix);

 protected:
  vtkNormalizedCuts();
  ~vtkNormalizedCuts() {};

  //BTX
  InputType *InputWeightMatrix;
  vnl_symmetric_eigensystem<double> *EigenSystem;
  OutputClassifierType::Pointer OutputClassifier;
  //ETX

  int NumberOfClusters;
  int NumberOfEigenvectors;
  int EmbeddingNormalization;
  //BTX
  enum EmbeddingNormalizationType { NONE, ROW_SUM, LENGTH_ONE } ;
  //ETX
  vtkSetMacro(EmbeddingNormalization,int);

  vtkImageData *NormalizedWeightMatrixImage;
  vtkImageData *EigenvectorsImage;
  virtual void SetNormalizedWeightMatrixImage(vtkImageData *);
  virtual void SetEigenvectorsImage(vtkImageData *);

 private:
  vtkNormalizedCuts(const vtkNormalizedCuts&); // Not implemented.
  void operator=(const vtkNormalizedCuts&); // Not implemented.
};

#endif

