#include "vtkNormalizedCuts.h"

// for vtk objects we use here
#include "vtkObjectFactory.h"

// ITK objects
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkMinimumDecisionRule.h"

// for debug output/visualization of features
#include "vtkImageData.h"

// for random k-means init
#include <cstdlib>
#include <ctime>


vtkCxxRevisionMacro(vtkNormalizedCuts, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkNormalizedCuts);


vtkNormalizedCuts::vtkNormalizedCuts()
{
  this->InputWeightMatrix = NULL;

  this->NumberOfClusters = 2;
  this->NumberOfEigenvectors = 7;

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
  // Also create a vtkImageData which contains the same information.
  // This is for visualization.
  // Also create vtkImageDatas which contain the same information.
  // This is for visualization.
  if (this->NormalizedWeightMatrixImage) 
    {
      this->NormalizedWeightMatrixImage->Delete();
    }
  this->NormalizedWeightMatrixImage = vtkImageData::New();
  this->NormalizedWeightMatrixImage->
    SetDimensions(this->InputWeightMatrix->cols(),
          this->InputWeightMatrix->rows(),1);
  this->NormalizedWeightMatrixImage->SetScalarTypeToDouble();
  this->NormalizedWeightMatrixImage->AllocateScalars();
  double *weightMatrixImage = (double *) this->NormalizedWeightMatrixImage->
    GetScalarPointer();

  idx1=0;
  while (idx1 < this->InputWeightMatrix->rows())
    {
      idx2=0;
      while (idx2 < this->InputWeightMatrix->cols())
    {
      (*this->InputWeightMatrix)[idx1][idx2] = 
        ((*this->InputWeightMatrix)[idx1][idx2])/
        (rowWeightSum[idx1]*rowWeightSum[idx2]);
      
      *weightMatrixImage = (*this->InputWeightMatrix)[idx1][idx2]; 
      weightMatrixImage++;

      idx2++;
    }
      idx1++;
    }

  // Calculate eigenvectors
  vnl_symmetric_eigensystem<double> eigensystem(*this->InputWeightMatrix);

  vtkDebugMacro("Eigenvector matrix: " << eigensystem.V << 
        " Eigenvalues: " << eigensystem.D);


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
      // scale for slicer, cast to write png
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
      while (idx2 < this->InternalNumberOfEigenvectors)
    {
      // this was wrong.
      //ev[idx2]=(eigensystem.V[idx1][idx2+1])/rowWeightSum[idx1];
      ev[idx2]=(eigensystem.V[idx1][eigensystem.V.cols()-idx2-1])/rowWeightSum[idx1];
      idx2++;
    }

      // put the embedding vector for this tract onto the sample list
      embedding->PushBack( ev );      
      idx1++;
    }


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
  treeGenerator->Update();


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
  estimator->StartOptimization();

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

  this->OutputClassifier->Update();
  this->OutputClassifier->GetOutput()->DebugOn();

}
