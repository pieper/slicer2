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
// EMLocal =  using EM Algorithm with Local Tissue Class Probability
#ifndef __vtkImageEMLocalSegmenter_h
#define __vtkImageEMLocalSegmenter_h 

#include "vtkImageEMGeneral.h"
//BTX
// Needed to parallelise the algorithm
typedef struct {
  int           id ;
  double        *w_m_input;
  unsigned char *MapVector;
  float         *cY_M;
  int           imgX;
  int           imgY;
  int           imgXY;
  int           StartVoxel;
  int           EndVoxel;
  int           NumClasses;
  int           NumInputImages;
  double        Alpha;
  double        ***MrfParams;
  void          ** ProbDataPtr;
  int           *ProbDataIncY; 
  int           *ProbDataIncZ; 
  int           ProbDataType;
  int*          ProbDataLocal;
  double        **LogMu;
  double        ***InvLogCov;
  double        *InvSqrtDetLogCov;
  double        *TissueProbability;
  double        *w_m_output;
} MF_Approximation_Work;

//ETX
// End of parallel specific implementation
 
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

  vtkSetMacro(StartSlice, int);
  vtkGetMacro(StartSlice, int);

  vtkSetMacro(EndSlice, int);
  vtkGetMacro(EndSlice, int);

  // Description:
  // PrintIntermediateResults = 1 => Prints out intermediate result of 
  // the algorithm in a matlab file. The file is called  EMLocalSegmResult<iteration>.m  
  vtkSetMacro(PrintIntermediateResults, int); 
  vtkGetMacro(PrintIntermediateResults, int); 

  // Description:
  // Print out the result of which slice 
  vtkSetMacro(PrintIntermediateSlice, int); 
  vtkGetMacro(PrintIntermediateSlice, int); 

  // Description:
  // Print out the result after how many iteration steps
  vtkSetMacro(PrintIntermediateFrequency, int); 
  vtkGetMacro(PrintIntermediateFrequency, int); 

  // Description:
  // Changing NumClasses re-defines also all the arrays which depon the number of classes e.g. prob
  void SetNumClasses(int NumberOfClasses);
  vtkGetMacro(NumClasses, int);

  // Description:
  // Number of input images for the segmentation - Has to be defined before Mu, Sigma and NumClasses
  // Be carefull: this is just the number of images not attlases, 
  // e.g. I have 5 tissue classes and 3 Inputs (T1, T2, SPGR) -> NumInputImages = 3
  void SetNumInputImages(int number);
  vtkGetMacro(NumInputImages, int);

  // -----------------------------------------------------
  // Class Function
  // -----------------------------------------------------
  void SetTissueProbability(double value, int index);

  void SetInputIndex(int index, vtkImageData *image) {this->SetInput(index,image);}

  // Description:
  // Mu is now of dimesion (Number of Tissue Classes) x (Number of Input Images) 
  void SetLogMu(double mu, int y,int x);

  // Description:
  // Sigma is now of dimesion (Number of Tissue Classes) x (Number of Input Images) x (Number of Input Images) 
  // This is the Coveriance Matrix for our Tissue Class Gauss distribution
  // Be careful: the Matrix must be regluar, also we use the inverse of the squarred cov matrix and the 
  // determinand of the coveriance matrix for the Gauss Calculations => 
  // gaus[i] = 1/(sqrt(2*pi)det(Sigma[i]))^NumInputImages * exp((x-mu[i])'* Inverse(Sigma[i]^2) *(x-mu[i]))    
  void SetLogCovariance(double value, int z, int y,int x);

  void SetLabel(int label, int index);
  int GetLabel(int index) {return this->Label[index-1];}

  // Description:
  // Does the class use a local prior map or not 
  void SetProbDataLocal(int value, int index);

  // Description:
  // Defines the tissue class interaction matrix MrfParams(k,i,j)
  // where k = direction [1...6], 
  //       i = class of the current pixel t+1 
  //       j = class of neighbouring pixel t 
  void SetMarkovMatrix(double value, int j, int i, int k);

  void SetProbDataPtr(void *ptr,int NumClass) { this->ProbDataPtr[NumClass] = ptr;} 

  // -----------------------------------------------------
  // Input Image
  // -----------------------------------------------------
  vtkGetMacro(ImageMaxZ, int);

  vtkGetMacro(ImageMaxY, int);

  vtkGetMacro(ImageMaxX, int);

  vtkGetMacro(ImageProd, int); 

  // Description:
  // Writes data into a MRI file 
  // Writes an array of shorts to a MRI File - good for debugging
  void WriteShortDataToMRIFile(char* filename, short* data, int StartSlice, int EndSlice,int IncY,int IncZ);
  void WriteDoubleDataToMRIFile(char* filename, double* data, int StartSlice, int EndSlice,int IncY,int IncZ);

  // -----------------------------------------------------
  // EM-MF Function 
  // -----------------------------------------------------  
  // Description:
  // Defines the Label map of a given image
  void DeterminLabelMap(unsigned char *LabelMap, double * w_m, int imgXY,int imgZ);

  // Desciption:
  // Special function for parallelise MF part -> Creating Threads 
  int MF_Approx_Workpile(double * w_m_input,unsigned char* MapVector, float *cY_M, int imgXY,double ***LogCov,double *InvSqrtDetLogCov,double *w_m_output);

  // Description:
  // Print out intermediate result of the algorithm in a matlab file
  // The file is called  EMLocalSegmResult<iteration>.m
  void PrintMatlabGraphResults(int iter,int slice,int FullProgram,int imgXY, double * w_m);

  // Description:
  // Before EM Algorithm is started it checks of all values are set correctly
  int checkValues(); 

  // -----------------------------------------------------
  // Special Function that should not be tickelt, bc 
  // they return pointers - tcl cannot deal with it 
  // -----------------------------------------------------  
