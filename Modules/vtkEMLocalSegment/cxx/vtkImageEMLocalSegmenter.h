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
// .NAME vtkImageEMLocalSegmenter
// Since 22-Apr-02 vtkImageEMLocal3DSegmenter is called vtkImageEMLocalSegmenter - Kilian
// EMLocal =  using EM Algorithm with Private Tissue Class Probability
#ifndef __vtkImageEMLocalSegmenter_h
#define __vtkImageEMLocalSegmenter_h 
  
#include "vtkImageEMGeneral.h" 
#include <vtkEMLocalSegmentConfigure.h> 
#include "vtkImageData.h"
#include "vtkImageEMSuperClass.h"

// Just for debugging purposes
#define EM_DEBUG 1

// Do you want to run the code with all the print outs (set 1) or not (0)
#define EMVERBOSE 0
// ---------------------------------------------------
// Kilian through out later 
# // Definition for PCA
#define EMSEGMENT_PCA_GREATER_MAX_DIST     1 
#define EMSEGMENT_PCA_EIGENVECTORS_ZERO    2


//BTX  
// Needed to parallelise the algorithm
typedef struct {
  int           id ;
  float         **w_m_input;
  unsigned char *MapVector;
  float         *cY_M; 
  int           imgX;
  int           imgY; 
  int           imgXY;
  int           StartVoxel;
  int           EndVoxel;
  int           NumClasses;
  int           NumTotalTypeCLASS;
  int*          NumChildClasses;
  int           NumInputImages;
  double        Alpha;
  double        ***MrfParams;
  void          ** ProbDataPtr;
  int           *ProbDataIncY; 
  int           *ProbDataIncZ; 
  float         *ProbDataWeight;
  float         *ProbDataMinusWeight;
  int           ProbDataType;
  double        **LogMu;
  double        ***InvLogCov;
  double        *InvSqrtDetLogCov;
  double        *TissueProbability;
  int           *VirtualNumInputImages;
  float         **w_m_output;
} vtkImageEMLocal_MF_Approximation_Work;

