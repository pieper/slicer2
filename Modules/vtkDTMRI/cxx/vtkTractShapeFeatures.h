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
// .NAME vtkTractShapeFeatures - Compute tract similarity matrix for clustering

// .SECTION Description
// Takes as input a collection of N tracts (vtkHyperStreamlinePoints objects). 
// Compares them, and outputs an NxN weight matrix for clustering.
// The same matrix is output also as an image for visualization.

// .SECTION See Also
// vtkClusterTracts vtkNormalizedCuts 

#ifndef __vtkTractShapeFeatures_h
#define __vtkTractShapeFeatures_h

#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
#include "itkListSample.h"
#include "itkVariableSizeMatrix.h"

// Forward declarations to avoid including header files here.
// Goes along with use of new vtkCxxSetObjectMacro
class vtkCollection;
class vtkImageData;
class vtkHyperStreamlinePoints;

class VTK_DTMRI_EXPORT vtkTractShapeFeatures : public vtkObject
{
 public:
  // Description
  // Construct
  static vtkTractShapeFeatures *New();

  vtkTypeRevisionMacro(vtkTractShapeFeatures,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);


  // Description
  // Compute the output
  void ComputeFeatures();

  // Description
  // Set/Get input to this class
  virtual void SetInputStreamlines(vtkCollection*);
  vtkGetObjectMacro(InputStreamlines, vtkCollection);

  // Description
  // Set sigma of gaussian kernel for tract comparison
  vtkSetMacro(Sigma,double);
  vtkGetMacro(Sigma,double);

  // Description
  // For Hausdorff only.  Use every Nth point on the tract path for 
  // comparison. 
  vtkSetClampMacro(HausdorffN,int,1,100);
  vtkGetMacro(HausdorffN,int);

  
  //BTX
  // (wrapping doesn't work here so exclude this with BTX)
  // Description
  // Use the matrix format provided by ITK for the output distance
  // and similarity matrices
  typedef double AffinityMatrixValueType;
  typedef itk::VariableSizeMatrix< AffinityMatrixValueType > OutputType;

  // Description
  // Returns the output distance matrix after computation
  OutputType * GetOutputDistanceMatrix() 
    {
      return &m_InterTractDistanceMatrix;
    };

  // Description
  // Returns the output similarity/weight matrix after computation
  OutputType * GetOutputSimilarityMatrix() 
    {
      return &m_InterTractSimilarityMatrix;
    };
  //ETX

  // Description
  // Get the images which contain the matrices above (for visualization)
  vtkImageData *GetInterTractDistanceMatrixImage();
  vtkImageData *GetInterTractSimilarityMatrixImage();

  void SetFeatureTypeToHausdorff()
    {
      this->SetFeatureType(HAUSDORFF);
    };
  void SetFeatureTypeToMeanAndCovariance()
    {
      this->SetFeatureType(MEAN_AND_COVARIANCE);
    };
  void SetFeatureTypeToEndPoints()
    {
      this->SetFeatureType(ENDPOINTS);
    };
  vtkGetMacro(FeatureType,int);

  // Description
  // Make a vtk image to visualize contents of a vnl matrix
  vtkImageData * ConvertVNLMatrixToVTKImage(OutputType *matrix, vtkImageData *image);

 protected:
  vtkTractShapeFeatures();
  ~vtkTractShapeFeatures();

  vtkCollection *InputStreamlines;
  double Sigma;
  int HausdorffN;

  vtkImageData *InterTractDistanceMatrixImage;
  vtkImageData *InterTractSimilarityMatrixImage;

  void ComputeFeaturesMeanAndCovariance();
  void ComputeFeaturesHausdorff();
  void ComputeFeaturesEndPoints();
  
  //BTX
  enum ShapeFeatureType { MEAN_AND_COVARIANCE, HAUSDORFF, ENDPOINTS } ;
  //ETX
  int FeatureType;
  vtkSetMacro(FeatureType,int);


  //BTX
  // ITK typedefs
  // x,y,z points on tract paths are 3-vectors
  typedef itk::Vector< double, 3 > XYZVectorType;
  // list of all points on a tract path
  typedef itk::Statistics::ListSample< XYZVectorType > TractPointsListType;

  OutputType m_InterTractDistanceMatrix;
  OutputType m_InterTractSimilarityMatrix;

  void GetPointsFromHyperStreamlinePointsSubclass(TractPointsListType::Pointer sample, vtkHyperStreamlinePoints *currStreamline);
  //ETX

 private:
  vtkTractShapeFeatures(const vtkTractShapeFeatures&); // Not implemented.
  void operator=(const vtkTractShapeFeatures&); // Not implemented.
};

#endif

