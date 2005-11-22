/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology 

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/

#ifndef _EMLOCALSHAPE_H_INCLUDED
#define _EMLOCALSHAPE_H_INCLUDED 1

#include <vtkEMLocalSegmentConfigure.h>
#include "EMLocalInterface.h" 
#include "vtkMultiThreader.h"

typedef struct {
  float Result;
  int VoxelStart[3];
  int DataJump;
  int *PCAMeanShapeJump;
  int **PCAEigenVectorsJump;
  int *ProbDataJump;
  // Job specifc number of Voxels in Image Space (not considering boundaries); 
  int NumberOfVoxels;
} EMLocalShape_MultiThreadedParameters;

class VTK_EMLOCALSEGMENT_EXPORT EMLocalShape { 
public:
  // ------------------------
  // Core Functions
  // ------------------------

  // Interface of this class - starts the optimization of the PCA model 
  void CalculateOptimalParameters(float **PCAShapeParameters, int PCAMaxX, int PCAMinX, int PCAMaxY, int PCAMinY, 
                             int PCAMaxZ, int PCAMinZ, int BoundaryMinX, int BoundaryMinY, int BoundaryMinZ, int Boundary_LengthX, int Boundary_LengthY, 
                             float** w_m, unsigned char* PCA_ROI, void  **initProbDataPtr, float** initPCAMeanShapePtr, 
                             int* initPCAMeanShapeIncY, int *initPCAMeanShapeIncZ, float*** initPCAEigenVectorsPtr, int **initPCAEigenVectorsIncY, 
                             int** initPCAEigenVectorsIncZ, float &PCACost);

  // Interface with MultiThreaded cost functions
  float ShapeCostFunctionMultiThreaded(float *initPCAPara);

  // Initialize values
  EMLocalShape(EMLocal_Hierarchical_Class_Parameters* initEMHierarchyParameters, int  *initPCANumberOfEigenModes, int DisableMultiThreading);
  ~EMLocalShape();

  // ------------------------
  // Set/Get  Function
  // ------------------------

  void InitRegSpecPara(int RegistrationType, int LengthX, int LengthY, int LengthZ);

  int GetROI_MinZ(){return this->ROI_MinZ;}
  int GetROI_MinY(){return this->ROI_MinY;}
  int GetROI_MinX(){return this->ROI_MinX;}
  int GetROI_MaxZ(){return this->ROI_MaxZ;}
  int GetROI_MaxY(){return this->ROI_MaxY;}
  int GetROI_MaxX(){return this->ROI_MaxX;}

  int GetROI_LengthX(){return this->ROI_LengthX;}
  int GetROI_LengthY(){return this->ROI_LengthY;}
  int GetROI_LengthZ(){return this->ROI_LengthZ;}

  unsigned char* GetROI() {return this->ROI;}
  void ROI_Unassign() { this->ROI = NULL;}

  int  GetROIIncY() {return this->ROIIncY;}
  int  GetROIIncZ() {return this->ROIIncZ;}

  // Got rid of it bc I have my own instance of NumTotalTypeCLASS (NumberOfTotalTypeCLASS)
  int    GetNumberOfTotalTypeCLASS()   {return this->NumberOfTotalTypeCLASS;}

  int    GetNumClasses()          {return this->EMHierarchyParameters->NumClasses; } 
  int*   GetNumChildClasses()     {return this->EMHierarchyParameters->NumChildClasses; } 

  // They are differently defined from this->EMHierarchyParameter
  int    GetProbDataType()        {return this->EMHierarchyParameters->ProbDataType;}
  int*   GetProbDataIncY()        {return this->ProbDataIncY;}
  int*   GetProbDataIncZ()        {return this->ProbDataIncZ;}
  void   SetProbDataPtr(void* init, int i)  {this->ProbDataPtr[i] = init;}
  void** GetProbDataPtr()         {return this->ProbDataPtr;}
  
  float* GetProbDataMinusWeight() {return this->EMHierarchyParameters->ProbDataMinusWeight;}
  float* GetProbDataWeight ()     {return this->EMHierarchyParameters->ProbDataWeight;}

  int*   GetImage_Length() { return this->Image_Length;}
  float* GetPCAPara() { return this->PCAPara;}
  int    GetNumOfThreads() { return this->NumOfThreads;} 
  EMLocalShape_MultiThreadedParameters* GetMultiThreadedParameters() {return this->MultiThreadedParameters;}
  int    GetRegistrationType(){return this->RegistrationType;}
  char*  GetIncludeSpatialPriorForNormalization() { return this->IncludeSpatialPriorForNormalization;}
  void   SetROIExactVoxelCount(int* init) {this->ROIExactVoxelCount = init;} 