//ETX 
class VTK_EMLOCALSEGMENT_EXPORT vtkImageEMLocalSegmenter : public vtkImageEMGeneral
{
  public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImageEMLocalSegmenter *New();
  vtkTypeMacro(vtkImageEMLocalSegmenter,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  // -----------------------------------------------------
  // Setting Algorithm 
  // -----------------------------------------------------
  vtkSetMacro(NumEMShapeIter,int);
  vtkGetMacro(NumEMShapeIter,int);

  vtkSetMacro(NumIter, int);
  vtkGetMacro(NumIter, int);

  vtkSetMacro(NumRegIter, int);
  vtkGetMacro(NumRegIter, int);

  vtkSetMacro(Alpha, double);
  vtkGetMacro(Alpha, double);

  vtkSetMacro(SmoothingWidth, int);
  vtkGetMacro(SmoothingWidth, int);

  vtkSetMacro(SmoothingSigma, int);
  vtkGetMacro(SmoothingSigma, int);

  void SetNumberOfTrainingSamples(int Number) {this->NumberOfTrainingSamples = Number;}
  vtkGetMacro(NumberOfTrainingSamples, int);

  int* GetSegmentationBoundaryMin();
  int* GetSegmentationBoundaryMax();

  int* GetExtent(){return this->Extent;}

  // -----------------------------------------------------
  // Print functions 
  // -----------------------------------------------------
  vtkGetStringMacro(PrintDir);
  vtkSetStringMacro(PrintDir);

  // Description:
  // Number of input images for the segmentation - Has to be defined before defining any class specific setting 
  // Otherwise they get deleted
  // Be carefull: this is just the number of images not attlases, 
  // e.g. I have 5 tissue classes and 3 Inputs (T1, T2, SPGR) -> NumInputImages = 3
  void SetNumInputImages(int number);
  int GetNumInputImages() {return this->NumInputImages;} 

  void SetImageInput(int index, vtkImageData *image) {this->SetInput(index,image);}

  vtkGetMacro(ImageProd, int); 

  // Desciption:
  // Dimension of work area => SegmentationBoundaryMax[i] - SegmentationBoundaryMin[i] + 1 
  int GetDimensionX();
  int GetDimensionY();
  int GetDimensionZ();

  // -----------------------------------------------------
  // EM-MF Function 
  // -----------------------------------------------------  
  // Description:
  // Defines the Label map of a given image
  static void DetermineLabelMap(short *LabelMap, int NumTotalTypeCLASS, int* NumChildClasses, vtkImageEMSuperClass* head,  short* ROI,int ImageMax, float **w_m);

  // Desciption:
  // Special function for parallelise MF part -> Creating Threads 
  int MF_Approx_Workpile(float **w_m_input,unsigned char* MapVector, float *cY_M, int imgXY,double ***InvLogCov,
                         double *InvSqrtDetLogCov, int NumTotalTypeCLASS, int* NumChildClasses, int NumClasses, void** ProbDataPtr, 
                         int* ProbDataIncY, int* ProbDataIncZ, float* ProbDataWeight, float *ProbDataMinusWeight, double** LogMu, 
                         double* TissueProbability, int *VirtualNumInputImages, vtkImageEMSuperClass* head, float **_m_output);

  // Description:
  // Print out intermediate result of the algorithm in a  file
  // The file is called  this->PrintIntermediateDir/EM*.m
  void PrintIntermediateResultsToFile(int iter, float **w_m, short *ROI, unsigned char* OutputVector, int NumTotalTypeCLASS, int* NumChildClasses, 
                      vtkImageEMSuperClass* actSupCl, char* LevelName, void **ClassList, classType *ClassListType, int* LabelList, 
                      FILE** QualityFile);

  // -----------------------------------------------------
  // Intensity Correction 
  // -----------------------------------------------------

  // Sets the class given as intensity class 
  void SetIntensityAvgClass(vtkImageEMClass *init) {this->IntensityAvgClass = init;}

  //BTX
  vtkImageEMClass* GetIntensityAvgClass() {return this->IntensityAvgClass;}
  //ETX  
  double GetIntensityAvgValuePreDef(int index) {return this->IntensityAvgValuePreDef[index];}
  void SetIntensityAvgValuePreDef(double value, int index);

  double GetIntensityAvgValueCurrent(int index) {return this->IntensityAvgValueCurrent[index];}
  void SetIntensityAvgValueCurrent(double value, int index) {this->IntensityAvgValueCurrent[index] = value;}
 
  //BTX
  vtkImageEMSuperClass* GetActiveSuperClass() {return this->activeSuperClass;}
  vtkImageEMSuperClass* GetHeadClass() {return this->HeadClass;}
  //ETX
  int HierarchicalSegmentation(vtkImageEMSuperClass* head, float** InputVector,short *ROI, short *OutputVector, EMTriVolume & iv_m, EMVolume *r_m, char* LevelName);

  //Kilian rewrite it 
  void PrintSuperClass () {
    printf("Current: %f Prob: %f Label: %d Parent: %f \n", activeSuperClass, activeSuperClass->GetTissueProbability(), activeSuperClass->GetLabel(), activeSuperClass->GetParentClass());
  } 

  // Desciption:
  // Head Class is the inital class under which all subclasses are attached  
  void SetHeadClass(vtkImageEMSuperClass *InitHead) {
    InitHead->Update(); 
    if (InitHead->GetErrorFlag()) {
      // This is done before this->Update() so we cannot use Error Message Report;
      vtkErrorMacro(<<"Cannot set HeadClass because the class given has its ErrorFlag activated !");
      return;
    }
    this->HeadClass   = InitHead;
    this->activeClass = (void*) InitHead;
    this->activeClassType  = SUPERCLASS;
  }

  // =============================
  // For Message Protocol
  char* GetErrorMessages() {return this->ErrorMessage.GetMessages(); }
  int GetErrorFlag() {return  this->ErrorMessage.GetFlag();}
  void ResetErrorMessage() {this->ErrorMessage.ResetParameters();}
  // So we can also enter streams for functions outside vtk
  ProtocolMessages* GetErrorMessagePtr(){return &this->ErrorMessage;}

  char* GetWarningMessages() {return this->WarningMessage.GetMessages(); }
  int GetWarningFlag() {return  this->WarningMessage.GetFlag();}
  void ResetWarningMessage() {this->WarningMessage.ResetParameters();}
  ProtocolMessages* GetWarningMessagePtr(){return &this->WarningMessage;}
  //ETX
protected:
  vtkImageEMLocalSegmenter();
  vtkImageEMLocalSegmenter(const vtkImageEMLocalSegmenter&) {};
  ~vtkImageEMLocalSegmenter(); 
  void DeleteVariables();

  void operator=(const vtkImageEMLocalSegmenter&) {};
  void ExecuteData(vtkDataObject *);   

  //BTX
  // Description:
  // Checks all intput image if they have coresponding dimensions 
  int CheckInputImage(vtkImageData * inData,int DataTypeOrig, float DataSpacingOrig[3], int num);

  // Description:
  // Resets the error flag and messages 
  void ResetMessageSettings();  

  int NumEMShapeIter; // How many times should the iteration between EM and Shape be repeated 
  int NumIter;         // Number of EM-iterations
  int NumRegIter;      // Number of iteration in E- Step to regularize weights 
  double Alpha;        // alpha - Paramter 0<= alpaha <= 1

  int SmoothingWidth;  // Width for Gausian to regularize weights   
  int SmoothingSigma;  // Sigma paramter for regularizing Gaussian

  int NumInputImages;               // Number of input images  
 
  // These are defined in vtkEMImagePrivateSegment
  char* PrintDir;        // In which directory should the results be printed  

  // New Variables for Local Version 
  int ImageProd;                   // Size of Image = DimensionX*DimensionY*DimensionZ

  int Extent[6];                  // Extent of images - needed for several inputs 

  int NumberOfTrainingSamples;    // Number of Training Samples Probability Image has been summed up over !  

  //BTX
  vtkImageEMClass *IntensityAvgClass;          // Label of Tissue class for which Intensity value is defined, = -1 => no intensity correction

  //ETX 
  double* IntensityAvgValuePreDef;  // Average intensity gray value (not log gray value) for the class and every input channel - this is predefined
  double* IntensityAvgValueCurrent; // Average intensity gray value (not log gray value) of the current image 
  //BTX

  vtkImageEMSuperClass *activeSuperClass;   // Currently Active Super Class -> Important for interface with TCL
  classType    activeClassType;

  vtkImageEMSuperClass *HeadClass;          // Initial Class
  //ETX 
  void *activeClass;               // Currently Active Class -> Important for interface with TCL

  short**  DebugImage;             // Just used for debuging

  ProtocolMessages ErrorMessage;    // Lists all the error messges -> allows them to be displayed in tcl too 
  ProtocolMessages WarningMessage;  // Lists all the warning messges -> allows them to be displayed in tcl too 

};
#endif











