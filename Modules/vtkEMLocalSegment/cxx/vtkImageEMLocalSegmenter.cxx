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
// Since 22-Apr-02 vtkImageEMLocal3DSegmenter is called vtkImageEMLocalSegmenter - Kilian
// EMLocal =  using EM Algorithm with Local Tissue Class Probability

#include "vtkImageEMLocalSegmenter.h"
#include "vtkObjectFactory.h"
// This has always to be defined when using vtkDataTimeDef . Do not ask me why
extern "C" {
  struct timeval preciseTimeEnd;
  struct timeval preciseTimeStart;
}

//------------------------------------------------------------------------------
vtkImageEMLocalSegmenter* vtkImageEMLocalSegmenter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMLocalSegmenter");
  if(ret)
  {
    return (vtkImageEMLocalSegmenter*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMLocalSegmenter;
}

//----------------------------------------------------------------------------
vtkImageEMLocalSegmenter::vtkImageEMLocalSegmenter()
{
  this->NumInputImages = 0;   // Number of input images for the segmentation - Has to be defined before Mu, Sigma and NumClasses
  this->NumClasses = 0;       // Number of Classes - has to be set to 0 bc currently nothing is set
  this->NumIter = 1;          // Number of EM-MRF Iterations
  this->NumRegIter = 1;       // Number of Regularizing Iterations
  this->Alpha = 0.7;          // 0 <= alpha <= 1. Be carefull - has great influence over outcome 
  this->SmoothingWidth = 11;  // Width for Gausian to regularize weights
  this->SmoothingSigma = 5;   // Sigma paramter for regularizing Gaussian
  this->StartSlice = 1;       // First Slide to be segmented
  this->EndSlice = 1;         // Last Slide to be segmented  
  this->PrintIntermediateResults = 0 ;   // Print intermediate results in a Matlab File (No = 0, Yes = 1)
  this->PrintIntermediateSlice = 1 ;     // Print out the result of which slice  
  this->PrintIntermediateFrequency = 1 ; // Print out the result after how many steps

  this->NumberOfTrainingSamples = 0;     // Number of Training Samples Probability Image has been summed up over !
  this->ImageMaxZ = 0;                   // MaxZ = EndSlice - StartSlice + 1;  
  this->ImageMaxY = 0;                   // Size of volume in Y direction (normally 256 or 512)
  this->ImageMaxX = 0;                   // Size of volume in X direction (normally 256 or 512)
  this->ImageProd = 0;                   // Size of Image = maxX*maxY*maxZ

  this->IntensityAvgClass = -1;          // Label of Tissue class for which Intensity value is defined, = -1 => no intensity correction 
  this->BiasPrint = 0;                   // Should the bias field be printed or not

  this->LogMu = NULL;this->LogCovariance = NULL;this->Label = NULL;this->TissueProbability = NULL;this->MrfParams = NULL;
  this->ProbDataPtr = NULL;this->ProbDataIncY = NULL; this->ProbDataIncZ = NULL; this->ProbDataLocal =NULL; this->IntensityAvgValuePreDef = NULL;
  this->IntensityAvgValueCurrent = NULL; this->BiasRootFileName = NULL;
}

vtkImageEMLocalSegmenter::~vtkImageEMLocalSegmenter(){
  this->DeleteVariables();
}

//----------------------------------------------------------------------------
void vtkImageEMLocalSegmenter::PrintSelf(ostream& os)
{
}

//------------------------------------------------------------------------------
template <class Tin, class Tprob>
static double vtkImageEMLocalSegmenterCalculateIntensityCorrection(vtkImageEMLocalSegmenter* self, Tin *in1Ptr, Tprob **ProbDataPtr, int InputIndex,int inIncY, int inIncZ) {
  int label = self->GetIntensityAvgClass();
  // Intensity correction is disabled !
  if (label < 0) return 0.0;
  int tissue = -1;
  int z = 1,y,x ;
  double result;
  // Debugging
  while ((tissue == -1) && (z <= self->GetNumClasses())) {
    if (label == self->GetLabel(z)) tissue = z-1;
    z++;
  }
  if (tissue == -1) {
    cout << "vtkImageEMLocalSegmenterCalculateIntensityCorrection::Warning: Label definition for IntensityAvgClass ("<< label <<") does not match any class label. Intensity correction is deactivated !" << endl;
    return 0.0; 
  }
  if ((self->GetProbDataLocal())[tissue] == 0) {
    cout << "vtkImageEMLocalSegmenterCalculateIntensityCorrection::Warning:No probability data map defined for class "<<  tissue+1 << ". Intensity correction is deactivated !"<< endl;
    return 0.0;
  }

  int ProbDataIncY   = (self->GetProbDataIncY())[tissue];
  int ProbDataIncZ   = (self->GetProbDataIncZ())[tissue];
  int ImageMaxZ      = self->GetImageMaxZ();
  int ImageMaxY      = self->GetImageMaxY();
  int ImageMaxX      = self->GetImageMaxX();
  int SampleCount   = 0;
  double SampleValue = 0.0;
  int border = int(0.95 * double(self->GetNumberOfTrainingSamples()));
  Tprob* ProbDataCopyPtr = ProbDataPtr[tissue];
  for (z=0; z < ImageMaxZ; z++) {
    for (y=0; y < ImageMaxY; y++) {
      for (x=0; x < ImageMaxX; x++) {
    if (int(*ProbDataCopyPtr) > border) {
      SampleCount ++;
      SampleValue += double(* in1Ptr);
    }
    ProbDataCopyPtr ++;
    in1Ptr ++;
      }
      in1Ptr += inIncY;
      ProbDataCopyPtr += ProbDataIncY;
    }
    in1Ptr += inIncZ;
    ProbDataCopyPtr += ProbDataIncZ;
  }
  ProbDataCopyPtr = ProbDataPtr[tissue];

  if (SampleCount) {
    result = SampleValue /double(SampleCount);
    // printf("IntensityCorrection:: Input: %d Samples: %d Average: %.4f",InputIndex,SampleCount, result); 
    cout << "IntensityCorrection:: Input: "<< InputIndex << " Samples: " << SampleCount << " Average: "<< result; 
    self->SetIntensityAvgValueCurrent(result,InputIndex); 
    if (self->GetIntensityAvgValuePreDef(InputIndex+1) < 0.0) {
      cout << endl;
        return 0.0;
    }
    result -= self->GetIntensityAvgValuePreDef(InputIndex+1);
    if (fabs(result) < 1e-3) result = 0.0;
    cout << " Delta Correction: "<< result << endl;
    return result;
  }
  cout << "vtkImageEMLocalSegmenterCalculateIntensityCorrection::Warning:No samples could be taken for intensity correction for image " << InputIndex +1 <<".  Intensity correction is deactivated !"<< endl;
  return 0.0;
}


void vtkImageEMLocalSegmenter::WriteShortDataToMRIFile(char* filename, short* data, int StartSlice, int EndSlice,int IncY,int IncZ) { 
  int JumpPerSlice = (this->ImageMaxX + IncY)*this->ImageMaxY + IncZ;
  int NumPixelPerSlice = this->ImageMaxX*this->ImageMaxY;
  int i;
  char *NewFileName = new char[strlen(filename) + 5];
  vtkFileOps write; 
  cout << "vtkImageEMLocalSegmenter::WriteDataToMRIFile: Writing File "<< filename <<".* !" <<endl;  
  data +=  JumpPerSlice * StartSlice;  
  for (i = StartSlice; i<= EndSlice; i++) { 
    sprintf(NewFileName,"%s.%03d",filename,i);
    // Currently we have no header
    write.WriteMRIfile(NewFileName, NULL,0,data, NumPixelPerSlice);
    if (i < EndSlice) data += JumpPerSlice;
  }
  delete []NewFileName;
}

void vtkImageEMLocalSegmenter::WriteDoubleDataToMRIFile(char* filename, double* data, int StartSlice, int EndSlice,int IncY,int IncZ) { 
  int JumpPerSlice = (this->ImageMaxX + IncY)*this->ImageMaxY + IncZ;
  int NumPixelPerSlice = this->ImageMaxX*this->ImageMaxY;
  int i;
  char *NewFileName = new char[strlen(filename) + 5];
  vtkFileOps write; 
  cout << "vtkImageEMLocalSegmenter::WriteDoubleDataToMRIFile: Writing File "<< filename <<".* !" <<endl;  
  data +=  JumpPerSlice * StartSlice;  
  for (i = StartSlice; i<= EndSlice; i++) { 
    sprintf(NewFileName,"%s.%03d",filename,i);
    // Currently we have no header
    write.WriteMRIfile(NewFileName, data, NumPixelPerSlice);
    if (i < EndSlice) data += JumpPerSlice;
  }
  delete []NewFileName;
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageEMLocalSegmenterReadInputChannel(vtkImageEMLocalSegmenter *self,vtkImageData *in1Data, T *in1Ptr,int inExt[6],int rowLength,int maxY,int StartEndSlice,double ** InputVector,int InputIndex)
{

  int idxR, idxY, idxZ;
  int inIncX, inIncY, inIncZ;
  int maxXY = rowLength*maxY;;
  int StartSlice = self->GetStartSlice();
  int index = 0; 
  double IntensityCorrection;

  // Get increments to march through data 
  in1Data->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  // We assume here input is 1 scalar -> it is checked in CheckInputImages

  //2.)  Define InputVector by reading the log gray values of the image
  in1Ptr += ((rowLength+inIncY)*maxY + inIncZ)*(StartSlice -1); 

  switch (self->GetInput(0)->GetScalarType()) {
    case VTK_DOUBLE:         IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(double**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break;
    case VTK_FLOAT:          IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(float**) self->GetProbDataPtr(), InputIndex, inIncY, inIncZ); break; 
    case VTK_LONG:           IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(long**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_UNSIGNED_LONG:  IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(unsigned long**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_INT:            IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(int**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_UNSIGNED_INT:   IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(unsigned int**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_SHORT:          IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(short**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_UNSIGNED_SHORT: IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(unsigned short**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_CHAR:           IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(char**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break; 
    case VTK_UNSIGNED_CHAR:  IntensityCorrection = vtkImageEMLocalSegmenterCalculateIntensityCorrection(self,in1Ptr,(unsigned char**) self->GetProbDataPtr(),InputIndex, inIncY, inIncZ); break;
  default:
    cout << "vtkImageEMLocalSegmenterReadInputChannel: Unknown ScalarType" << endl;
    return;
  } 
  

  for (idxZ = 0; idxZ < StartEndSlice ; idxZ++) { 
    for (idxY = maxY-1; idxY > -1; idxY--) {
      for (idxR = 0; idxR < rowLength; idxR++) {
    if (double(* in1Ptr) >  IntensityCorrection) {
      InputVector[index][InputIndex] = log(double(* in1Ptr) +1 - IntensityCorrection);
    } else {
      InputVector[index][InputIndex] = 0.0;
    }
    index ++; 
    in1Ptr++;
      }
      in1Ptr += inIncY;
    }
    in1Ptr += inIncZ;
  }
}



//----------------------------------------------------------------------------
template <class T>
static void vtkImageEMLocalSegmenterIncrementProbDataPointer(vtkImageEMLocalSegmenter *self,int NumClass, T *inProbDataPtr, int jump) {
  self->SetProbDataPtr((void*) (inProbDataPtr + jump),NumClass);
}

//------------------------------------------------------------------------------
void vtkImageEMLocalSegmenter::SetNumInputImages(int number) {
  if (this->NumClasses > 0) {
    vtkErrorMacro(<< "Number of Input Images cannot be changed anymore after Number of Tissue classes have been set !");
    return;
  }
  this->NumInputImages = number;
}


//------------------------------------------------------------------------------
bool vtkImageEMLocalSegmenter::CheckInputImage(vtkImageData * inData,int DataTypeOrig, int DataTypeNew, int num, int outExt[6]) {
  // Check if InData is defined 
  int inExt[6];
  if (inData == 0) {
    vtkErrorMacro(<< "Probability Map for class "<< num << " must be specified.");
    return false;
  }

  // Check for Data Type if correct : Remember ProbabilityData all has to be of the same data Type
  if (DataTypeOrig != DataTypeNew) {
    vtkErrorMacro(<< "Probability Map for class "<< num << " has wrong data type - not conform with others.");
    return false;
  }

  // Check if it is one of the registered types
  if ((DataTypeNew != VTK_DOUBLE) && (DataTypeNew != VTK_FLOAT) && 
      (DataTypeNew != VTK_LONG) && (DataTypeNew != VTK_UNSIGNED_LONG) && (DataTypeNew != VTK_INT) && (DataTypeNew != VTK_UNSIGNED_INT) && 
      (DataTypeNew != VTK_SHORT) && (DataTypeNew != VTK_UNSIGNED_SHORT) && (DataTypeNew != VTK_CHAR) && (DataTypeNew != VTK_UNSIGNED_CHAR)) return false;

  // Check for dimenstion of InData
  // Could be easily fixed if needed 
  inData->GetWholeExtent(inExt);
  if ((inExt[1] != outExt[1]) || (inExt[0] != outExt[0]) || (inExt[3] != outExt[3]) || (inExt[2] != outExt[2]) || (inExt[5] != outExt[5]) || (inExt[4] != outExt[4])) {
    vtkErrorMacro(<< "Execute: Extension of Input Image " << num << ", " << inExt[0] << "," << inExt[1] << "," << inExt[2] << "," << inExt[3] << "," << inExt[4] << "," << inExt[5] 
                  << "is not alligned with output image "  << outExt[0] << "," << outExt[1] << "," << outExt[2] << "," << outExt[3] << "," << outExt[4] << " " << outExt[5]);
    return false;
  }
  if (inData->GetNumberOfScalarComponents() != 1) {
    vtkErrorMacro(<< "Execute: This filter assumes input to filter is defined with one scalar component. " << num  << " has " << inData->GetNumberOfScalarComponents() 
                  << " Can be easily changed !");
    return false;
  }
  // Kilian Check for orientation has to be done in TCL !!!!
  return true;
}

//----------------------------------------------------------------------------
// We need to create a special SetNumClass, bc whenver NumClasses are set, we 
// also have to set arrays which depend on the number of classes e.g. prob
//----------------------------------------------------------------------------
void vtkImageEMLocalSegmenter::SetNumClasses(int NumberOfClasses)
{
  int z,y,x;
  if (this->NumInputImages < 1) {
    vtkErrorMacro(<< "Number of Input Images has to be defined before setting the number of classes");
    return;
  }

  if (this->NumClasses == NumberOfClasses) return;

  this->DeleteVariables();
  if (NumberOfClasses > 0 ) {
    // Create new space for variables
    this->LogMu         = new double*[NumberOfClasses];
    this->LogCovariance = new double**[NumberOfClasses];
    for (z=0; z < NumberOfClasses; z++) {
      this->LogMu[z]         = new double[this->NumInputImages];
      this->LogCovariance[z] = new double*[this->NumInputImages];
      for (y=0; y < this->NumInputImages ; y++) this->LogCovariance[z][y] = new double[this->NumInputImages];
    } 

    this->Label             = new int[NumberOfClasses];
    this->TissueProbability = new double[NumberOfClasses];
    this->ProbDataLocal     = new int[NumberOfClasses];
    this->ProbDataPtr       = new void*[NumberOfClasses];
    this->ProbDataIncY      = new int[NumberOfClasses];
    this->ProbDataIncZ      = new int[NumberOfClasses];

    this->IntensityAvgValuePreDef = new double[this->NumInputImages];
    this->IntensityAvgValueCurrent = new double[this->NumInputImages];

    this->MrfParams = new double**[6];
    for (z=0; z < 6; z++) {
      this->MrfParams[z] = new double*[NumberOfClasses];
      for (y=0;y < NumberOfClasses; y ++) 
    this->MrfParams[z][y] = new double[NumberOfClasses];
    }
    for (z= 0; z < NumberOfClasses; z++) {
      this->Label[z] = -1;
      this->TissueProbability[z] = -1;
      this->ProbDataPtr[z] = NULL;
      this->ProbDataIncY[z] = -1;
      this->ProbDataIncZ[z] = -1;

      for (y= 0; y <this->NumInputImages; y++) {
    this->LogMu[z][y] = -1;
    memset(this->LogCovariance[z][y], 0,this->NumInputImages*sizeof(double));
      }
    }

    for (z=0; z <this->NumInputImages; z++) {  
      this->IntensityAvgValuePreDef[z] = -1.0;
      this->IntensityAvgValueCurrent[z] = -1.0;
    }

    for (z=0; z < 6; z++) {
      for (y=0;y < NumberOfClasses; y ++) memset(this->MrfParams[z][y], 0,NumberOfClasses*sizeof(double)); 
    }
  } else {
    this->LogMu = NULL;this->LogCovariance = NULL;this->TissueProbability = NULL;this->Label = NULL; this->MrfParams = NULL;this->ProbDataLocal;
    this->ProbDataPtr = NULL; this->ProbDataIncY = NULL; this->ProbDataIncZ = NULL;this->IntensityAvgValuePreDef = NULL;this->IntensityAvgValueCurrent = NULL;
  }
  this->NumClasses = NumberOfClasses;
}

//---------------------------------------------------------------------
// Checks of all paramters are declared  
// 1  = eveything correct
// -1 = Dimension of Volume not correct
// -2 = Not all Mu defined correctly
// -3 = Not all Covariance defined correctly
// -4 = Not all Prob defined correctly
// -5 = Not all MrfParams defined correctly
// -6 = Not all Classes have labels defined

int vtkImageEMLocalSegmenter::checkValues()
{
  int i,k,j;
  for (i=0;i < this->NumClasses; i++) {
    if (this->Label[i] < 0) {
      cout << "vtkImageEMLocalSegmenter:checkValues:  Color[" << i+1 <<"] = " << this->Label[i] << " is not defined" << endl;
      return -6;
    } 
    if (this->TissueProbability[i] < 0) {
      cout << "vtkImageEMLocalSegmenter:checkValues:  TissueProbability[" << i+1 <<"] = " << this->TissueProbability[i] << " is not defined" << endl;
      return -7;
    } 
    for (j = 0; j < this->NumInputImages; j++) {
      if (this->LogMu[i][j] < 0) {
    cout << "vtkImageEMLocalSegmenter:checkValues:  Mu[" << i+1<<"][" << j+1 <<"] = " << this->LogMu[i][j] << " must be greater than 0!" << endl;
    return -2;
      }
    }

    // Check if matrix is symmetric
    for (j=0; j< this->NumInputImages; j++) {
      for (k=j+1; k <   this->NumInputImages; k++) 
    if (this->LogCovariance[i][j][k] != this->LogCovariance[i][k][j]) {
       cout << "vtkImageEMLocalSegmenter:checkValues:  Covariance must be syymetric for tissue class " << i+1 << endl;
      return -3;
    }
    }
    // Check if the matrix is positiv - Jakobian is devilish it changes the matrix this->LogCovariance[i]
    //if (vtkMath::JacobiN(this->LogCovariance[i],this->NumInputImages,eigval, eigvec) == 0) {
    //        cout << "vtkImageEMLocalSegmenter:checkValues:  Cannot calculate Eigenvalues for Covariance for tissue class " << i+1 << ". Check Matrix !"<< endl;
    //      return -3;
    // }
    //if (eigval[this->NumInputImages-1] < 1e-5) {
    //      cout << "vtkImageEMLocalSegmenter:checkValues:  Covariance for tissue class " << i+1 << "is a singular matrix, almost singular (must be regular) or not positiv definit!"<< endl;
    //      return -3;
    // }
 
    for (j = 0; j < this->NumClasses; j++) {
       for (k = 0; k < 6; k++) {
     if ((this->MrfParams[k][j][i] < 0) || (this->MrfParams[k][j][i] > 1)) {
       cout << "vtkImageEMLocalSegmenter:checkValues:  MrfParams[" << k+1 <<"] [" << j+1 <<"] [" << i+1 <<"] = " << this->MrfParams[k][j][i] 
        << " is not between 0 and 1!" << endl;
       return -5;
     }
       }
    }
  }
  return 1;
}

//----------------------------------------------------------------------------
// Define the arrays 
//----------------------------------------------------------------------------
void vtkImageEMLocalSegmenter::SetLogMu(double mu, int y, int x){
  if ((y<1) || (y > this->NumClasses) || (x<1) || (x > this->NumInputImages) || (mu < 0)) {
      vtkErrorMacro(<< "Error: Incorrect input");
    return;
  }
  this->LogMu[y-1][x-1] = mu;
}

void vtkImageEMLocalSegmenter::SetLogCovariance(double value, int z, int y, int x){
  if ((z<1) || (z > this->NumClasses) || (y<1) || (y > this->NumInputImages) || (x<1) || (x > this->NumInputImages)) {
    vtkErrorMacro (<<"Error: Incorrect input");
    return;
  }
  this->LogCovariance[z-1][y-1][x-1] = value;
}

void vtkImageEMLocalSegmenter::SetLabel(int label, int index){
  if ((index<1) || (index > this->NumClasses) || (label < 0)) {
    vtkErrorMacro(<<"vtkImageEMLocalSegmenter::SetLabel: Incorrect input");
    return;
  }
  this->Label[index-1] = label;
}

void vtkImageEMLocalSegmenter::SetIntensityAvgValuePreDef(double value, int index){
  if ((index<1) || (index > this->NumInputImages)) {
    vtkErrorMacro(<<"vtkImageEMLocalSegmenter::SetIntensityAvgValue: Incorrect input");
    return;
  }
  this->IntensityAvgValuePreDef[index-1] = value;
}

void vtkImageEMLocalSegmenter::SetProbDataLocal(int value, int index){
  if ((index<1) || (index > this->NumClasses)) {
    vtkErrorMacro(<<"vtkImageEMLocalSegmenter::SetProbDataLocal: Incorrect input");
    return;
  }
  this->ProbDataLocal[index-1] = value;
}

void vtkImageEMLocalSegmenter::SetTissueProbability(double value, int index) {
  if ((index<1) || (index > this->NumClasses) || (value  < 0) || (value  > 1.0)) {
    vtkErrorMacro(<<"vtkImageEMLocalSegmenter::TissueProbability: Incorrect input index "<< index << " value " << value);
    return;
  }
  this->TissueProbability[index-1] = value;
}

void vtkImageEMLocalSegmenter::SetMarkovMatrix(double value, int k, int j, int i) {
  if ((j<1) || (j > this->NumClasses) ||
      (i<1) || (i > this->NumClasses) ||
      (k<1) || (k > 6) || (value < 0) || (value > 1)) {
     vtkErrorMacro (<< "Error: Incorrect input");
    return;
  }
  this->MrfParams[k-1][j-1][i-1] = value;
}

// -----------------------------------------------------------
// Calculate MF - parrallelised version 
// -----------------------------------------------------------
template  <class T>
static void MeanFieldApproximation3D(int id,
                     double *w_m_input, 
                     unsigned char* MapVector, 
                     double *cY_M, 
                     int imgX, 
                     int imgY, 
                     int imgXY, 
                     int StartVoxel, 
                     int EndVoxel,
                                     int NumClasses,
                     int NumInputImages, 
                     double Alpha, 
                     double ***MrfParams,
                     T **ProbDataPtr, 
                     int *ProbDataIncY, 
                     int *ProbDataIncZ,
                     int *ProbDataLocal,
                     double **LogMu, 
                     double ***InvLogCov, 
                     double *InvSqrtDetLogCov,
                     double *TissueProbability,
                     double *w_m_output) {

  double normRow;                                 
  double mp;  
  int i,j,k, 
    JumpHorizontal  = imgY*NumClasses,
    JumpSlice       = imgXY*NumClasses;
  int LeftOverX,LeftOverY;
  int StartPixel;
  double *w_m_outStart = w_m_output;
  double *wxp = new double[NumClasses],*wxpPtr = wxp,*wxn = new double[NumClasses],*wxnPtr = wxn, *wyn = new double[NumClasses],*wynPtr = wyn,
         *wyp = new double[NumClasses],*wypPtr = wyp,*wzn = new double[NumClasses],*wznPtr = wzn, *wzp = new double[NumClasses],*wzpPtr = wzp;  
  
  // -----------------------------------------------------------
  // neighbouring field values
  // -----------------------------------------------------------
  // The different Matrisses are defined as follow:
  // Matrix[i] = Neighbour in a certain direction
  // In Tcl TK defined in this order: West North Up East South Down 
  // MrfParams[i] :  
  // i = 3 pixel and east neighbour
  // i = 0 pixel and west neighbour
  // i = 4 pixel and south neighbour
  // i = 1 pixel and north neighbour
  // i = 2 pixel and previous neighbour (up)
  // i = 5 pixel and next neighbour (down)

  // -----------------------------------------------------------
  // Calculate Neighbouring Tissue Relationships
  // -----------------------------------------------------------

  // Debug
  // vtkImageEMLocalSegmenter Test;
  // char filename[50];
  // char varname[50];
  // double *weight = new double[imgXY];
//    while (j < NumClasses) { 
//      // Retrieve the vector with certain slice and class from w_m
//      for (i = 0; i < imgXY; i++) weight[i] = w_m_input[j+i*NumClasses];
//      j++;
//      sprintf(varname,"weight(%d,:)",j);
//      sprintf(filename,"EMLocalSegmResult%dWeight%d.m",id,j);
//      Test.WriteVectorToFile(filename,varname,weight,imgXY);
//    }

  j = StartVoxel;
  while (j< EndVoxel) {
    // cout << j << endl;
    // if OutputVector == 64 => Label Class is already predefined for this pixel
    // cout << j <<  " --  " << StartVoxel << endl;
    // if (j == 3421) {
   //      cout << "Hey ------------" << endl;
   //      cout << bool(*MapVector&EMSEGMENT_WEST)  << " " 
   //           << bool(*MapVector&EMSEGMENT_EAST)  << " " 
   //           << bool(*MapVector&EMSEGMENT_NORTH)  << " " 
   //           << bool(*MapVector&EMSEGMENT_SOUTH)  << " " << bool(*MapVector&EMSEGMENT_FIRST)  << " " << bool(*MapVector&EMSEGMENT_LAST)  << " " << bool(*MapVector < EMSEGMENT_DEFINED) << endl;
   //      cout << "Op ------------" << endl;
   //  }

    if (*MapVector < EMSEGMENT_DEFINED) {
      // Is is 0 => it is not an edge -> fast smmothing
      if (*MapVector) {
    for (i=0;i< NumClasses ;i++){
      *wxp=*wxn=*wyn=*wyp=*wzn=*wzp=0;
      if (i > 0) w_m_input -= NumClasses; 
      for (k=0;k< NumClasses ;k++){
        // f(j,l,h-1)
        if (*MapVector&EMSEGMENT_WEST) *wxn += (*w_m_input)*MrfParams[3][k][i]; 
        else                     *wxn += w_m_input[-JumpHorizontal]*MrfParams[3][k][i];
        // f(j,l,h+1)
        if (*MapVector&EMSEGMENT_EAST) *wxp += (*w_m_input)*MrfParams[0][k][i];
        else                     *wxp += w_m_input[JumpHorizontal]*MrfParams[0][k][i];
        //  Remember: The picture is upside down:
        // Therefore I had to switch the MRF paramters 1 (South) and 4(North)
        // f(j,l-1,h)
        if (*MapVector&EMSEGMENT_NORTH)  *wyn += (*w_m_input)*MrfParams[1][k][i];                       
        else                     *wyn += w_m_input[-NumClasses]*MrfParams[1][k][i]; 
        // f(j,l+1,h)
        if (*MapVector&EMSEGMENT_SOUTH)  *wyp += (*w_m_input)*MrfParams[4][k][i];
        else  *wyp += w_m_input[NumClasses]*MrfParams[4][k][i];
        // f(j-1,l,h)
        if (*MapVector&EMSEGMENT_FIRST) *wzn += (*w_m_input)*MrfParams[5][k][i];  
        else                     *wzn += w_m_input[-JumpSlice]*MrfParams[5][k][i]; 
        // f(j+1,l,h)
        if (*MapVector&EMSEGMENT_LAST)  *wzp += (*w_m_input)*MrfParams[2][k][i]; 
        else                     *wzp += w_m_input[JumpSlice]*MrfParams[2][k][i]; 
        w_m_input ++;
      }
      wxn++;wxp++;wyn++;wyp++;wzn++;wzp++;
    }
      } else {
    for (i=0;i<NumClasses ;i++){
      *wxp=*wxn=*wyn=*wyp=*wzn=*wzp=0;
      if (i>0) w_m_input -= NumClasses; 
      for (k=0;k<NumClasses ;k++){
        // f(j,l,h-1)
        *wxn += w_m_input[-JumpHorizontal]*MrfParams[3][k][i];
        // f(j,l,h+1)
        *wxp += w_m_input[JumpHorizontal]*MrfParams[0][k][i];
        // f(j,l-1,h)
        *wyn += w_m_input[-NumClasses]*MrfParams[4][k][i]; 
        // f(j,l+1,h)
        *wyp += w_m_input[NumClasses]*MrfParams[1][k][i];
        // f(j-1,l,h)
        *wzn += w_m_input[-JumpSlice]*MrfParams[5][k][i]; 
        // f(j+1,l,h)
        *wzp += w_m_input[JumpSlice]*MrfParams[2][k][i]; 
        w_m_input ++;
      }
      wxn++;wxp++;wyn++;wyp++;wzn++;wzp++;
    }
      }
      wxp = wxpPtr; wxn = wxnPtr; wyn = wynPtr; wyp = wypPtr; wzn = wznPtr; wzp = wzpPtr;      
      // -----------------------------------------------------------
      // Calculate Probabilities and Update Weights
      // -----------------------------------------------------------
      // mp = (ones(prod(imS),1)*p).*(1-alpha + alpha*wxn).*(1- alpha +alpha*wxp)...
      //.*(1- alpha + alpha*wyp).*(1-alpha + alpha*wyn).*(1- alpha + alpha*wzp).*(1-alpha + alpha*wzn);
      // w have to be normalized !
      normRow = 0;
      for (i=0; i<NumClasses; i++) {
        // Goes a little bit further in the branches - like it better
        // it should be exp(*ProbData)[i] from Tina's thesis I say now bc values with zero probability will be still displayed
        // Kilian May 2002: I believe the form underneath is correct - Tina's PhD thesis displays it exactly like underneath 
    mp = TissueProbability[i] * (1-Alpha+Alpha*exp((*wxp++) + (*wxn++) + (*wyp++) + (*wyn++) + (*wzp++) + (*wzn++)));
    if (ProbDataLocal[i]) mp *= double(*ProbDataPtr[i]);
    w_m_output[i] = mp*vtkImageEMGeneral::FastGaussMulti(InvSqrtDetLogCov[i],cY_M, LogMu[i],InvLogCov[i],NumInputImages); 
    normRow += w_m_output[i];
      } 
      wxp = wxpPtr; wxn = wxnPtr; wyn = wynPtr; wyp = wypPtr; wzn = wznPtr; wzp = wzpPtr;
      // I do not know if I should assign it ot first or second term
      if (normRow == 0.0) {
    for (i=0; i<NumClasses; i++) {
      w_m_output[i] =  TissueProbability[i] * exp((*wxp++) + (*wxn++) + (*wyp++) + (*wyn++) + (*wzp++) + (*wzn++));
      normRow += w_m_output[i];
    } 
    wxp = wxpPtr; wxn = wxnPtr; wyn = wynPtr; wyp = wypPtr; wzn = wznPtr; wzp = wzpPtr;
    if (normRow == 0.0) {
      for (i=0; i<NumClasses; i++) {
        // (*w_m_output)[i] = vtkImageEMGeneral::LookupGauss(GaussLookupTable[i],TableLBound[i],TableUBound[i],TableResolution[i],*cY_M, NumInputImages);
        w_m_output[i] = TissueProbability[i] * vtkImageEMGeneral::FastGaussMulti(InvSqrtDetLogCov[i],cY_M, LogMu[i],InvLogCov[i],NumInputImages); 
        normRow += w_m_output[i];
      } 
    }
    if (normRow == 0.0) {normRow = 1;}
      }
      // Norm Weights
      for (i=0; i< NumClasses; i++) {
    (*w_m_output ++) /= normRow;       
      }
          
    } else {
      w_m_output += NumClasses;
      w_m_input += NumClasses;
    }
    cY_M += NumInputImages;MapVector++;
    for (i=0; i < NumClasses; i++) { if (ProbDataLocal[i]) ProbDataPtr[i] ++;}
    j++;
    if (j % imgX) {
      for (i=0; i < NumClasses; i++) { if (ProbDataLocal[i]) ProbDataPtr[i] += ProbDataIncY[i];}
      if (j % imgXY) for (i=0; i < NumClasses; i++) { if (ProbDataLocal[i]) ProbDataPtr[i] += ProbDataIncZ[i];}
    }
  }
  w_m_output = w_m_outStart;
  //j = 0;
  //while (j < NumClasses) { 
  //      // Retrieve the vector with certain slice and class from w_m
  //      for (i = 0; i < imgXY; i++) weight[i] = w_m_output[j+i*NumClasses];
  //      j++;
  //      sprintf(varname,"weight(%d,:)",j);
  //      sprintf(filename,"EMLocalSegmResult%dWeight%d.m",id,j);
  //      Test.WriteVectorToFile(filename,varname,weight,imgXY);
  //    }
  //vtkImageEMLocalSegmenter test;
  //char filename[50];
  // sprintf(filename,"ProbDataPtr.%d",id);
  // test.WriteMRIfile(filename,NULL, 0,(short*)*w_m, 256*256);

  // Delete Variables
  delete[] wxp;delete[] wxn;delete[] wyn;delete[] wyp;delete[] wzn;delete[] wzp;
}

// -----------------------------------------------------------
// Calculate MF - parrallelised version 
// user interface when called with threads 
// -----------------------------------------------------------
void MeanFieldApproximation3DThread(void *jobparm) {
  MF_Approximation_Work *job = (MF_Approximation_Work *)jobparm;
  switch (job->ProbDataType) {
    case VTK_DOUBLE:    MeanFieldApproximation3D(job->id, job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (double**) job->ProbDataPtr, job->ProbDataIncY,
                         job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,job->TissueProbability,
                         job->w_m_output);
    break; 
    case VTK_FLOAT:  MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (float**) job->ProbDataPtr, job->ProbDataIncY,
                         job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,job->TissueProbability,
                             job->w_m_output);
    break; 
    case VTK_LONG:  MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (long**) job->ProbDataPtr, job->ProbDataIncY,
                         job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,job->TissueProbability,
                             job->w_m_output); 
    break; 
    case VTK_UNSIGNED_LONG:   MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (unsigned long**) job->ProbDataPtr, 
                         job->ProbDataIncY, job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,
                         job->TissueProbability,job->w_m_output);
    break; 
    case VTK_INT:  MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (int**) job->ProbDataPtr, job->ProbDataIncY,
                         job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,job->TissueProbability,
                             job->w_m_output);
    break; 
    case VTK_UNSIGNED_INT:  MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (unsigned int**) job->ProbDataPtr, 
                         job->ProbDataIncY, job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov, job->InvSqrtDetLogCov, 
                         job->TissueProbability,job->w_m_output);
    break; 
    case VTK_SHORT:  MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (short**) job->ProbDataPtr, job->ProbDataIncY,
                         job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,job->TissueProbability,
                             job->w_m_output);
    break; 
    case VTK_UNSIGNED_SHORT:   MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (unsigned short**) job->ProbDataPtr, 
                         job->ProbDataIncY,job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,
                         job->TissueProbability,job->w_m_output);
    break; 
    case VTK_CHAR:   MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (char**) job->ProbDataPtr, job->ProbDataIncY,
                         job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,job->TissueProbability,
                             job->w_m_output);
    break; 
    case VTK_UNSIGNED_CHAR:  MeanFieldApproximation3D(job->id,job->w_m_input,job->MapVector,job->cY_M,job->imgX,job->imgY,job->imgXY,job->StartVoxel,job->EndVoxel,
                         job->NumClasses,job->NumInputImages,job->Alpha,job->MrfParams, (unsigned char**) job->ProbDataPtr, 
                         job->ProbDataIncY,job->ProbDataIncZ, job->ProbDataLocal, job->LogMu,job->InvLogCov,job->InvSqrtDetLogCov,
                         job->TissueProbability,job->w_m_output);
    break;
    default:
    return;
  }
}

// -----------------------------------------------------------
// Print out results into a file "EMLocalSegmResult<name>.m"
// Results can be display with Matlab Program listed below 
// -----------------------------------------------------------
void vtkImageEMLocalSegmenter::PrintMatlabGraphResults(int iter,int slice,int FullProgram,int imgXY, double *w_m) {
  char filename[40];
  char varname[40];
  int appendFlag = 0;
  int c;
  vtkFileOps write;

  double *weight = new double[imgXY];
  // double *b_mvec = new double[imgXY];
  unsigned char *ind_Vector = new unsigned char[imgXY];
  this->DeterminLabelMap(ind_Vector, w_m,imgXY,1);
  sprintf(filename,"EMLocalSegmResult%dImage.m",iter);
  write.WriteVectorMatlabFile(filename,"Image",ind_Vector, imgXY);  

  c = 0;
  w_m += imgXY*(slice-1)*this->NumClasses; 
  while (c < this->NumClasses) { 
    // Retrieve the vector with certain slice and class from w_m
    for (int i = 0; i < imgXY; i++) weight[i] = w_m[c+i*this->NumClasses];
    c++;
    sprintf(varname,"weight(%d,:)",c);
    sprintf(filename,"EMLocalSegmResult%dWeight%d.m",iter,c);
    write.WriteVectorMatlabFile(filename,varname,weight,imgXY);
    // GE Format
    sprintf(filename,"EMLocalSegmResult%dWeight%d.%03d",iter,c,this->StartSlice+slice-1);
    // KILIAN - not in old FileOps
    //write. WriteDoubleToGEFile(filename,weight,imgXY);
  }

  c=0;
//    b_m += imgXY*(slice-1); 
//    while (c < this->NumInputImages) { 
//      // Retrieve the vector with certain slice and class from w_m
//      for (int i = 0; i < imgXY; i++) b_mvec[i] = b_m [c+i*this->NumClasses];
//      c++;
//      sprintf(varname,"b_m(%d,:)",c);
//      sprintf(filename,"EMLocalSegmResult%dBias%d.m",iter,c);
//      this->WriteVectorToFile(filename,varname,b_mvec,imgXY);
//    }
  // sprintf(filename,"EMLocalSegmResult%db_m.m",iter);
  // this->WriteMatrixToFile(filename,"b_m",b_m,imgXY,this->NumInputImages);

  if (FullProgram) {
    sprintf(filename,"EMLocalSegmResult%d.m",iter);
    FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
    if ( f == NULL ) {
      cerr << "Could not open file " << filename << "\n";
      return;
    }

    fprintf(f,"%% (c) Copyright 2002 Massachusetts Institute of Technology\n");
    fprintf(f,"%% Result of the %d iteration of the EM Algorithm\n",iter);
    fprintf(f,"EMLocalSegmResult%dImage\n",iter);
    fprintf(f,"figure(%d1)\n",iter);
    fprintf(f,"set(%d1, 'position', [10 445 300 300]);\n",iter);
    fprintf(f,"imagesc([reshape(Image(:),[%d %d])]);\n",this->ImageMaxX,this->ImageMaxY);
    fprintf(f,"title('Label Map - Iteration %d');\n", iter);
    // fprintf(f,"colormap([0 0 1; 0 1 0; 1 0 0; 1 1 0;]);\n");
    fprintf(f,"colormap('default');\n");
    fprintf(f,"\n");

    fprintf(f,"figure(%d2);\n",iter);
    fprintf(f,"set(%d2, 'position', [10 10 1210 360]);\n",iter);
    fprintf(f,"colormap(gray); \n");
    fprintf(f,"dwm=[];\n");
    for (c = 1; c <= this->NumClasses ; c++ ) 
      fprintf(f,"EMLocalSegmResult%dWeight%d\n",iter,c); 
    fprintf(f,"for t=1:%d\n",this->NumClasses);
    fprintf(f,"  temp_m=reshape(weight(t,:),[%d %d]);\n",this->ImageMaxX, this->ImageMaxY); 
    fprintf(f,"  dwm = [dwm,  temp_m(:,:)];\n");
    fprintf(f,"end\n");
    fprintf(f,"imagesc([dwm]);\n");
    fprintf(f,"title('Weights - Iteration %d');\n", iter);
    fprintf(f,"\n");

    //  fprintf(f,"figure(%d3);\n",iter);
//      fprintf(f,"set(%d3, 'position', [110 110 1310 460]);\n",iter);
//      fprintf(f,"colormap(gray); \n");
//      fprintf(f,"bwm=[];\n");
//      for (c = 1; c <= this->NumInputImages ; c++ ) 
//        fprintf(f,"EMLocalSegmResult%dBias%d\n",iter,c); 
//      fprintf(f,"for t=1:%d\n",this->NumInputImages);
//      fprintf(f,"  temp_m=reshape(b_m(t,:),[%d %d]);\n",this->ImageMaxX, this->ImageMaxY); 
//      fprintf(f,"  bwm = [bwm,  temp_m(:,:)];\n");
//      fprintf(f,"end\n");
//      fprintf(f,"imagesc([bwm]);\n");
//      fprintf(f,"title('Bias - Iteration %d');\n", iter);
//      fprintf(f,"\n");
    fflush(f);
    fclose(f);
  }
  // delete[] b_mvec;
  delete[] weight;
  delete[] ind_Vector;
}

void vtkImageEMLocalSegmenter::DeterminLabelMap(unsigned char* LabelMap, double * w_m, int imgXY, int imgZ) { 
  int idx,l,MaxProbIndex;
  double MaxProbValue;
  int max = imgXY*imgZ;
  for (idx = 0; idx < max ; idx++) {
    MaxProbValue = (*w_m);
    MaxProbIndex = 0;
    w_m ++;
    for (l=1; l<this->NumClasses; l++) {
      if ( (*w_m) > MaxProbValue) {MaxProbValue = (*w_m); MaxProbIndex = l;}
      w_m++;
    }
    *(LabelMap++) = this->Label[MaxProbIndex];
  }
}

// Special Vector and Matrix functions
void vtkImageEMLocalSegmenter::DeleteVariables() {
  int z,y;
  if (this->NumClasses > 0 ){
    delete[] this->ProbDataLocal;
    delete[] this->Label;
    delete[] this->TissueProbability;
    delete[] this->ProbDataPtr;
    delete[] this->ProbDataIncY;
    delete[] this->ProbDataIncZ;
    delete[] this->IntensityAvgValuePreDef;
    delete[] this->IntensityAvgValueCurrent;
    delete[] this->BiasRootFileName;

    for (z=0; z< this->NumClasses; z ++) { 
       for (y=0; y < this->NumInputImages; y++)  delete[] this->LogCovariance[z][y];
       delete[] this->LogCovariance[z];
       delete[] this->LogMu[z]; 
    }
    delete[] this->LogCovariance;
    delete[] this->LogMu; 

    for (z=0; z< 6; z ++) { 
      for (y=0; y < this->NumClasses; y++)
    delete[] this->MrfParams[z][y];
      delete[] this->MrfParams[z];
    }
    delete[] this->MrfParams;

    this->LogMu = NULL;this->LogCovariance = NULL;this->TissueProbability;this->Label;this->MrfParams = NULL;this->ProbDataLocal;
    this->ProbDataPtr = NULL; this->ProbDataIncY = NULL; this->ProbDataIncZ = NULL; this->IntensityAvgValuePreDef = NULL;this->IntensityAvgValueCurrent = NULL;    
    this->BiasRootFileName = NULL;
  }
}

// -----------------------------------------------------------
// Calculate MF - necessary for parrallising algorithm
// -----------------------------------------------------------

int vtkImageEMLocalSegmenter::MF_Approx_Workpile(double *w_m_input,unsigned char* MapVector, double *cY_M, int imgXY,
                           double ***InvLogCov,double *InvSqrtDetLogCov,double *w_m_output) {
  #define MAXMFAPPROXIMATIONWORKERTHREADS 32
  int numthreads = 0;
  int jobsize,i,j;
  int StartIndex = 0;
  int StartPointer;

  MF_Approximation_Work job[MAXMFAPPROXIMATIONWORKERTHREADS];

#ifdef _WIN32
  numthreads = 1;
#else
  workpile_t workpile;
  vtkThread thread;
  numthreads = vtkThreadNumCpus(void) ;

  assert((numthreads <= MAXMFAPPROXIMATIONWORKERTHREADS) && (numthreads > 0));

  workpile = thread.work_init(numthreads,MeanFieldApproximation3DThread, numthreads);
#endif
  jobsize = this->ImageProd/numthreads;

  for (i = 0; i < numthreads; i++) {
    job[i].id               = i;
    job[i].w_m_input        = w_m_input  + StartIndex*this->NumClasses;
    job[i].MapVector        = MapVector  + StartIndex;
    job[i].cY_M             = cY_M       + StartIndex*this->NumInputImages;
    job[i].imgX             = this->ImageMaxX;
    job[i].imgY             = this->ImageMaxY;
    job[i].imgXY            = imgXY;
    job[i].StartVoxel       = StartIndex;
    job[i].EndVoxel         = StartIndex + jobsize;
    job[i].NumClasses       = this->NumClasses;
    job[i].NumInputImages   = this->NumInputImages;
    job[i].Alpha            = this->Alpha;
    job[i].MrfParams        = this->MrfParams;      
    // --- Begin ProbDataPtr Definition
    job[i].ProbDataType     = this->GetInput(0)->GetScalarType(); 
    job[i].ProbDataLocal     = this->ProbDataLocal; 
    job[i].ProbDataPtr      = new void*[this->NumClasses];
    for (j=0; j <this->NumClasses; j++) {
      StartPointer  = ((this->ImageMaxX + this->ProbDataIncY[j])*this->ImageMaxY + this->ProbDataIncZ[j])*(StartIndex/imgXY); // Z direction
      StartPointer += (this->ImageMaxX + this->ProbDataIncY[j])*((StartIndex/this->ImageMaxX) % this->ImageMaxY); // Y direction
      StartPointer += StartIndex % this->ImageMaxX; // X Direction 
      if (job[i].ProbDataLocal[j]) {
    switch (job[i].ProbDataType) {
          case VTK_DOUBLE:         job[i].ProbDataPtr[j] =  (void *) ((double *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_FLOAT:          job[i].ProbDataPtr[j] =  (void *) ((float *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_LONG:           job[i].ProbDataPtr[j] =  (void *) ((long *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_UNSIGNED_LONG:  job[i].ProbDataPtr[j] =  (void *) ((unsigned long *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_INT:            job[i].ProbDataPtr[j] =  (void *) ((int *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_UNSIGNED_INT:   job[i].ProbDataPtr[j] =  (void *) ((unsigned int *) this->ProbDataPtr[j] + StartPointer); break;
    case VTK_SHORT:            job[i].ProbDataPtr[j] =  (void *) ((short *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_UNSIGNED_SHORT: job[i].ProbDataPtr[j] =  (void *) ((unsigned short *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_CHAR:           job[i].ProbDataPtr[j] =  (void *) ((char *) this->ProbDataPtr[j] + StartPointer); break;
          case VTK_UNSIGNED_CHAR:  job[i].ProbDataPtr[j] =  (void *) ((unsigned char *) this->ProbDataPtr[j] + StartPointer); break;
    default:
      cout << "Execute: Unknown ScalarType" << endl;
      return 1;
    }
      } else {
    job[i].ProbDataPtr[j] = NULL;
      }    
    }               
    job[i].ProbDataIncY      = this->ProbDataIncY;
    job[i].ProbDataIncZ      = this->ProbDataIncZ;
    // --- End ProbDataPtr Definition
    job[i].w_m_output        = w_m_output + StartIndex*this->NumClasses;
    job[i].LogMu             = this->LogMu;
    job[i].InvLogCov         = InvLogCov;
    job[i].InvSqrtDetLogCov  = InvSqrtDetLogCov;
    job[i].TissueProbability = this->TissueProbability;
    StartIndex += jobsize;
#ifdef _WIN32
    MeanFieldApproximation3DThread(&job[i]);
#else
    thread.work_put(workpile, &job[i]);
#endif
  }

  /* At this point all the jobs are done and the workers are waiting */
  /* In order to avoid a memory leak they should now all be killed off,
   *   or asked to suicide
   */
#ifdef _WIN32
#else
  thread.work_wait(workpile);
  thread.work_finished_forever(workpile);
#endif
  // Delete here
  for (i=0; i <numthreads; i++) delete[] job[i].ProbDataPtr;        
  return 0; /* Success */
}

//----------------------------------------------------------------------------
// The EM Algorithm based on Tina Kapur PhD Thesis and the Matlab 
// implementation from Carl-Fredrik Westin, Mats Bjornemo and Anders Brun
template  <class T>
static void vtkImageEMLocalAlgorithm(vtkImageEMLocalSegmenter *self,T **ProbDataPtrStart, double** InputVector, unsigned char *OutputVector) {
  cout << "This Computer has " << vtkThreadNumCpus(void)  << " Processors" << endl;
  cout << "vtkImageEMLocalAlgorithm: Initialize Variables" << endl;

  // Read variables Class Definition

  int ImageProd             = self->GetImageProd();
  int NumClasses            = self->GetNumClasses();
  int NumInputImages        = self->GetNumInputImages(); 
  int ImageMaxZ             = self->GetImageMaxZ();
  int ImageMaxY             = self->GetImageMaxY();
  int ImageMaxX             = self->GetImageMaxX();
  int NumIter               = self->GetNumIter();
  int NumRegIter            = self->GetNumRegIter();
  int *ProbDataIncZ         = self->GetProbDataIncZ();
  int *ProbDataIncY         = self->GetProbDataIncY();
  double **LogMu            = self->GetLogMu();
  double ***LogCovariance   = self->GetLogCovariance();
  double *TissueProbability = self->GetTissueProbability();
  double Alpha              = self->GetAlpha();
  int SmoothingWidth        = self->GetSmoothingWidth();
  int SmoothingSigma        = self->GetSmoothingSigma();
  // Bias Information
  int BiasPrint             = self->GetBiasPrint();
  int BiasLengthFileName    = 0;
  if (self->GetBiasRootFileName() != NULL) BiasLengthFileName = strlen(self->GetBiasRootFileName());
  char BiasRootFileName[BiasLengthFileName + 1];
  if ((BiasPrint) && (BiasLengthFileName)) strcpy(BiasRootFileName,self->GetBiasRootFileName());
  else sprintf(BiasRootFileName,"\n");

  // Define Default OutputVector
  memset(OutputVector, 0, ImageProd*sizeof(unsigned char));
  // Check if everything is set correctly
  if (self->checkValues() < 0) return;
  // ------------------------------------------------------
  // General Variables 
  // ------------------------------------------------------
  int i,j,k,l,m,n,index, x,y,z;
  int iter,regiter;
  int imgXY = ImageMaxY*ImageMaxX;
  

  char filename[50];
  T **ProbDataPtrCopy = new T*[NumClasses];
  for (i =0;i<NumClasses;i++) {
    ProbDataPtrCopy[i] = ProbDataPtrStart[i];

    // sprintf(filename,"ImageWeight%d",i);
    // self->WriteShortDataToMRIFile(filename, (short*) ProbDataPtrCopy[i],0,self->GetEndSlice()-self->GetStartSlice(),ProbDataIncY[i],ProbDataIncZ[i]);
  }
  // ------------------------------------------------------
  // Setting up Class Distribution 
  // Calculate the mean of the log(greyvalue+1), 
  // currently we have the mean over the grey scale values !
  // ------------------------------------------------------
  double *InvSqrtDetLogCov = new double[NumClasses];
  double ***InvLogCov      = new double**[NumClasses];
  for (i=0; i<NumClasses; i++) {
    InvLogCov[i] = new double*[NumInputImages];
    for (j=0; j<NumInputImages; j++) {
      InvLogCov[i][j] = new double[NumInputImages];
    }
  }
  int SIZEOF_NUMCLASS_DOUBLE = NumClasses*sizeof(double);

  for (i=0; i < NumClasses; i++) {
    if (vtkImageEMGeneral::InvertMatrix(LogCovariance[i],InvLogCov[i],NumInputImages) == 0) {
      cout << "Could not caluclate the Inverse of the Log Covariance of tissue class "<<i <<". Covariance Matrix is probably almost signular!" << endl;
      return;
    } 
    InvSqrtDetLogCov[i] = vtkImageEMGeneral::determinant(LogCovariance[i],NumInputImages);
    if  (InvSqrtDetLogCov[i] <= 0.0) {
      cout << "Coveriance Matrix (value= " << InvSqrtDetLogCov[i] <<") for tissue class "<<i<<" is probably almost signular or not positiv! Could not calculate the inverse determinant of it " << endl;
      return;
    } 
    InvSqrtDetLogCov[i] = sqrt(1/InvSqrtDetLogCov[i]);
  }
  // ------------------------------------------------------------
  // EM-MF Variables 
  // ------------------------------------------------------------
  // bias for emseg+mrf - got rid of it to save memory 
  // double *b_m = new double[NumInputImages*ImageProd], *b_mPtr = b_m; // dimension NumInputImages x ImageProd 
  // memset(b_m[i], 0, sizeof(double)*NumInputImages*ImageProd);
  double b_m;

  double *w_m = new double[NumClasses*ImageProd],*w_mPtr = w_m; // Result of Weights after trad. E Step -  dimesion NumClasses x ImageProd
  memset(w_m,0,sizeof(double)*NumClasses*ImageProd);

  double *w_m_second = new double[NumClasses*ImageProd],*w_m_secondPtr = w_m_second; // Needed for Parallelising MF-Approximation

  double *cY_M = new double[NumInputImages* ImageProd], *cY_MPtr = cY_M; // cY_M correct log intensity - dimension NumInputImages x ImageProd
  // We assume the input is always positive - otherwise you have to take fabs(InputImage)  
  // cY_M  = fabs(InputVector - b_m) = {b_m ==0} = fabs(InputVector) = InputVector;
  for (i=0; i< ImageProd; i++) {
    memcpy(cY_M,InputVector[i],sizeof(double)*NumInputImages);
    cY_M += NumInputImages;
  }
  cY_M = cY_MPtr;
  
  unsigned char *OutputVectorPtr = OutputVector;
  int ProbValue;
  double normRow;   

  for (i=0;                    i < imgXY ; i++)           OutputVector[i] |= EMSEGMENT_FIRST; 
  for (i=ImageProd-imgXY;i < ImageProd ; i++) OutputVector[i] |= EMSEGMENT_LAST;
  index = 0;
  for (j=0; j < ImageMaxZ; j++) { 
    for (i=0;          i<ImageMaxY;   i++)                     OutputVector[i+index] |= EMSEGMENT_WEST;
    for (i=imgXY-ImageMaxY; i<imgXY;  i++)                     OutputVector[i+index] |= EMSEGMENT_EAST;
    index += imgXY;
  } 
  //  Remember: The picture is upside down - I left the orientation but later on in the MF step the MRF Matirx is changed
  for (i=0;                 i<ImageProd;i+=ImageMaxY)  OutputVector[i] |= EMSEGMENT_NORTH;
  for (i=ImageMaxY-1; i<ImageProd;i+=ImageMaxY)  OutputVector[i] |= EMSEGMENT_SOUTH;
  // ------------------------------------------------------------
  // M Step Variables 
  // ------------------------------------------------------------
  double lbound = (-(SmoothingWidth-1)/2); // upper bound = - lbound
  double temp;
  double *skern = new double[SmoothingWidth]; 
  for (i=0; i < SmoothingWidth; i++) skern[i] = vtkImageEMGeneral::FastGauss(1.0 / SmoothingSigma,i + lbound);

  EMVolume *r_m  = new EMVolume[NumInputImages]; // weighted residuals
  for (i=0; i < NumInputImages; i++) {
    r_m[i].Resize(ImageMaxZ,ImageMaxY,ImageMaxX);
  }

  EMTriVolume iv_m(NumInputImages,ImageMaxZ,ImageMaxY,ImageMaxX); // weighted inverse covariances 
  double **iv_mat     = new double*[NumInputImages];
  double **inv_iv_mat = new double*[NumInputImages];
  for (i=0; i < NumInputImages; i++) {
    iv_mat[i]     = new double[NumInputImages];
    inv_iv_mat[i] = new double[NumInputImages];
  }

  // Bias Field 
  FILE **BiasFile = new FILE*[NumInputImages];
  bool PrintBiasFlag;
  if (BiasLengthFileName) BiasLengthFileName = BiasLengthFileName + 7 + NumInputImages/10;
  char BiasFileName[BiasLengthFileName + 1];
  if (BiasLengthFileName == 0)  sprintf(BiasFileName,"\n");

  START_PRECISE_TIMING;
  // ------------------------------------------------------------
  // Start Algorithm 
  // ------------------------------------------------------------
  for (iter=1; iter <= NumIter;iter++){
    // -----------------------------------------------------------
    // E-Step
    // -----------------------------------------------------------
    // cY_m = abs(Y - b_m(:));  -> corrected log intensities we will do this now at the end of the M-Step to save memory 
    // Ininitialize first iteration
    // This is the EM Algorithm with out MF Part -> The Regulizing part is the part where 
    // the MF part is added
   if ((iter == 1) || (Alpha == 0)) { 
      cout << "vtkImageEMLocalAlgorithm: "<< iter << ". Estep " << endl;
      for (z = 0; z < ImageMaxZ ; z++) { 
    for (y = 0; y < ImageMaxY ; y++) {
      for (x = 0; x < ImageMaxX ; x++) {
        normRow = 0;
        ProbValue = -2;
        for (j=0; j < NumClasses; j++) {
          // check down a little bit later
          // (*w_m)[j] = (*ProbabilityData)[j]*vtkImageEMGeneral::LookupGauss(GaussLookupTable[j],TableLBound[j],TableUBound[j],TableResolution[j],*cY_M, NumInputImages);
          // (*w_m)[j] = ProbabilityData[i][j]*vtkImageEMGeneral::FastGaussMulti(InvSqrtDetLogCov[j],*cY_M, LogMu[j],InvLogCov[j],NumInputImages);
          w_m[j] = TissueProbability[j] * vtkImageEMGeneral::FastGaussMulti(InvSqrtDetLogCov[j],cY_M, LogMu[j],InvLogCov[j],NumInputImages);

          if (ProbDataPtrCopy[j]) w_m[j] *= ((double) *ProbDataPtrCopy[j]);
          normRow += w_m[j];
          // Predefine those areas where only one Atlas map has a value
          if (ProbValue != -1) {
        if (ProbDataPtrCopy[j]) {
          if (*ProbDataPtrCopy[j] > 0) {
            if (ProbValue == -2) ProbValue = j; //no other class has (*ProbDataPtrCopy[j]) > 0 so far
            else ProbValue = -1;                //more than one other class has (*ProbDataPtrCopy[j]) > 0 so far
          }
        } else {
          if (ProbValue == -2) ProbValue = j; 
          else ProbValue = -1;                
        }
          }
        }
        // There is a problem if normRow is 0 -> happens quite frequently with local priors
        // Just assing it to local prior for inital setting. The second term is not of such great value
        // because we do not know anything about bias -> Probability Map is more precise
        if (normRow == 0.0) {
          for (j=0; j < NumClasses; j++) {
        if (ProbDataPtrCopy[j]) w_m[j] = double(*ProbDataPtrCopy[j]);
        normRow += w_m[j];
          }
          if (normRow == 0.0) {
        for (j=0; j < NumClasses; j++) {
          // vtkImageEMGeneral::LookupGauss(GaussLookupTable[j],TableLBound[j],TableUBound[j],TableResolution[j],*cY_M, NumInputImages);
          // (*w_m)[j] = TissueProbability[j]*vtkImageEMGeneral::FastGaussMulti(InvSqrtDetLogCov[j],*cY_M, LogMu[j],InvLogCov[j],NumInputImages); 
          w_m[j] = TissueProbability[j]*vtkImageEMGeneral::FastGaussMulti(InvSqrtDetLogCov[j],cY_M, LogMu[j],InvLogCov[j],NumInputImages); 
          normRow += w_m[j];
        }
          }
          if (normRow == 0.0) {normRow = 1;}
        }
        // Normalize Rows and find Maxium of every Row and write it in Matrix        
        if (ProbValue > -2) {
          for (j=0; j < NumClasses; j++) {w_m[j] /= normRow;}
          
          // Label for this pixel ia lready defined
          if (ProbValue > -1) {
        // Has to be done that way otherwise w_m_second is not defined at those places and that is deadramental in the M Step (think about w_m[-ImgY])
        *OutputVector |= EMSEGMENT_DEFINED;
        memcpy(w_m_second,w_m,SIZEOF_NUMCLASS_DOUBLE);
          }

        } else { 
          // Probabilityt Map is 0 for every class -> Label is assigned to first class
          *w_m = 1;
          for (j=1; j < NumClasses; j++) {w_m[j] = 0;}
          // *OutputVector = (int) Label[0]; 
          *OutputVector |= EMSEGMENT_DEFINED; 
          // See not above
          memcpy(w_m_second,w_m,SIZEOF_NUMCLASS_DOUBLE);
        }
        w_m += NumClasses;w_m_second +=NumClasses;cY_M += NumInputImages; OutputVector++;
        for (j=0; j < NumClasses; j++) {if (ProbDataPtrCopy[j]) ProbDataPtrCopy[j] ++;}
      }
      for (j=0; j < NumClasses; j++) {if (ProbDataPtrCopy[j]) ProbDataPtrCopy[j] += ProbDataIncY[j];}
    }
    for (j=0; j < NumClasses; j++) {if (ProbDataPtrCopy[j]) ProbDataPtrCopy[j] += ProbDataIncZ[j];}
      }
      cY_M = cY_MPtr;w_m = w_mPtr;w_m_second = w_m_secondPtr;OutputVector = OutputVectorPtr;
      for (j=0; j < NumClasses; j++) ProbDataPtrCopy[j] = ProbDataPtrStart[j];
    }
   // self->PrintMatlabGraphResults(iter,self->GetPrintIntermediateSlice(),1, imgXY, w_m);
    // -----------------------------------------------------------
    // regularize weights using a mean-field approximation
    // -----------------------------------------------------------
    if (Alpha > 0) {
      for (regiter=1; regiter <= NumRegIter; regiter++) {
    cout << "vtkImageEMLocalAlgorithm: "<< regiter << ". EM - MF Iteration" << endl;
    if (regiter%2) self->MF_Approx_Workpile(w_m,OutputVector,cY_M,imgXY,InvLogCov,InvSqrtDetLogCov,w_m_second);
    else self->MF_Approx_Workpile(w_m_second,OutputVector,cY_M,imgXY,InvLogCov,InvSqrtDetLogCov,w_m);
      }
      // if it is an odd number of iterations w_m_second holds the current result ! -> Therefore we have to change it !
      if (NumRegIter%2) memcpy(w_m,w_m_second,sizeof(double)*NumClasses*ImageProd);;
    }

    // Does not work
    //for (i=0; i < NumClasses; i++) {
      // sprintf(filename,"ImageWeight%d",i);
      // self->WriteDoubleDataToMRIFile(filename, w_m,0,self->GetEndSlice()-self->GetStartSlice(),0,0);
      //w_m += ImageProd;
    // }
    // w_m = w_mPtr;

    if ((self->GetPrintIntermediateResults()) && ((iter%self->GetPrintIntermediateFrequency()) == 0)){
      cout << "vtkImageEMLocalAlgorithm: Print intermediate result " << endl;
      self->PrintMatlabGraphResults(iter,self->GetPrintIntermediateSlice(),1, imgXY, w_m);
      cout << "vtkImageEMLocalAlgorithm: Return to Algorithm " << endl;
    }
    if (iter < NumIter) {
      cout << "vtkImageEMLocalAlgorithm: Mstep " << endl;
      // -----------------------------------------------------------
      // M-step
      // -----------------------------------------------------------
      // compute weighted residuals 
      // r_m  = (w_m.*(repmat(cY_M,[1 num_classes]) - repmat(mu,[prod(imS) 1])))*(ivar)';
      // iv_m = w_m * ivar';
      // Compare to sandy i = l => he does exactly the same thing as I do 
      index =0;
      for (l=0; l<NumInputImages; l++) {
    r_m[l].SetValue(0.0);
      }
      iv_m.SetValue(0.0);
      for (i = 0; i< ImageMaxZ;i++){
    for (j = 0; j<ImageMaxX;j++){
      for (k = 0; k<ImageMaxY;k++){

        for (l=0; l<NumClasses; l++) {
          for (m=0; m<NumInputImages; m++) {  
        for (n=0; n<NumInputImages; n++) {
          temp =  *w_m * InvLogCov[l][m][n];
          r_m[m](i,k,j)     += temp * (InputVector[index][n] - LogMu[l][n]);
          if (n <= m) iv_m(m,n,i,k,j) += temp;
        }
          }
          w_m++;
        }
        index ++;
      }
    }
      }
      w_m = w_mPtr;
      //  smooth residuals and inv covariances - 3D
      // w(k) = sum(u(j)*v(k+1-j))
      iv_m.Conv(skern,SmoothingWidth);
      for (i=0; i<NumInputImages; i++) r_m[i].Conv(skern,SmoothingWidth);
      // estimate the smoothed bias
      // transform r (smoothed weighted residuals) by iv (smoother inv covariances)
      // b_m = r_m./iv_m ;

      index = 0;
      PrintBiasFlag = bool((iter ==  NumIter-1) && BiasPrint && BiasLengthFileName);
      if (PrintBiasFlag) cout << "vtkImageEMLocalAlgorithm: Write Bias to " << BiasRootFileName << endl;
      for (i = 0; i<ImageMaxZ;i++){
    // Open File to write Bias Field
    if (PrintBiasFlag) {
       for (l=0; l< NumInputImages; l++) { 
         sprintf(BiasFileName,"%sCh%d.%03d",BiasRootFileName,l,i);
         BiasFile[l]= fopen(BiasFileName, "w");
         if ( BiasFile[l] == NULL ) {
           cerr << "vtkImageEMLocalAlgorithm::Error: Could not open file " << BiasFileName << "\n";
           PrintBiasFlag = 0; 
           for (k=0; k< l; k++) fclose(BiasFile[l]);
           l = NumInputImages;
         }
       }
    }
    for (k = 0; k<ImageMaxX;k++){
      for (j = 0; j<ImageMaxY;j++){
        for (l=0; l< NumInputImages; l++) {
          iv_mat[l][l] = iv_m(l,l,i,j,k);
          for (m = 0; m<= l; m++) iv_mat[m][l] = iv_mat[l][m] = iv_m(l,m,i,j,k);
        }
        if (vtkImageEMGeneral::InvertMatrix(iv_mat, inv_iv_mat, NumInputImages)) {
          for (l=0; l< NumInputImages; l++) {
        b_m = 0.0;
        for (m = 0; m< NumInputImages; m++) b_m += inv_iv_mat[l][m]*r_m[m](i,j,k);
        (*cY_M ++) = fabs(InputVector[index][l] - b_m);
        if (PrintBiasFlag) fwrite(&b_m, sizeof(double), 1, BiasFile[l]); 
          }
        } else if (PrintBiasFlag) {
          b_m = 0.0;
          for (l=0; l< NumInputImages; l++) fwrite(&b_m, sizeof(double), 1, BiasFile[l]); 
        }
        index++;
      }
    }      
    if (PrintBiasFlag) {
      for (l=0; l< NumInputImages; l++) {
        fflush(BiasFile[l]);
        fclose(BiasFile[l]);
      }
    }
      }
      cY_M = cY_MPtr;
    } else {
      // -----------------------------------------------------------
      // Copy results to Image
      // -----------------------------------------------------------
      // Find out the maximum propability assigned to a certain class 
      // and assign that pixel to that class
      self->DeterminLabelMap(OutputVector, w_m,imgXY,ImageMaxZ);
    }    
  }
  END_PRECISE_TIMING;
  SHOW_ELAPSED_PRECISE_TIME;  
  // cout << "Calculation Time: " << (clock() - start_time)/double(CLOCKS_PER_SEC) << " seconds " << endl;

  delete[] ProbDataPtrCopy;
  delete[] BiasFile;

  delete[] InvSqrtDetLogCov;
  for (i=0; i<NumClasses; i++) {
    for (j=0; j<NumInputImages; j++) {
      delete[] InvLogCov[i][j];
    }
    delete[] InvLogCov[i];
  }
  delete[] InvLogCov;

  delete[] w_m;
  delete[] w_m_second;
  delete[] cY_M;

  delete[] skern;
  for (i=0; i < NumInputImages; i++) {
    delete[] iv_mat[i];
    delete[] inv_iv_mat[i];
  }
  delete[] iv_mat;
  delete[] inv_iv_mat;

  delete []r_m;
  cout << "vtkImageEMLocalAlgorithm: Finished " << endl;
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class TOut>
static void vtkImageEMLocalSegmenterExecute(vtkImageEMLocalSegmenter *self,double **InputVector,vtkImageData *outData, TOut *outPtr,int outExt[6])
{
  // -----------------------------------------------------
  // 1.) Allocate Memory 
  // -----------------------------------------------------
  int idxR, idxY, idxZ,idx1;
  int outIncX, outIncY, outIncZ;
  int ImageMaxX = self->GetImageMaxX();
  int ImageMaxY = self->GetImageMaxY();
  int ImageMaxZ = self->GetImageMaxZ();
  int maxZ = outExt[5] - outExt[4] + 1;
  int index = 0;
  int NumProbMap = 0 ;
  unsigned char *OutputVector = new unsigned char[ImageMaxZ*ImageMaxY*ImageMaxX];
  
  // -----------------------------------------------------
  // 2.) Define Output
  // -----------------------------------------------------

  // Get increments to march through data 
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  // We assume output is 1 scalar 
  //outData->GetIncements(X,Y,Z) gives you the increments you have to add to go from 
  // poit to point(X, eg 1) row to row (Y eg 256) and slice to slice (Z, eg 256*256)
  
  // 1.) Read thorugh beginning slides we do not want to read through for input and output
  for (idxZ = 1; idxZ < self->GetStartSlice(); idxZ++) {
    for (idxY = 0; idxY < ImageMaxY; idxY++) {
      for (idxR = 0; idxR < ImageMaxX; idxR++) {
    *outPtr = 0;
    outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ; 
  }

  switch (self->GetInput(0)->GetScalarType()) {
    case VTK_DOUBLE:         vtkImageEMLocalAlgorithm(self,(double**) self->GetProbDataPtr(), InputVector, OutputVector); break;
    case VTK_FLOAT:          vtkImageEMLocalAlgorithm(self,(float**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_LONG:           vtkImageEMLocalAlgorithm(self,(long**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_UNSIGNED_LONG:  vtkImageEMLocalAlgorithm(self,(unsigned long**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_INT:            vtkImageEMLocalAlgorithm(self,(int**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_UNSIGNED_INT:   vtkImageEMLocalAlgorithm(self,(unsigned int**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_SHORT:          vtkImageEMLocalAlgorithm(self,(short**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_UNSIGNED_SHORT: vtkImageEMLocalAlgorithm(self,(unsigned short**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_CHAR:           vtkImageEMLocalAlgorithm(self,(char**) self->GetProbDataPtr(), InputVector, OutputVector); break; 
    case VTK_UNSIGNED_CHAR:  vtkImageEMLocalAlgorithm(self,(unsigned char**) self->GetProbDataPtr(), InputVector, OutputVector); break;
  default:
    cout << "Execute: Unknown ScalarType" << endl;
    return;
  } 
  // 5.) Write result in outPtr
  for (idxZ = 0; idxZ < ImageMaxZ; idxZ++) {
    for (idxY = 0; idxY < ImageMaxY; idxY++) {
      for (idxR = 0; idxR < ImageMaxX; idxR++) {
    *outPtr = (TOut) (OutputVector[index]);
    index ++; 
    outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ;
  }
  // 6.) Run through the end slides that were not segmented
  for (idxZ = self->GetEndSlice(); idxZ < maxZ; idxZ++) {
    for (idxY = 0; idxY < ImageMaxY; idxY++) {
      for (idxR = 0; idxR < ImageMaxX; idxR++) {
    *outPtr = 0;
    outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ; 
  }
  delete[] OutputVector;
  //EMVolume test(5,5,5);
  // test.Test(3);
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.

void vtkImageEMLocalSegmenter::ExecuteData(vtkDataObject *)
{
  void *outPtr;
  int idx1;
  int outExt[6];
  int inIncX;
  int StartInputImages = 0;
  int jump;
  int NumProbMap = 0;
  int FirstProbMapDef = -1;
  // vtk4
  vtkImageData **inData  = (vtkImageData **) this->GetInputs();
  vtkImageData *outData = this->GetOutput();
  outData->GetWholeExtent(outExt);
  outData->SetExtent(outExt); 
  outData->AllocateScalars(); 
  // vtk4
  // -----------------------------------------------------
  // Define and Check General Parameters
  // -----------------------------------------------------
  vtkDebugMacro(<< "Execute: inData = " << inData << ", outData = " << outData);

  for (idx1=0 ; idx1<this->NumClasses; idx1++) {
    if (this->ProbDataLocal[idx1]) StartInputImages ++;
  }
  if (this->NumberOfTrainingSamples < 1) {
    vtkErrorMacro(<< "Number of Training Samples taken for the probability map has to be defined first!");
    return;
  }
  if (this->ProbDataLocal) {
    if ((StartInputImages+this-> NumInputImages) > this->NumberOfInputs) {
      vtkErrorMacro(<< "Not enough Input images defined (defined :"<< this->NumberOfInputs << ", need: "<< StartInputImages+this->NumInputImages);
      return;
    }
  } 
  if (outData == NULL) {
    vtkErrorMacro(<< "Ouput must be specified.");
    return;
  }
  // -----------------------------------------------------
  // Define Image Parameters
  // -----------------------------------------------------
  this->ImageMaxX = (outExt[1] - outExt[0]+1)*this->GetInput(StartInputImages)->GetNumberOfScalarComponents();
  this->ImageMaxY = outExt[3] - outExt[2] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
  this->ImageMaxZ = this->EndSlice - this->StartSlice + 1;
  this->ImageProd = this->ImageMaxZ * this->ImageMaxY * this->ImageMaxX;

  // -----------------------------------------------------
  // Checking dimension with start and end slice 
  // -----------------------------------------------------
  // Making sure values are set correctly
  if ((this->StartSlice < 1) || (this->StartSlice > this->EndSlice) || (this->EndSlice > (outExt[5] - outExt[4] + 1))) {
    vtkErrorMacro(<< "Start Slice,"<<this->StartSlice<< ", or EndSlice," << this->EndSlice << ", not defined correctly !");
    return;
  }
  // -----------------------------------------------------
  // 1.) Check Probability Maps
  // -----------------------------------------------------
  // First find out here the first map is defined for the label
  for (idx1 = 0; idx1 < this->NumClasses ; ++idx1){
    // Wherever no local label map is defined we have to catch it when a local label map is needed and therefore make two cases 
    if (this->ProbDataLocal[idx1]) {
      if (FirstProbMapDef == -1)  FirstProbMapDef =  NumProbMap; 
      if (this->CheckInputImage(inData[NumProbMap],this->GetInput(FirstProbMapDef)->GetScalarType(),this->GetInput(NumProbMap)->GetScalarType(),NumProbMap+1,outExt) == false) return ;
      inData[NumProbMap]->GetContinuousIncrements(outExt, inIncX, this->ProbDataIncY[idx1], this->ProbDataIncZ[idx1]);
      
      jump = ((this->ImageMaxX + this->ProbDataIncY[idx1])*this->ImageMaxY + this->ProbDataIncZ[idx1])*(this->StartSlice - 1);
      // Because in check I make sure outExt = inExt the following line is true 
       switch (this->GetInput(NumProbMap)->GetScalarType()) {
     vtkTemplateMacro4(vtkImageEMLocalSegmenterIncrementProbDataPointer,this,idx1, (VTK_TT*) inData[NumProbMap]->GetScalarPointerForExtent(outExt), jump);
      default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
      } 
       NumProbMap ++;
    } else {
      // Wherever no local label map is defined we have to catch it when a local label map is needed and therefore make two cases 
      this->ProbDataPtr[idx1] = NULL;
    }
    
  } 
  // -----------------------------------------------------
  // 2.) Read Input Images
  // -----------------------------------------------------
  double **InputVector = new double*[this->ImageProd];
  for(idx1 = 0; idx1 <this->ImageProd; idx1++) InputVector[idx1] = new double[this->NumInputImages];

  for (idx1 = StartInputImages ; idx1 < StartInputImages+this->NumInputImages ; ++idx1){
    if (this->CheckInputImage(inData[idx1],this->GetInput(idx1)->GetScalarType(),this->GetInput(idx1)->GetScalarType(),idx1+1,outExt) == false) return;
    // Because in check I make sure outExt = inExt the following line is true 
    switch (this->GetInput(idx1)->GetScalarType()) {
      vtkTemplateMacro9(vtkImageEMLocalSegmenterReadInputChannel,this, inData[idx1], (VTK_TT *)(inData[idx1]->GetScalarPointerForExtent(outExt)),outExt,this->ImageMaxX,this->ImageMaxY,this->ImageMaxZ,InputVector,idx1-StartInputImages);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    } 

  }
  // -----------------------------------------------------
  // 2.) Read Probability Data and execute Segmentation Algorithmm
  // -----------------------------------------------------
  outPtr = outData->GetScalarPointerForExtent(outData->GetExtent());
  switch (this->GetOutput()->GetScalarType()) {
    vtkTemplateMacro5(vtkImageEMLocalSegmenterExecute, this, InputVector, outData, (VTK_TT*)outPtr,outExt);
  default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
  }
  for(idx1 = 0; idx1 <this->ImageProd; idx1++) delete[] InputVector[idx1];
  delete[] InputVector;
}
