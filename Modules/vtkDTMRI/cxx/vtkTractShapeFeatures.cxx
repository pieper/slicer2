// for vtk objects we use here
#include "vtkHyperStreamlinePoints.h"
#include "vtkCollection.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"


#include "vtkTractShapeFeatures.h"


// ITK for tract feature computations
#include "itkListSample.h"
#include "itkCovarianceCalculator.h"
#include "itkVector.h"
#include "itkEuclideanDistance.h"
#include "itkSymmetricEigenSystem.h"

// for debug output of features
#include "vtkImageData.h"




vtkCxxRevisionMacro(vtkTractShapeFeatures, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkTractShapeFeatures);

vtkCxxSetObjectMacro(vtkTractShapeFeatures, InputStreamlines, vtkCollection);


vtkTractShapeFeatures::vtkTractShapeFeatures()
{
  this->InputStreamlines = NULL;
  this->InterTractDistanceMatrix = NULL;
  this->InterTractSimilarityMatrix = NULL;
  this->InterTractDistanceMatrixImage = NULL;
  this->InterTractSimilarityMatrixImage = NULL;

  this->Sigma = 100;

  this->FeatureType = MEAN_AND_COVARIANCE;
}

vtkTractShapeFeatures::~vtkTractShapeFeatures()
{
  if (this->InputStreamlines)
    this->InputStreamlines->Delete();

  if ( this->InterTractDistanceMatrix )
    {
      delete this->InterTractDistanceMatrix;
    }

  if ( this->InterTractSimilarityMatrix )
    {
      delete this->InterTractSimilarityMatrix;
    }
}

void vtkTractShapeFeatures::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Sigma: " << this->Sigma << "\n";

}


void vtkTractShapeFeatures::GetPointsFromHyperStreamlinePointsSubclass(TractPointsListType::Pointer sample, vtkHyperStreamlinePoints *currStreamline)
{
  XYZVectorType mv;
  vtkPoints *hs0, *hs1;
  int ptidx, numPts;
  double point[3];

  // clear the contents of the list sample
  sample->Clear();

  //GetHyperStreamline0/1 and FA at points.
  hs0=currStreamline->GetHyperStreamline0();
  hs1=currStreamline->GetHyperStreamline1();
  //attr0=currStreamline->GetFractionalAnisotropy0();
  //attr1=currStreamline->GetFractionalAnisotropy1();
  
  // Access the first one in reverse order since both lines
  // travel outward from the initial point.
  // Also, skip the first point in the second line since it
  // is a duplicate of the initial point.
  numPts=hs0->GetNumberOfPoints();
  ptidx=numPts-1;
  while (ptidx >= 0)
    {
      hs0->GetPoint(ptidx,point);
      mv[0]=point[0];
      mv[1]=point[1];
      mv[2]=point[2];
      sample->PushBack( mv );
      ptidx--;
    }
  numPts=hs1->GetNumberOfPoints();
  ptidx=1;
  while (ptidx < numPts)
    {
      hs1->GetPoint(ptidx,point);
      mv[0]=point[0];
      mv[1]=point[1];
      mv[2]=point[2];
      sample->PushBack( mv );
      ptidx++;
    }
}

void vtkTractShapeFeatures::ComputeFeatures()
{
  switch (this->FeatureType)
    {
    case MEAN_AND_COVARIANCE:
      {
    this->ComputeFeaturesMeanAndCovariance();
    break;
      }
    case HAUSDORFF:
      {
    this->ComputeFeaturesHausdorff();
    break;
      }
      
    }
}