  void   SetNumberOfVoxelsInImage(int init) {this->NumberOfVoxelsInImage = init;}
  void   SpatialCostFunctionOn(); 
  float* GetSpatialCostFunction(){return this->SpatialCostFunction;} 
  void   SpatialCostFunctionOff();

  int    GetPCANumberOfEigenModes(int index) {return this->PCANumberOfEigenModes[index];}
  int    *GetPCANumberOfEigenModes() {return this->PCANumberOfEigenModes;}
 
  float  GetGaussianPenalty() {return this->GaussianPenalty;}
  float  GetImagePenalty() {return this->ImagePenalty;}

  float  GetImage_MidX() { return this->Image_MidX; } 
  float  GetImage_MidY() { return this->Image_MidY; } 
  float  GetImage_MidZ() { return this->Image_MidZ; } 

  float* Getweights(int index) {return this->weights[index];}
  int    GetweightsIncY() {return this->weightsIncY;}
  int    GetweightsIncZ() {return this->weightsIncZ;}

  float* GetPCAEigenVectorsPtr(int id, int index) {return this->PCAEigenVectorsPtr[id][index];}
  int    **GetPCAEigenVectorsIncY() {return this->PCAEigenVectorsIncY;}
  int    **GetPCAEigenVectorsIncZ() {return this->PCAEigenVectorsIncZ;}


  float* GetPCAMeanShapePtr(int index) {return this->PCAMeanShapePtr[index];}
  int*   GetPCAMeanShapeIncY() {return this->PCAMeanShapeIncY;}
  int*   GetPCAMeanShapeIncZ() {return this->PCAMeanShapeIncZ;}

  int    GetDataJump() {return this->DataJump;}
  int    GetDataIncY() {return this->DataIncY;}
  int    GetDataIncZ() {return this->DataIncZ;}

  // ------------------------
  // Utility Functions
  // ------------------------
  double Transfere_DistanceMap_Into_SpatialPrior(double distance, float variance, float boundary, float Min, float Max);


  // ------------------------
  // Public Variables
  // ------------------------

  // Variables set by vtkImageEMLocalSegment
  int   NumberOfTrainingSamples;
  int   PCAShapeModelType;  
  int    PCATotalNumOfShapeParameters;
  float  PCASpatialPriorVariance;

  // Variables defined by vtkImageEMLocalSuperClass
  float  **PCAInverseEigenValues;
  // You can say if each class has their own shape parameters defined or not
  float  *PCALogisticSlope;
  float  *PCALogisticBoundary;
  float  *PCALogisticMin;
  float  *PCALogisticMax;

  // Interface to registration model 
  float  **ClassToAtlasRotationMatrix;
  float  **ClassToAtlasTranslationVector; 

private:
  void   DefinePCADataPtr(int inPCAMinX, int inPCAMinY, int inPCAMinZ, int inBoundaryMaxX, int inBoundaryMaxY, int inDataIncY,  int inDataIncZ, int &outJump, int &outDataIncY,  int &outDataIncZ);

  EMLocal_Hierarchical_Class_Parameters *EMHierarchyParameters;
  EMLocalShape_MultiThreadedParameters *MultiThreadedParameters;
  int NumOfThreads;
  vtkMultiThreader *Threader; 
  float *PCAPara;

  int ROI_MaxZ;
  int ROI_MaxY; 
  int ROI_MaxX; 

  int ROI_MinZ; 
  int ROI_MinY;
  int ROI_MinX;  
 
  int ROI_LengthX;
  int ROI_LengthY;
  int ROI_LengthZ;

  // These are neeeded bc of registration - set by function
  float Image_MidX; 
  float Image_MidY;
  float Image_MidZ;
  int RegistrationType;
  int Image_Length[3];

  void **ProbDataPtr;
  int  *ProbDataIncY;
  int  *ProbDataIncZ;

  float  **PCAMeanShapePtr;
  int    *PCAMeanShapeIncY;
  int    *PCAMeanShapeIncZ;

  float ***PCAEigenVectorsPtr;
  int    **PCAEigenVectorsIncY;
  int    **PCAEigenVectorsIncZ;

  int    *PCANumberOfEigenModes;

  unsigned char* ROI;
  int ROIIncY;
  int ROIIncZ;
  int *ROIExactVoxelCount;
 
  float** weights;
  int weightsIncY;
  int weightsIncZ;

  char *IncludeSpatialPriorForNormalization;

  //Needed own instance of this variable bc EMHierarchyParameter could be deleted 
  // before this class is deleted 
  int NumberOfTotalTypeCLASS;

  int DataJump;
  int DataIncY;
  int DataIncZ;

  float GaussianPenalty;
  float ImagePenalty;

  // Needed for Print function
  int NumberOfVoxelsInImage;
  float *SpatialCostFunction;

};
#endif

