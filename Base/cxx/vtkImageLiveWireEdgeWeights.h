/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
// .NAME vtkImageLiveWireEdgeWeights -  Computes edge weights for input
// to vtkImageLiveWire.
// .SECTION Description
// 

#ifndef __vtkImageLiveWireEdgeWeights_h
#define __vtkImageLiveWireEdgeWeights_h

//  #define LIVEWIRE_TRAINING_EXPERIMENT

#include "vtkImageMultipleInputFilter.h"

//BTX

// This class contains all info necessary to use a computed feature
class featureProperties
{
 public:
  // pointer to member function to  convert image feature to cost (0<=cost<=1)
  float (featureProperties:: * Transform)(float val); 
  // parameters of this transform function
  float * TransformParams;
  int NumberOfParams;
  // weight, or importance, of this feature in the overall edge cost
  float Weight;
  featureProperties();
  ~featureProperties();

 protected:
  float GaussianCost(float x);

};

//ETX

class VTK_EXPORT vtkImageLiveWireEdgeWeights : public vtkImageMultipleInputFilter
{
public:
  static vtkImageLiveWireEdgeWeights *New();
  vtkTypeMacro(vtkImageLiveWireEdgeWeights,vtkImageMultipleInputFilter);

  // Description:
  // grayscale ImageData for creating edge weight map
  void SetOriginalImage(vtkImageData *image) {this->SetInput(0,image);}
  vtkImageData *GetOriginalImage() {return this->GetInput(0);}

  // Description:
  // colormap with previous (2D) image's contour drawn
  void SetPreviousContourImage(vtkImageData *image) {this->SetInput(1,image);}
  vtkImageData *GetPreviousContourImage() {return this->GetInput(1);}

  // Description:
  // colormap with points of interest marked for training
  void SetTrainingPointsImage(vtkImageData *image) {this->SetInput(2,image);}
  vtkImageData *GetTrainingPointsImage() {return this->GetInput(2);}

  // Description:
  // Maximum edge weight that this filter will output
  vtkGetMacro(MaxEdgeWeight, int);
  vtkSetMacro(MaxEdgeWeight, int);

  // Description:
  // Direction of edges to compute
#define UP_EDGE 0
#define DOWN_EDGE 1
#define LEFT_EDGE 2
#define RIGHT_EDGE 3
  vtkGetMacro(EdgeDirection, int);
  vtkSetMacro(EdgeDirection, int);

  // Description:
  // 
  vtkGetMacro(Difference, float);
  vtkSetMacro(Difference, float);

  // Description:
  // 
  vtkGetMacro(InsidePixel, float);
  vtkSetMacro(InsidePixel, float);

  // Description:
  // 
  vtkGetMacro(OutsidePixel, float);
  vtkSetMacro(OutsidePixel, float);

  // Description: 
  // Set/Get the filename where the feature values will be written
  // Used for feature training experiments
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Number of features computed per voxel
  vtkGetMacro(NumberOfFeatures, int);
  //vtkSetMacro(NumberOfFeatures, int);

  // Description:
  // Size of neighborhood to compute features in
  vtkGetMacro(Neighborhood, int);
  //vtkSetMacro(Neighborhood, int);

  // Description:
  // Set/Gets are for access from tcl
  // Lauren check bounds!!!!!!!!!!
  // Set a parameter used to compute this feature
  void SetParamForFeature(int featureNum, int paramNum, float param) 
    {this->FeatureSettings[featureNum].TransformParams[paramNum] = param;};
  // Get total number of parameters used to compute this feature
  int GetNumberOfParamsForFeature(int f) 
    {return this->FeatureSettings[f].NumberOfParams;};
  // Get value of a parameter
  float GetParamForFeature(int f, int p)
    {return this->FeatureSettings[f].TransformParams[p];};
  float GetWeightForFeature(int f)
    {return this->FeatureSettings[f].Weight;};
  float SetWeightForFeature(int f, float w)
    {this->FeatureSettings[f].Weight = w;};

  // Description:
  // Get all parameters used to compute the ith feature
  featureProperties *GetFeatureSettings(int f) {return &this->FeatureSettings[f];};

  // Description:
  // training...
  vtkSetObjectMacro(TrainingPoints, vtkPoints);
  vtkGetObjectMacro(TrainingPoints, vtkPoints);

  vtkBooleanMacro(TrainingMode, int);
  vtkSetMacro(TrainingMode, int);
  vtkGetMacro(TrainingMode, int);

  // Description:
  // internal access from Execute: don't call this
  //vtkGetMacro(TrainingAverages, float*);
  float *GetTrainingAverages(){return this->TrainingAverages;};

protected:
  vtkImageLiveWireEdgeWeights();
  ~vtkImageLiveWireEdgeWeights();

  int MaxEdgeWeight;

  // User settings that affect edge weight calculation
  float Difference;
  float InsidePixel;
  float OutsidePixel; 
  int EdgeDirection;

  // Total number of features to compute
  int NumberOfFeatures;

  // Neighborhood to compute features in
  int Neighborhood;

  // Array containing all settings for each feature
  // The intention is to make adding new features simple and general
  featureProperties *FeatureSettings;

  // for testing
  char *FileName;

  vtkPoints *TrainingPoints;
  int TrainingMode;
  float *TrainingAverages;

  void ExecuteInformation(vtkImageData **inputs, vtkImageData *output); 

  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData, 
    int extent[6], int id);
};

#endif

