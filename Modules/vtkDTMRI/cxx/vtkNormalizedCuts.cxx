#include "vtkNormalizedCuts.h"

// for vtk objects we use here
#include "vtkObjectFactory.h"

// ITK objects
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkMinimumDecisionRule.h"
#include "itkExceptionObject.h"

// for debug output/visualization of features
#include "vtkImageData.h"

// for random k-means init
#include <cstdlib>
#include <ctime>


vtkCxxRevisionMacro(vtkNormalizedCuts, "$Revision: 1.7 $");
vtkStandardNewMacro(vtkNormalizedCuts);


vtkNormalizedCuts::vtkNormalizedCuts()
{
  this->NumberOfClusters = 2;
  this->NumberOfEigenvectors = this->InternalNumberOfEigenvectors;
  this->EmbeddingNormalization = ROW_SUM;

  this->NormalizedWeightMatrixImage = NULL;
  this->EigenvectorsImage = NULL;
}

void vtkNormalizedCuts::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "InputWeightMatrix: " << this->InputWeightMatrix << "\n";
  os << indent << "NumberOfClusters: " << this->NumberOfClusters << "\n";
  os << indent << "NumberOfEigenvectors: " << this->NumberOfEigenvectors << "\n";

}

void vtkNormalizedCuts::ComputeClusters()
{

  // NOTE: what is the way to do this correctly for itk pointers?
  // First clear output in case we exit with an error.
  // This prevents any old output from being re-used after such an error.
  //this->OutputClassifier->Delete();


  // test we have input
  if (this->InputWeightMatrix == NULL)
    {
      vtkDebugMacro("You must set the InputWeightMatrix before using this class.");
      return;
    }

  // first normalize the matrix
  typedef vnl_vector<double> VectorType;
  // create vector initialized to zero
  VectorType rowWeightSum(this->InputWeightMatrix->rows(),0);
  int idx1, idx2;
  idx1=0;
  while (idx1 < this->InputWeightMatrix->rows())
    {
      idx2=0;
      while (idx2 < this->InputWeightMatrix->cols())
    {
      // sum the row (for matrix normalization later)
      rowWeightSum[idx1] += (*this->InputWeightMatrix)[idx1][idx2];
      // TEST to turn off normalization 
      // rowWeightSum[idx1] =1;
      idx2++;
    }
      // take square root of final row sum
      rowWeightSum[idx1] = sqrt(rowWeightSum[idx1]);
      vtkDebugMacro("row " << idx1 << " sum: " << rowWeightSum[idx1]);
      idx1++;
    }


  // Create normalized Laplacian from weight matrix.
  // See Fowlkes et al., Spectral Grouping Using the Nystro¨m Method
  // IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, 
  // VOL. 26, NO. 2, FEBRUARY 2004
  // Normalize: D^(-1/2) W D^(-1/2) 
  // Where W = square, symmetric weight matrix.
  // And D = diagonal matrix with row sum of W on diagonal
  // Then leading eigenvectors of the above are used to partition.
  // This D^(-1/2) normalization method amounts to the following:
  // compute row sum of W as a vector, for each
  // number then take 1/sqrt of it.  Then multiply columns and rows 
  // by this.
  // Iterate over the matrix to perform normalization.
  // Delete any old image from previous inputs
  if (this->NormalizedWeightMatrixImage) 
    {
      this->NormalizedWeightMatrixImage->Delete();
    }

  idx1=0;
  while (idx1 < this->InputWeightMatrix->rows())
    {
      idx2=0;
      while (idx2 < this->InputWeightMatrix->cols())
    {
      (*this->InputWeightMatrix)[idx1][idx2] = 
        ((*this->InputWeightMatrix)[idx1][idx2])/
        (rowWeightSum[idx1]*rowWeightSum[idx2]);
      
      idx2++;
    }
      idx1++;
    }

  // Test the matrix is finite (not NaN or Inf), otherwise can have
  // an error in eigensystem computation
  if (!this->InputWeightMatrix->is_finite())
    {    
      // Exit here with an error
      vtkErrorMacro("Normalized input weight matrix not finite (Nan or Inf)");
      return;
    }

  // Calculate eigenvectors
  vnl_symmetric_eigensystem<double> eigensystem(*this->InputWeightMatrix);

  vtkDebugMacro("Eigenvector matrix: " << eigensystem.V << 
        " Eigenvalues: " << eigensystem.D);

  vtkErrorMacro("Eigenvalues: " << eigensystem.D);


  if (this->EigenvectorsImage) 
    {
      this->EigenvectorsImage->Delete();
    }
  this->EigenvectorsImage = vtkImageData::New();
  this->EigenvectorsImage->
    SetDimensions(this->InputWeightMatrix->cols(),
          this->InputWeightMatrix->rows(),1);
  this->EigenvectorsImage->SetScalarTypeToDouble();
  this->EigenvectorsImage->AllocateScalars();
  double *eigenvectorsImage = (double *) this->EigenvectorsImage->
    GetScalarPointer();

  idx1=0;
  while (idx1 < eigensystem.V.rows())
    {
      idx2=0;
      while (idx2 < eigensystem.V.cols())
    {
      *eigenvectorsImage = eigensystem.V[idx1][idx2]; 
      eigenvectorsImage++;
      idx2++;
    }
      idx1++;
    }


  // Create new feature vectors using the eigenvector embedding.
  // eigenvectors are be sorted with smoothest last. 
  // TEST must have more than this many tracts for the code to run
  // TEST this is a problem, must specify vector length at compile time.
  // TEST Need to specify a larger number here, 
  // TEST then see if zero-padding affects the classifier
  EmbedVectorType ev;
  EmbedSampleType::Pointer embedding = EmbedSampleType::New();
  
  // TEST write to disk
  ofstream fileEmbed;
  fileEmbed.open("embed.txt");

  idx1=0;
  // outer loop over rows of eigenvector matrix, 
  // pick out all entries for one tract
  while (idx1 < eigensystem.V.rows())
    {    
      idx2=0;
      // inner loop over columns of eigenvector matrix
      // place entries for this tract into an itk vector
      // skip first eigenvector.
      // TEST put formula here

      double length = 0;
      while (idx2 < this->InternalNumberOfEigenvectors)
    {
      // this was wrong.
      //ev[idx2]=(eigensystem.V[idx1][idx2+1])/rowWeightSum[idx1];
      // This included the constant major eigenvector 
      //ev[idx2]=(eigensystem.V[idx1][eigensystem.V.cols()-idx2-1]);

      // This is correct.
      ev[idx2]=(eigensystem.V[idx1][eigensystem.V.cols()-idx2-2]);

      if (this->EmbeddingNormalization == LENGTH_ONE)
    {
      length += ev[idx2]*ev[idx2];
    }
      if (this->EmbeddingNormalization == ROW_SUM)
    {
      ev[idx2]=ev[idx2]/rowWeightSum[idx1];
    }
      // else don't normalize

      idx2++;
    }
      
      // If the embedding normalization is by length, normalize now
      if (this->EmbeddingNormalization == LENGTH_ONE)
    {
      length = sqrt(length);
      if (length == 0)
        {
          vtkErrorMacro("0-length embedding vector %d" << idx1);
          length = 1;
        }
      for (idx2 = 0; idx2 < this->InternalNumberOfEigenvectors; idx2++)
        {
          ev[idx2]=ev[idx2]/length;
        }
    }

      // TEST write to disk
      for (idx2 = 0; idx2 < this->InternalNumberOfEigenvectors; idx2++)
        {
          fileEmbed << ev[idx2] << " ";
        }
      fileEmbed << endl;

      // put the embedding vector for this tract onto the sample list
      embedding->PushBack( ev );      
      idx1++;
    }

  // TEST write to disk
  fileEmbed.close();

  // Create a Kd tree and populate it with the embedding vectors,
  // in order to run k-means.
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< EmbedSampleType > 
    TreeGeneratorType;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

  treeGenerator->SetSample( embedding );
  // This is the number of items that can go into a leaf node.
  // it seems not to work with 1
  treeGenerator->SetBucketSize( 3 );
  //treeGenerator->SetBucketSize( 16 );
  try {
    treeGenerator->Update();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in tree generation: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in tree generation");
    return;
  }

  // Run kmeans on the new vectors (eigenvector embedding of tract info)
  // store these as that sample list with classes 
  typedef TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();

  // In the parameters vector, the vectors for each of the means are 
  // concatenated.  So array size is vector_length*number_of_vector_means.
  int numberOfClusters=this->NumberOfClusters;
  EstimatorType::ParametersType initialMeans(this->InternalNumberOfEigenvectors*numberOfClusters);
  idx2=0;
  std::srand ( static_cast<unsigned>(time(0)) );
  idx1=0;
  int sampleIdx;
  int meanIdx =0;
  while (idx1 < numberOfClusters)
    {
      // Init means with randomly selected sample member vectors.
      // index is random number between 0 and number of vectors-1.
      sampleIdx = std::rand()%embedding->Size();
      ev = embedding->GetMeasurementVector(sampleIdx);
      idx2=0;
      while (idx2 < this->InternalNumberOfEigenvectors)
    {
      initialMeans[meanIdx] = ev[idx2];
      idx2++;
      meanIdx++;
    }

      idx1++;
    }



  estimator->SetParameters( initialMeans );
  vtkDebugMacro("estimator params: " << estimator->GetParameters());
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 200 );
  //estimator->SetMaximumIteration( 1000 );
  //estimator->SetMaximumIteration( 20 );
  estimator->SetCentroidPositionChangesThreshold(0.0);
  try {
    estimator->StartOptimization();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in cluster estimation: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in cluster estimation");
    return;
  }

  EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();
  vtkDebugMacro("Final estimator params: " << estimator->GetParameters());
  for ( unsigned int i = 0 ; i < 2 ; ++i )
    {
      std::cout << "cluster[" << i << "] " << std::endl;
      std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
      
      vtkDebugMacro("cluster[" << i << "] ");
      vtkDebugMacro("    estimated mean : " << estimatedMeans[i]);
    }

  typedef itk::Statistics::EuclideanDistance< EmbedVectorType > 
    MembershipFunctionType;
  typedef itk::MinimumDecisionRule DecisionRuleType;
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();
  
  // this smart pointer will deallocate anything old it may have
  // pointed to, so this line is okay.
  this->OutputClassifier = OutputClassifierType::New();

  //this->OutputClassifier->DebugOn();

  this->OutputClassifier->SetDecisionRule( (itk::DecisionRuleBase::Pointer) decisionRule);
  this->OutputClassifier->SetSample( embedding );
  // need to keep this object around, increment its reference count
  // otherwise it is deleted but the classifier is still around, 
  // and we need to fully use the classifier later.
  embedding->Register();
  //embedding->DebugOn();
  this->OutputClassifier->SetNumberOfClasses( numberOfClusters );

  std::vector< unsigned int > classLabels;
  classLabels.resize( numberOfClusters );
  idx1=0;
  while (idx1 < numberOfClusters)
    {
      classLabels[idx1] = idx1;
      idx1++;
    }

  this->OutputClassifier->SetMembershipFunctionClassLabels( classLabels );


  std::vector< MembershipFunctionType::Pointer > membershipFunctions;
  MembershipFunctionType::OriginType origin;
  int index = 0;
  for ( unsigned int i = 0 ; i < numberOfClusters ; i++ ) 
    {
      membershipFunctions.push_back( MembershipFunctionType::New() );
      for ( unsigned int j = 0 ; j < EmbedSampleType::MeasurementVectorSize ; j++ )
        {
          origin[j] = estimatedMeans[index++];
        }
      membershipFunctions[i]->SetOrigin( origin );

      vtkDebugMacro("origin " << i << " " << membershipFunctions[i]->GetOrigin());

      this->OutputClassifier->AddMembershipFunction( membershipFunctions[i].GetPointer() );
    }

  try {
    this->OutputClassifier->Update();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in update of output classifier: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in update of output classifier");
    return;
  }
  //this->OutputClassifier->GetOutput()->DebugOn();

}



vtkImageData * vtkNormalizedCuts::GetNormalizedWeightMatrixImage()
{
  return (this->ConvertVNLMatrixToVTKImage(this->InputWeightMatrix,this->NormalizedWeightMatrixImage));
}

vtkImageData * vtkNormalizedCuts::ConvertVNLMatrixToVTKImage(InputType *matrix, vtkImageData *image)
{
  // If the image hasn't been created, create it
  if (image == NULL)
    {
      image = vtkImageData::New();

      if (matrix != NULL)
    {
      int rows = matrix->rows();
      int cols = matrix->cols();
      image->SetDimensions(cols,rows,1);
      image->SetScalarTypeToDouble();
      image->AllocateScalars();
      double *imageArray = (double *) image->GetScalarPointer();
      
      for (int idx1 = rows-1; idx1 >= 0; idx1--)
        {
          for (int idx2 = 0; idx2 < cols; idx2++)
        {
          *imageArray = (*matrix)[idx1][idx2];
          imageArray++;
        }
        }
    }
    }

  return (image);

}