void vtkTractShapeFeatures::ComputeFeaturesHausdorff()
{
  vtkHyperStreamlinePoints *currStreamline1, *currStreamline2;
  XYZVectorType mv1, mv2;

  int numberOfStreamlines = this->InputStreamlines->GetNumberOfItems();

  // Set up our matrices (the matrix class provided by vnl)
  if ( this->InterTractDistanceMatrix )
    {
      delete this->InterTractDistanceMatrix;
    }
  this->InterTractDistanceMatrix = 
    new OutputType(numberOfStreamlines,numberOfStreamlines,0);
  if ( this->InterTractSimilarityMatrix )
    {
      delete this->InterTractSimilarityMatrix;
    }
  this->InterTractSimilarityMatrix = 
    new OutputType(numberOfStreamlines,numberOfStreamlines,0);

  // put each tract's points onto a list
  TractPointsListType::Pointer sample1 = TractPointsListType::New();
  TractPointsListType::Pointer sample2 = TractPointsListType::New();

  // get ready to traverse streamline collection.
  // test we have a streamline collection.
  if (this->InputStreamlines == NULL)
    {
      vtkErrorMacro("The InputStreamline collection must be set before using this class.");
      return;      
    }
  
  this->InputStreamlines->InitTraversal();
  // TO DO: make sure this is a vtkHyperStreamlinePoints object
  currStreamline1= (vtkHyperStreamlinePoints *)this->InputStreamlines->GetNextItemAsObject();
  
  // test we have streamlines
  if (currStreamline1 == NULL)
    {
      vtkErrorMacro("No streamlines are on the collection.");
      return;      
    }

  vtkDebugMacro( "Traverse STREAMLINES" );
  double sumDist, countDist, minDist, currDist;
  TractPointsListType::InstanceIdentifier count1, count2, increment;
  unsigned int size1, size2;
  increment = 20;
  typedef itk::Statistics::EuclideanDistance< XYZVectorType > 
    DistanceMetricType;
  DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();

  // compute distances
  for (int i = 0; i < numberOfStreamlines; i++)
    {
      vtkDebugMacro( "Current Streamline: " << i);
      currStreamline1= (vtkHyperStreamlinePoints *)
    this->InputStreamlines->GetItemAsObject(i);

      // Get the tract path's points on an itk list sample object
      this->GetPointsFromHyperStreamlinePointsSubclass(sample1, 
                               currStreamline1);
      for (int j = 0; j < numberOfStreamlines; j++)
    {
      currStreamline2= (vtkHyperStreamlinePoints *)
        this->InputStreamlines->GetItemAsObject(j);

      // Get the tract path's points on an itk list sample object
      this->GetPointsFromHyperStreamlinePointsSubclass(sample2, 
                               currStreamline2);

      // Compare the tracts sample1 and sample2 using 
      // "average Hausdorff" distance.

      // vars for computing distance
      sumDist = 0;
      countDist = 0;

      size1 = sample1->Size();
      count1 = 0;
      while( count1 < size1 )
        {
          // minDist is min dist so far to this point
          minDist=VTK_FLOAT_MAX;
          
          size2 = sample2->Size();
          count2 = 0;
          while( count2 < size2 )
        {
          // distance between points
          currDist = distanceMetric->
            Evaluate( sample1->GetMeasurementVector(count1), 
                  sample2->GetMeasurementVector(count2) );

          vtkDebugMacro( "size1 size2 i,j, count1, count2, Dist (min, curr)" << size1 << " " << size2 << " " << sample1->GetMeasurementVector(count1) << " " << sample2->GetMeasurementVector(count2) << " " << i << " " << j << " " << count1 << " " << count2 << " " << minDist << " " << currDist);

          if (currDist < minDist)
            minDist=currDist;

          count2+=increment;
        }
          // accumulate the min dist to this point
          sumDist = sumDist + minDist; 
          countDist++;
          vtkDebugMacro( "sumDist: " << sumDist);
          count1+=increment;
        }

      // Store distance for this pair of tracts.
      // Save "average Hausdorff" (avg of min dists) in matrix.
      // normal Hausdorff is the max of the min dists.
      // Note this is a symmetric distance measure, we use the 
      // average of dist(a->b) and (b->a):
      (*this->InterTractDistanceMatrix)[i][j] += sumDist/(countDist*2);
      (*this->InterTractDistanceMatrix)[j][i] += sumDist/(countDist*2);
      //(*this->InterTractDistanceMatrix)[i][j] += i+j;
    }
    }


  // to convert distances to similarities/weights
  double sigmasq=this->Sigma*this->Sigma;

  // Also create vtkImageDatas which contain the same information as matrices.
  // This is for visualization.
  if (this->InterTractDistanceMatrixImage) 
    {
      this->InterTractDistanceMatrixImage->Delete();
    }
  if (this->InterTractSimilarityMatrixImage) 
    {
      this->InterTractSimilarityMatrixImage->Delete();
    }

  this->InterTractDistanceMatrixImage = vtkImageData::New();
  this->InterTractDistanceMatrixImage->
    SetDimensions(numberOfStreamlines,numberOfStreamlines,1);
  this->InterTractDistanceMatrixImage->SetScalarTypeToDouble();
  this->InterTractDistanceMatrixImage->AllocateScalars();
  double *distImage = (double *) this->InterTractDistanceMatrixImage->GetScalarPointer();

  this->InterTractSimilarityMatrixImage = vtkImageData::New();
  this->InterTractSimilarityMatrixImage->
    SetDimensions(numberOfStreamlines,numberOfStreamlines,1);
  this->InterTractSimilarityMatrixImage->SetScalarTypeToDouble();
  this->InterTractSimilarityMatrixImage->AllocateScalars();
  double *weightImage = (double *) this->InterTractSimilarityMatrixImage->GetScalarPointer();

  // Now create similarity matrix and copy into images
  for (int idx1 = 0; idx1 < numberOfStreamlines; idx1++)
    {
      for (int idx2 = 0; idx2 < numberOfStreamlines; idx2++)
    {
      // save the similarity in a matrix
      (*this->InterTractSimilarityMatrix)[idx1][idx2] = 
        exp(-((*this->InterTractDistanceMatrix)[idx1][idx2])/sigmasq);
      
      // save it also into the images for visualization
      *distImage = (*this->InterTractDistanceMatrix)[idx1][idx2];
      *weightImage = (*this->InterTractSimilarityMatrix)[idx1][idx2]; 
      
      distImage++;
      weightImage++;

    }
    }
  vtkDebugMacro( "Hausdorff distances computed." );

}