//BTX
  void**    GetProbDataPtr() {return this->ProbDataPtr;}
  int*      GetProbDataIncZ() {return this->ProbDataIncZ;}
  int*      GetProbDataIncY() {return this->ProbDataIncY;}
  int*      GetProbDataLocal() {return this->ProbDataLocal;}
  double*   GetTissueProbability() {return this->TissueProbability;}
  double**  GetLogMu() {return this->LogMu;}
  double*** GetLogCovariance() {return this->LogCovariance;}
//ETX

  // -----------------------------------------------------
  // Intensity Correction 
  // -----------------------------------------------------
  
  vtkGetMacro(IntensityAvgClass,int);   
  vtkSetMacro(IntensityAvgClass,int);   
  double GetIntensityAvgValuePreDef(int index) {return this->IntensityAvgValuePreDef[index-1];}
  void SetIntensityAvgValuePreDef(double value, int index);

  double GetIntensityAvgValueCurrent(int index) {return this->IntensityAvgValueCurrent[index-1];}
  //BTX
  // Not for tcl use - 0 <index < NumInputImages
  void SetIntensityAvgValueCurrent(double value, int index) {this->IntensityAvgValueCurrent[index] = value;}
  //ETX

  // -----------------------------------------------------
  // Bias 
  // -----------------------------------------------------
  vtkGetStringMacro(BiasRootFileName);
  vtkSetStringMacro(BiasRootFileName);
  vtkGetMacro(BiasPrint, int);
  vtkSetMacro(BiasPrint, int);

  vtkGetMacro(ErrorFlag, int);
  char* GetErrorMessages(); 

protected:
  vtkImageEMLocalSegmenter();
  vtkImageEMLocalSegmenter(const vtkImageEMLocalSegmenter&) {};
  ~vtkImageEMLocalSegmenter();

  void DeleteVariables();

  void operator=(const vtkImageEMLocalSegmenter&) {};
  void ExecuteData(vtkDataObject *);

  // Description:
  // Checks all intput image if they have coresponding dimensions 
  bool CheckInputImage(vtkImageData * inData,int DataTypeOrig, int DataTypeNew, int num,int *outExt);

  // Description:
  // Resets the error flag and messages 
  void ResetErrorSettings();  

  int NumClasses;      // Number of Classes
  int NumIter;         // Number of EM-iterations
  int NumRegIter;      // Number of iteration in E- Step to regularize weights 
  double Alpha;        // alpha - Paramter 0<= alpaha <= 1

  int SmoothingWidth;  // Width for Gausian to regularize weights   
  int SmoothingSigma;  // Sigma paramter for regularizing Gaussian

  int StartSlice;      // First Slide to be segmented
  int EndSlice;        // Last Slide to be segmented 
 
  int PrintIntermediateResults;    //  Print intermediate results in a Matlab File (No = 0, Yes = 1)
  int PrintIntermediateSlice;      //  Print out the result of which slide 
  int PrintIntermediateFrequency;  //  Print out the result after how many steps 

  int NumInputImages;              // Number of input images for the segmentation - Has to be defined before Mu, Sigma and NumClasses
                                   // Be carefull: this is just the number of images not attlases, 
                                   // e.g. I have 5 tissue classes and 3 Inputs (T1, T2, SPGR) -> NumInputImages = 3
  double **LogMu;                 // Intensity distribution of the classes (changed for Multi Dim Version)
  double ***LogCovariance;        // Intensity distribution of the classes (changed for Multi Dim Version) -> This is the Coveriance Matrix
                                 // Be careful: the Matrix must be symmetric and positiv definite,
  int *Label;                    // Prior Probability of the classes
  double ***MrfParams;           // Markov Model Parameters: Matrix3D mrfparams(this->NumClasses,this->NumClasses,4);
  double *TissueProbability;     // Global Tissue Probability

  // New Variables for Local Version 
  int ImageMaxZ;                  // MaxZ = EndSlice - StartSlice + 1;  
  int ImageMaxY;                  // Size of volume in Y direction (normally 256 or 512)
  int ImageMaxX;                  // Size of volume in X direction (normally 256 or 512)
  int ImageProd;                  // Size of Image = ImageMaxX*ImageMaxY*ImageMaxZ

  int NumberOfTrainingSamples;    // Number of Training Samples Probability Image has been summed up over !  
  int   *ProbDataIncY;            // Increments for probability data in Y Dimension 
  int   *ProbDataIncZ;            // Increments for probability data in Z Dimension
  void **ProbDataPtr;             // Pointer to Probability Data 
  int   *ProbDataLocal;          // Do we use the local Prior or not for every class individual 

  // New Variables for Intensity corecetion
  int IntensityAvgClass;          // Label of Tissue class for which Intensity value is defined, = -1 => no intensity correction 
  double* IntensityAvgValuePreDef;  // Average intensity gray value (not log gray value) for the class and every input channel - this is predefined
  double* IntensityAvgValueCurrent; // Average intensity gray value (not log gray value) of the current image 
  // Bias Field Print Out Variables
  char* BiasRootFileName;
  int BiasPrint;

  vtkOStrStreamWrapper *ErrorMessage;
  int ErrorFlag;

};
#endif