void vtkTractShapeFeatures::ComputeFeaturesMeanAndCovariance()
{
  vtkHyperStreamlinePoints *currStreamline;

  // to calculate covariance of the points
  typedef itk::Statistics::CovarianceCalculator< TractPointsListType > 
    CovarianceAlgorithmType;
  // the features are mean (3 values) + covariance matrix (6 unique values)
  // to make 9 total components in the feature vector
  typedef itk::Vector< double, 9 > FeatureVectorType;
  //typedef itk::Vector< double, 12 > FeatureVectorType;
  typedef itk::Statistics::ListSample< FeatureVectorType > FeatureListType;

  CovarianceAlgorithmType::MeanType mean;
  CovarianceAlgorithmType::OutputType cov;
  CovarianceAlgorithmType::Pointer covarianceAlgorithm = 
    CovarianceAlgorithmType::New();
  FeatureListType::Pointer features = FeatureListType::New();
  FeatureVectorType fv;

  // put each tract's points onto the list for covariance computation
  TractPointsListType::Pointer sample = TractPointsListType::New();

  // get ready to traverse streamline collection.
  // test we have a streamline collection.
  if (this->InputStreamlines == NULL)
    {
      vtkErrorMacro("The InputStreamline collection must be set before using this class.");
      return;      
    }
  
  this->InputStreamlines->InitTraversal();
  // TO DO: make sure this is a vtkHyperStreamlinePoints object
  currStreamline= (vtkHyperStreamlinePoints *)this->InputStreamlines->GetNextItemAsObject();
  
  // test we have streamlines
  if (currStreamline == NULL)
    {
      vtkErrorMacro("No streamlines are on the collection.");
      return;      
    }

  vtkDebugMacro( "Traverse STREAMLINES" );

  while(currStreamline)
    {
      // Get the tract path's points on an itk list sample object
      this->GetPointsFromHyperStreamlinePointsSubclass(sample, currStreamline);

      // now compute the covariance of all of the points in this sample (tract)
      covarianceAlgorithm->SetInputSample( sample );
      // the covariance algorithm will output the mean and covariance matrix
      covarianceAlgorithm->SetMean( 0 );
      covarianceAlgorithm->Update();

      vtkDebugMacro( "Mean = " << *(covarianceAlgorithm->GetMean()) );

      vtkDebugMacro( "Covariance = " << *(covarianceAlgorithm->GetOutput()) );
      
      mean = *(covarianceAlgorithm->GetMean());
      cov = *(covarianceAlgorithm->GetOutput());

      // compute the matrix square root of the covariance matrix
      // this makes the units of its eigenvalues mm, instead of mm^2
      // so its entries have the same scaling as the mean values
      // algorithm: diagonalize, take sqrt of eigenvalues, recreate matrix
      // sqrtm(A) = E * sqrt(D) * E' where D is a diagonal matrix.
      typedef itk::SymmetricEigenSystem< double, 3 > EigenType;
      EigenType::Pointer eig = EigenType::New();
      eig->SetMatrix(&cov);
      eig->Update();
      EigenType::EigenValueArrayType eigenvals = *(eig->GetEigenValues());
      eigenvals[0] = sqrt(eigenvals[0]);
      eigenvals[1] = sqrt(eigenvals[1]);
      eigenvals[2] = sqrt(eigenvals[2]);
      EigenType::EigenVectorArrayType eigenvects = *(eig->GetEigenVectors());


      // Normalize for tract length. We want orientation information
      // to not be swamped by length information.  So make the trace
      // of the covariance matrix equal one.
      //double norm;
      //norm = eigenvals[0]+eigenvals[1]+eigenvals[2];
      //eigenvals[0] = eigenvals[0]/norm;
      //eigenvals[1] = eigenvals[1]/norm;
      //eigenvals[2] = eigenvals[2]/norm;

      for (int i = 0; i < 3; i++)
    {
      for (int j = 0; i < 3; i++)
        {
          cov[i][j]=0;
        }
    }
      for (int i = 0; i < 3; i++)
    {
      // sum outer product matrix from each eigenvalue lambda*vv'
      // the ith eigenvector is in column i of the eigenvector matrix
      cov[0][0]=eigenvects[0][i]*eigenvects[0][i]*eigenvals[i];
      cov[0][1]=eigenvects[0][i]*eigenvects[1][i]*eigenvals[i];
      cov[0][2]=eigenvects[0][i]*eigenvects[2][i]*eigenvals[i];

      cov[1][0]=eigenvects[1][i]*eigenvects[0][i]*eigenvals[i];
      cov[1][1]=eigenvects[1][i]*eigenvects[1][i]*eigenvals[i];
      cov[1][2]=eigenvects[1][i]*eigenvects[2][i]*eigenvals[i];

      cov[2][0]=eigenvects[2][i]*eigenvects[0][i]*eigenvals[i];
      cov[2][1]=eigenvects[2][i]*eigenvects[1][i]*eigenvals[i];
      cov[2][2]=eigenvects[2][i]*eigenvects[2][i]*eigenvals[i];
    }
 
      // TEST
      //mean[0]=1;
      //mean[1]=1;
      //mean[2]=1;

      // save the features of mean and covariance on the features list
      // use the lower triangular part of the covariance matrix.
      fv[0]=mean[0];      
      fv[1]=mean[1];      
      fv[2]=mean[2];
      fv[3]=cov[0][0];
      fv[4]=cov[1][0];
      fv[5]=cov[1][1];
      fv[6]=cov[2][0];
      fv[7]=cov[2][1];
      fv[8]=cov[2][2];

      // test (minor?) eigenvector as feature, gives ori of tract plane
      //fv[9] = eigenvects[0][2];
      //fv[10] = eigenvects[1][2];
      //fv[11] = eigenvects[2][2];
      
      // Save this path's features on the list
      features->PushBack( fv );

      // get next object in collection
      currStreamline= (vtkHyperStreamlinePoints *)
        this->InputStreamlines->GetNextItemAsObject();
    }

  // See how many tracts' features we have computed
  vtkDebugMacro( "Number of tracts = " << features->Size() );


  // Now measure distance between feature vectors
  typedef itk::Statistics::EuclideanDistance< FeatureVectorType > 
    DistanceMetricType;
  DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();


  // Set up our matrices (the matrix class provided by vnl)
  if ( this->InterTractDistanceMatrix )
    {
      delete this->InterTractDistanceMatrix;
    }
  this->InterTractDistanceMatrix = 
    new OutputType(features->Size(),features->Size(),0);
  if ( this->InterTractSimilarityMatrix )
    {
      delete this->InterTractSimilarityMatrix;
    }
  this->InterTractSimilarityMatrix = 
    new OutputType(features->Size(),features->Size(),0);

  // Also create vtkImageDatas which contain the same information.
  // This is for visualization.
  if (this->InterTractDistanceMatrixImage) 
    {
      this->InterTractDistanceMatrixImage->Delete();
    }
  if (this->InterTractSimilarityMatrixImage) 
    {
      this->InterTractSimilarityMatrixImage->Delete();
    }

  this->InterTractDistanceMatrixImage = vtkImageData::New();
  this->InterTractDistanceMatrixImage->
    SetDimensions(features->Size(),features->Size(),1);
  this->InterTractDistanceMatrixImage->SetScalarTypeToDouble();
  this->InterTractDistanceMatrixImage->AllocateScalars();
  double *distImage = (double *) this->InterTractDistanceMatrixImage->GetScalarPointer();

  this->InterTractSimilarityMatrixImage = vtkImageData::New();
  this->InterTractSimilarityMatrixImage->
    SetDimensions(features->Size(),features->Size(),1);
  this->InterTractSimilarityMatrixImage->SetScalarTypeToDouble();
  this->InterTractSimilarityMatrixImage->AllocateScalars();
  double *weightImage = (double *) this->InterTractSimilarityMatrixImage->GetScalarPointer();

  // Now we need to iterate over features and create feature distance matrix
  FeatureListType::Iterator iter1 = features->Begin() ;
  FeatureListType::Iterator iter2;
  // indices into arrays of distances and weights
  int idx1 = 0;
  int idx2;

  // to convert distances to similarities/weights
  double sigmasq=this->Sigma*this->Sigma;

  while( iter1 != features->End() )
    {
      vtkDebugMacro( "id = " << iter1.GetInstanceIdentifier()  
             << "\t measurement vector = " 
             << iter1.GetMeasurementVector() 
             << "\t frequency = " 
             << iter1.GetFrequency() 
             ) ;
      
      iter2 = features->Begin() ;
      idx2 = 0;

      while( iter2 != features->End() )
    {
      // save the distance in a matrix
      (*this->InterTractDistanceMatrix)[idx1][idx2] = distanceMetric->
        Evaluate( iter1.GetMeasurementVector(), 
              iter2.GetMeasurementVector() );

      // save the similarity in a matrix
      (*this->InterTractSimilarityMatrix)[idx1][idx2] = 
        exp(-((*this->InterTractDistanceMatrix)[idx1][idx2])/sigmasq);

      // save it also into the images for visualization
      *distImage = (*this->InterTractDistanceMatrix)[idx1][idx2];
      *weightImage = (*this->InterTractSimilarityMatrix)[idx1][idx2]; 
      
      distImage++;
      weightImage++;

      vtkDebugMacro( "id1 = " << iter1.GetInstanceIdentifier()  
             << " id2 = " << iter2.GetInstanceIdentifier()  
             << " distance = "
             << (*this->InterTractDistanceMatrix)[idx1][idx2] );
      ++iter2 ;
      idx2++;
    }

      ++iter1 ;
      idx1++;
    }

}

