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
#include "vtkImageEMSegmenter.h"
#include "vtkObjectFactory.h"

static inline double EMSegmenterGauss(double x,double m,double s) {
   if  (s > 0 ) return (1 / (sqrt(2*3.14159265358979)*s)*exp(-(x-m)*(x-m)/(2*pow(s,2))));
   return (m == x ? 1e20:0);
}

//------------------------------------------------------------------------------
vtkImageEMSegmenter* vtkImageEMSegmenter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMSegmenter");
  if(ret)
  {
    return (vtkImageEMSegmenter*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMSegmenter;
}

//----------------------------------------------------------------------------
vtkImageEMSegmenter::vtkImageEMSegmenter()
{
  this->NumClasses = 0;       // Number of Classes - has to be set to 0 bc currently nothing is set
  this->NumIter = 1;          // Number of EM-MRF Iterations
  this->NumRegIter = 1;       // Number of Regularizing Iterations
  this->Alpha = 0.7;          // 0 <= alpha <= 1. Be carefull - has great influence over outcome 
  this->SmoothingWidth = 11;  // Width for Gausian to regularize weights
  this->SmoothingSigma = 5;   // Sigma paramter for regularizing Gaussian
  this->StartSlice = 1;       // First Slide to be segmented
  this->EndSlice = 1;         // Last Slide to be segmented  
  this->ImgTestNo = -1;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  this->ImgTestDivision = -1; // Number of divisions/colors of the picture
  this->ImgTestPixel = -1;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)
  this->PrintIntermediateResults = 0 ;   // Print intermediate results in a Matlab File (No = 0, Yes = 1)
  this->PrintIntermediateSlice = 1 ;     // Print out the result of which slice  
  this->PrintIntermediateFrequency = 1 ; // Print out the result after how many steps
  this->Mu = NULL;this->Sigma = NULL;this->Prob = NULL;this->Label = NULL;this->MrfParams = NULL;
}

vtkImageEMSegmenter::~vtkImageEMSegmenter(){
  this->DeleteVariables();
}

//----------------------------------------------------------------------------
void vtkImageEMSegmenter::PrintSelf(ostream& os)
{
}

// To chage anything about output us this executed before Thread

//----------------------------------------------------------------------------
//void vtkImageAccumulate::ExecuteInformation(vtkImageData *vtkNotUsed(input), 
//					    vtkImageData *output)
//{
//  output->SetWholeExtent(this->ComponentExtent); // size of output
//  output->SetOrigin(this->ComponentOrigin);
//  output->SetSpacing(this->ComponentSpacing);
//  output->SetNumberOfScalarComponents(1);
//  output->SetScalarType(VTK_INT);      // type of output float, integer 
//}



//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageEMSegmenterExecute(vtkImageEMSegmenter *self,vtkImageData *in1Data, T *in1Ptr,
				  vtkImageData *outData, T *outPtr,int outExt[6], int id)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int rowLength;
  int maxXY;
  int StartSlice = self->get_StartSlice();
  int EndSlice = self->get_EndSlice();
  int StartEndSlice; 
  int ImageMax;
  int index;

  // find the region to loop over
  rowLength = (outExt[1] - outExt[0]+1)*in1Data->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
  maxZ = outExt[5] - outExt[4] + 1; // outExt[5/4] = Relative Maximum/Minimum Slice index  
  maxXY = rowLength*maxY;

  // Making sure values are ste correctly
  if (1 > StartSlice) StartSlice = 1;
  if (maxZ < EndSlice) EndSlice = maxZ; 
  else if (StartSlice > EndSlice) EndSlice = StartSlice -1;  
  StartEndSlice = EndSlice - StartSlice + 1;

  // Get increments to march through data 
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  // The Slices of the images -> dynaminc has to be that way otherwise can only use for very restricted number of ways 

  // 1.) Read thorugh beginning slides we do not want to read through for input and output
  in1Ptr += ((rowLength+inIncY)*maxY + inIncZ)*(StartSlice -1); 
  for (idxZ = 1; idxZ < StartSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = 0;
	outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ; 
  }

  //2.)  Define InputOutputVector by reading the log gray values of the image
  double *InputOutputVector = new double[rowLength*maxXY];

  ImageMax = ((int)*in1Ptr);
  for (idxZ = 0; idxZ < StartEndSlice ; idxZ++) { 
    for (idxY = maxY-1; idxY > -1; idxY--) {
      index  =  maxXY*idxZ+idxY;
      for (idxR = 0; idxR < rowLength; idxR++) {
	InputOutputVector[index] = log(double(* in1Ptr) + 1);
        if (int(*in1Ptr) > ImageMax) ImageMax = int(*in1Ptr);
	index += maxY;
	in1Ptr++;
      }
      in1Ptr += inIncY;
    }
    in1Ptr += inIncZ;
  }

  // 3.) Run algorithm
  self->vtkImageEMAlgorithm(InputOutputVector,rowLength,maxY,StartEndSlice, ImageMax);

  // 4.) Display results in output image
  for (idxZ = 0; idxZ < StartEndSlice ; idxZ++) {
    for (idxY = maxY-1; idxY > -1; idxY--) {
      index  = maxXY*idxZ+idxY;
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = (T) (InputOutputVector[index]);
	index += maxY;
	outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ;
  }
 
  // 5.) Run through the end slides that were not segmented
  StartEndSlice = maxZ - EndSlice + 1;
  for (idxZ = 1; idxZ < StartEndSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = 0;
	outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ; 
  }
  delete[] InputOutputVector;
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.

void vtkImageEMSegmenter::ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id)
{
  void *inPtr;
  void *outPtr;
  
  vtkDebugMacro(<< "Execute: inData = " << inData 
		<< ", outData = " << outData);
  

  if (inData == NULL)
    {
    vtkErrorMacro(<< "Input " << 0 << " must be specified.");
    return;
    }

  // Samson suggest:
  // make sure the input has only one component
  // set your output to be unsigned shorts.
  // make sure it works on a parallel machine
  // make sure your input and output sizes are the same
  // figure out how to use the output data directly rather than copying over.

  inPtr = inData->GetScalarPointerForExtent(outExt);
  outPtr = outData->GetScalarPointerForExtent(outExt);
  
  // I added ask Lauren
  if (inData->GetScalarType() != outData->GetScalarType()) {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
                  << ", must match out ScalarType " << outData->GetScalarType());
    return;
  }
  
  switch (inData->GetScalarType()) {
    vtkTemplateMacro7(vtkImageEMSegmenterExecute, this, inData, (VTK_TT *)(inPtr), 
                      outData, (VTK_TT *)(outPtr),outExt, id);
  default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
  }
}


//----------------------------------------------------------------------------
// We need to create a special SetNumClass, bc whenver NumClasses are set, we 
// also have to set arrays which depend on the number of classes e.g. prob
//----------------------------------------------------------------------------

void vtkImageEMSegmenter::SetNumClasses(int NumberOfClasses)
{
  int z,y,x;
  if (this->NumClasses == NumberOfClasses) return;
  this->DeleteVariables();

  if (NumberOfClasses > 0 ) {

    // Create new space for variables
    this->Mu    = new double[NumberOfClasses];
    this->Sigma = new double[NumberOfClasses];
    this->Prob  = new double[NumberOfClasses];
    this->Label = new double[NumberOfClasses];
    
    this->MrfParams = new double**[6];
    for (z=0; z < 6; z++) {
      this->MrfParams[z] = new double*[NumberOfClasses];
      for (y=0;y < NumberOfClasses; y ++) 
	this->MrfParams[z][y] = new double[NumberOfClasses];
    }

    // Set all initial values to -1
    for (x= 0; x < NumberOfClasses; x++) {
      *this->Mu    = -1;
      *this->Sigma = -1;
      *this->Prob  = -1;
      *this->Label = -1;
      this->Mu ++; this->Sigma ++;this->Prob++;this->Label++;
      for (z=0; z < 6; z++) {
	for (y=0;y < NumberOfClasses; y ++) this->MrfParams[z][y][x] = -1;
      }
    }
    this->Mu -= NumberOfClasses; this->Sigma  -= NumberOfClasses;this->Prob  -= NumberOfClasses;this->Label  -= NumberOfClasses;
  } else {
    this->Mu = NULL;this->Sigma = NULL;this->Prob = NULL;this->Label = NULL;this->MrfParams = NULL;
  }
  this->NumClasses = NumberOfClasses;
}
//---------------------------------------------------------------------
// Checks of all paramters are declared  
// 1  = eveything correct
// -1 = Dimension of Volume not correct
// -2 = Not all Mu defined correctly
// -3 = Not all Sigma defined correctly
// -4 = Not all Prob defined correctly
// -5 = Not all MrfParams defined correctly
// -6 = Not all Classes have labels defined

int vtkImageEMSegmenter::checkValues()
{
  int i=0,k,j;
  while (i < this->NumClasses) {
    if (this->Mu[i] < 0) {
      cout << "vtkImageEMSegmenter:checkValues:  Mu[" << i+1 <<"] = " << this->Mu[i] << " must be greater than 0!" << endl;
      return -2;
    }
    if (this->Sigma[i] < 0)  {
      cout << "vtkImageEMSegmenter:checkValues:  Sigma[" << i+1 <<"] = " << this->Sigma[i] << " is not greater than 0!" << endl;
      return -3;
    }
    if ((this->Prob[i] < 0) || (this->Prob[i] > 1.0)) {
      cout << "vtkImageEMSegmenter:checkValues:  Prob[" << i+1 <<"] = " << this->Prob[i] << " is not between 0 and 1!" << endl;
      return -4;
    }
    if (this->Label[i] < 0) {
      cout << "vtkImageEMSegmenter:checkValues:  Color[" << i+1 <<"] = " << this->Label[i] << " is not defined" << endl;
      return -6;
    }
    for (j = 0; j < this->NumClasses; j++) {
       for (k = 0; k < 6; k++) {
	 if ((this->MrfParams[k][j][i] < 0) || (this->MrfParams[k][j][i] > 1)) {
	   cout << "vtkImageEMSegmenter:checkValues:  MrfParams[" << k+1 <<"] [" << j+1 <<"] [" << i+1 <<"] = " << this->MrfParams[k][j][i] 
		<< " is not between 0 and 1!" << endl;
	   return -5;
	 }
       }
    }
    i++;
  }
  return 1;
}



//----------------------------------------------------------------------------
// Define the arrays 
//----------------------------------------------------------------------------
void vtkImageEMSegmenter::SetProbability(double prob, int index) {
  if ((index<1) || (index > this->NumClasses) ||
      (prob < 0) || (prob > 1)) {
      vtkErrorMacro( << "Error: Incorrect input");
    return;
  }
  this->Prob[index-1] = prob;
}

void vtkImageEMSegmenter::SetMu(double mu, int index){
  if ((index<1) || (index > this->NumClasses) ||
      (mu < 0)) {
      vtkErrorMacro(<< "Error: Incorrect input");
    return;
  }
  this->Mu[index-1] = mu;
}

void vtkImageEMSegmenter::SetSigma(double sigma, int index){
  if ((index<1) || (index > this->NumClasses) || (sigma < 0)) {
    vtkErrorMacro (<<"Error: Incorrect input");
    return;
  }
  this->Sigma[index-1] = sigma;
}

void vtkImageEMSegmenter::SetLabel(int label, int index){
  if ((index<1) || (index > this->NumClasses) || (label < 0)) {
      vtkErrorMacro (<<"Error: Incorrect input");
    return;
  }
  this->Label[index-1] = label;
}

void vtkImageEMSegmenter::SetMarkovMatrix(double value, int k, int j, int i) {
  if ((j<1) || (j > this->NumClasses) ||
      (i<1) || (i > this->NumClasses) ||
      (k<1) || (k > 6) || (value < 0) || (value > 1)) {
     vtkErrorMacro (<< "Error: Incorrect input");
    return;
  }
  this->MrfParams[k-1][j-1][i-1] = value;
}; 

//----------------------------------------------------------------------------
// The EM Algorithm based on Tina Kapur PhD Thesis and the Matlab 
// implementation from Carl-Fredrik Westin, Mats Bjornemo and Anders Brun
void vtkImageEMSegmenter::vtkImageEMAlgorithm(double *InputOutputVector,int imgX,int imgY, int  NumSlices, int ImageMax) {
  cout << "vtkImageEMAlgorithm: Initialize Variables" << endl;
  // ------------------------------------------------------
  // General Variables 
  // ------------------------------------------------------
  int i,j,k,l;
  int imgXY     = imgX*imgY; 
  int imgXYPlus = imgXY +1;
  int imgProd   = imgXY*NumSlices;
  int iter,regiter;
 
  // ------------------------------------------------------
  // Setting up Class Distribution 
  // Calculate the mean of the log(greyvalue+1), 
  // currently we have the mean over the grey scale values !
  // ------------------------------------------------------
  int iMax = ImageMax+750;
  double *MuLog    = new double[this->NumClasses], *MuLogPtr    = MuLog; 
  double *SigmaLog = new double[this->NumClasses], *SigmaLogPtr = SigmaLog;
  double *ProbSum  = new double[this->NumClasses], *ProbSumPtr  = ProbSum;
  double *MuPtr = this->Mu, *SigmaPtr = this->Sigma, *ProbPtr = this->Prob, *InputOutputVectorPtr = InputOutputVector; 
  for (k=0; k < this->NumClasses; k++) {
    *MuLog++    = 0; 
    *SigmaLog++ = 0;
    *ProbSum++  = 0;
  }
  MuLog = MuLogPtr; SigmaLog = SigmaLogPtr; ProbSum = ProbSumPtr; 

  double *LogPlus  = new double[iMax], *LogPlusPtr = LogPlus; 
  double  **ProbMatrix = new double*[this->NumClasses];
  for (k=0; k< this->NumClasses;k++) ProbMatrix[k] = new double[iMax];

  // The following is the same in Matlab as sum(p.*log(x+1)) with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 0; i < iMax; i++) {
    *LogPlus = log(i+1);
    for (k=0; k < this->NumClasses; k++) {
      ProbMatrix[k][i] = EMSegmenterGauss(i,*(this->Mu++),*(this->Sigma++));
      *MuLog++ +=  ProbMatrix[k][i]*(*LogPlus);
      *ProbSum++ += ProbMatrix[k][i]; 
    }
    LogPlus ++;
    this->Mu = MuPtr; this->Sigma = SigmaPtr; MuLog = MuLogPtr; ProbSum = ProbSumPtr;
  } 
  // Normalize Mu over psum
  LogPlus = LogPlusPtr;
  for (k=0; k < this->NumClasses; k++) {
      *MuLog /= *ProbSum;
      MuLog ++;ProbSum++;
  }
  MuLog = MuLogPtr; ProbSum = ProbSumPtr;

  // The following is the same in Matlab as sqrt(sum(p.*(log(x+1)-mulog).*(log(x+1)-mulog))/psum)
  // with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 0; i < iMax; i++) {
    for (k=0; k < this->NumClasses; k++) 
      *SigmaLog++ +=  ProbMatrix[k][i]*pow(*LogPlus - *MuLog++,2);
    MuLog = MuLogPtr;SigmaLog = SigmaLogPtr;
    LogPlus ++;
  } 
  LogPlus = LogPlusPtr;
  // Take the sqrt
  for (k=0; k < this->NumClasses; k++) {
      *SigmaLog =  sqrt(*SigmaLog / (*ProbSum++));
      SigmaLog++;
  }
  SigmaLog = SigmaLogPtr;
  ProbSum  = ProbSumPtr;

  // Calculate the inverse variance
  double *ivar = new double[this->NumClasses], *ivarPtr = ivar;
  for (i=0;i < this->NumClasses;i++) {
    *ivar++ = 1/((*SigmaLog) * (*SigmaLog)); // inverse variance, needed in emseg
    SigmaLog++;
  }
  SigmaLog = SigmaLogPtr;ivar = ivarPtr;

  // ------------------------------------------------------------
  // EM-MF Variables 
  // ------------------------------------------------------------
  // bias for emseg+mrf
  double *b_m = new double[imgProd], *b_mPtr = b_m;
  for (k=0; k < imgProd; k++) (*b_m++) = 0.0;
  b_m = b_mPtr;
 
  double  **w_m = new double*[imgProd], **w_mPtr = w_m;
  for (k=0; k< imgProd;k++) {
    w_m[k] = new double[this->NumClasses];
    for (i = 0; i < this->NumClasses; i++) w_m[k][i] = 0.0; // weights or posteriors at each iteration
  }
  double *cY_M = new double[imgProd], *cY_MPtr = cY_M;                       // cY_M correct log intensity
  // ------------------------------------------------------------
  // MF Step Variables 
  // ------------------------------------------------------------
  double NormXN, NormXP,NormYN,NormYP, NormZN, NormZP;
  double normRow;                                  // Norm the rows
  double *wxp = new double[this->NumClasses],*wxpPtr = wxp,*wxn = new double[this->NumClasses],*wxnPtr = wxn, *wyn = new double[this->NumClasses],*wynPtr = wyn,
         *wyp = new double[this->NumClasses],*wypPtr = wyp,*wzn = new double[this->NumClasses],*wznPtr = wzn, *wzp = new double[this->NumClasses],*wzpPtr = wzp;  
  double mp;  

  // ------------------------------------------------------------
  // M Step Variables 
  // ------------------------------------------------------------
  double lbound = (-(this->SmoothingWidth-1)/2); // upper bound = - lbound
  double *skern = new double[this->SmoothingWidth], *skernPtr = skern; 
  for (i=0; i < this->SmoothingWidth; i++) 
    // skern[i+1] = Gauss(lbound + i ,0,this->SmoothingSigma);
    *skern++ = EMSegmenterGauss(lbound + i ,0,this->SmoothingSigma);
  skern = skernPtr;

  double *** r_m  = new double**[NumSlices]; // weighted residuals
  double *** iv_m = new double**[NumSlices]; // weighted inverse covariances 
  for (k=0; k < NumSlices; k++) {
      r_m[k]  = new double*[imgY];
      iv_m[k] = new double*[imgY];
      for (i=0;i < imgY; i ++) {
	r_m[k][i]  = new double[imgX];
	iv_m[k][i] = new double[imgX];
      }
  }  
 
  // ------------------------------------------------------------
  // Start Algorithm 
  // ------------------------------------------------------------
  for (iter=1; iter <= this->NumIter;iter++){
    cout << "vtkImageEMAlgorithm: "<< iter << ". Estep " << endl;
    // -----------------------------------------------------------
    // E-Step
    // -----------------------------------------------------------
    // cY_m = abs(Y - b_m(:));  // corrected log intensities
    for (k=0; k< imgProd; k++) { 
      *cY_M = abs(*InputOutputVector - *b_m);
      cY_M++;InputOutputVector++;b_m++;
    }
    cY_M = cY_MPtr; InputOutputVector = InputOutputVectorPtr;b_m = b_mPtr;

    // Ininitialize first iteration
    // This is the EM Algorithm with out MF Part -> The Regulizing part is the part where 
    // the MF part is added
 
    if (iter == 1) { 
      for (k=0; k < imgProd; k++) {  
        normRow = 0;
        for (j=0; j < this->NumClasses; j++) {
	  (*w_m)[j] = (*this->Prob++)*(exp(*cY_M)*EMSegmenterGauss(*cY_M, *MuLog++,*SigmaLog++));
          normRow += (*w_m)[j];
	}
	cY_M++;
	this->Prob = ProbPtr;MuLog = MuLogPtr;SigmaLog = SigmaLogPtr;
        // Normalize Rows and find Maxium of every Row and write it in Matrix
        for (j=0; j < this->NumClasses; j++) {(*w_m)[j] /= normRow;}
	w_m++;
      }
      cY_M = cY_MPtr;w_m = w_mPtr;
    }
    // -----------------------------------------------------------
    // regularize weights using a mean-field approximation
    // -----------------------------------------------------------
    for (regiter=1; regiter <= this->NumRegIter; regiter++) {
      cout << "vtkImageEMAlgorithm: "<< regiter << ". MFA Iteration" << endl;
      // -----------------------------------------------------------
      // neighbouring field values
      // -----------------------------------------------------------
      // The different Matrisses are defined as follow:
      // Matrix[i] = Neighbour in a certain direction
      // In Tcl TK defined in this order: West North Up East South Down 
      // this->MrfParams[i] :  
      // i = 3 pixel and east neighbour
      // i = 0 pixel and west neighbour
      // i = 4 pixel and south neighbour
      // i = 1 pixel and north neighbour
      // i = 2 pixel and previous neighbour (up)
      // i = 5 pixel and next neighbour (down)
      for(j = 0; j<  NumSlices; j++) {
        for(l = 1 ; l< imgXYPlus; l++) {
	  NormXN = NormXP = NormYN = NormYP = NormZN = NormZP = 0; 
	  for (i=0;i<this->NumClasses ;i++){
	    *wxp=*wxn=*wyn=*wyp=*wzn=*wzp=0;

	    for (k=0;k<this->NumClasses ;k++){
	      // f(i,j-1,k)
              if (l > imgY)           *wxn += w_m[-imgY][k]*this->MrfParams[3][k][i];
	      else                    *wxn += (*w_m)[k]*this->MrfParams[3][k][i]; 
	      // f(i,j+1,k)
	      if (l < imgXYPlus-imgY) *wxp += w_m[imgY][k]*this->MrfParams[0][k][i]; 
	      else                    *wxp += (*w_m)[k]*this->MrfParams[0][k][i];
	      // f(i-1,j,k)
	      if ((l-1)%imgY)         *wyn += w_m[-1][k]*this->MrfParams[4][k][i];  
	      else                    *wyn += (*w_m)[k]*this->MrfParams[4][k][i];   	      	      
	      // f(i+1,j,k)
	      if (l%imgY)             *wyp += w_m[1][k]*this->MrfParams[1][k][i];
	      else                    *wyp += (*w_m)[k]*this->MrfParams[1][k][i];
	      // f(i,j,k-1)
	      if (j > 0)              *wzn += w_m[-imgXY][k]*this->MrfParams[5][k][i]; 
	      else                    *wzn += (*w_m)[k]*this->MrfParams[5][k][i];  
	      // f(i,j,k+1)
	      if (j < (NumSlices -1)) *wzp += w_m[imgXY][k]*this->MrfParams[2][k][i]; 
	      else                    *wzp += (*w_m)[k]*this->MrfParams[2][k][i]; 
	    }
	    NormXN += *wxn++;
	    NormXP += *wxp++;
	    NormYN += *wyn++;
	    NormYP += *wyp++;
	    NormZN += *wzn++;
	    NormZP += *wzp++;
	  }
	  wxp = wxpPtr; wxn = wxnPtr; wyn = wynPtr; wyp = wypPtr; wzn = wznPtr; wzp = wzpPtr;
	  // mp = (ones(prod(imS),1)*p).*(1-alpha + alpha*wxn).*(1- alpha +alpha*wxp)...
	  //.*(1- alpha + alpha*wyp).*(1-alpha + alpha*wyn).*(1- alpha + alpha*wzp).*(1-alpha + alpha*wzn);
	  // w have to be normalized !
          normRow = 0;
	  for (i=0; i<this->NumClasses; i++) {
	    mp = (*this->Prob++)*(1-this->Alpha+this->Alpha*(*wxp++/NormXP))*(1-this->Alpha+this->Alpha*(*wxn++/NormXN))
			        *(1-this->Alpha+this->Alpha*(*wyp++/NormYP))*(1-this->Alpha+this->Alpha*(*wyn++/NormYN))
			        *(1-this->Alpha+this->Alpha*(*wzp++/NormZP))*(1-this->Alpha+this->Alpha*(*wzn++/NormZN));
	    // w_m(:,i) = mp(:,i).*(exp(cY_m).*Gauss(cY_m, mu(i)*ones(size(X)),sigma(i)));
	    // w_m = w_m ./ repmat(eps +sum(w_m,2),[1,num_classes]);
	    // mp is not normalized right now  to be normalized !
	    (*w_m)[i] = mp*(exp(*cY_M)*EMSegmenterGauss(*cY_M, *MuLog++,*SigmaLog++));
	    normRow += (*w_m)[i];
	  } 
          cY_M ++;
	  wxp = wxpPtr; wxn = wxnPtr; wyn = wynPtr; wyp = wypPtr; wzn = wznPtr; wzp = wzpPtr;
	  this->Prob = ProbPtr; MuLog = MuLogPtr; SigmaLog = SigmaLogPtr;

	  for (i=0; i< this->NumClasses; i++) {
	      (*w_m)[i] /= normRow;       
	  }
	  w_m++;
	}
      }
      cY_M = cY_MPtr;w_m = w_mPtr;
    }

    if ((this->PrintIntermediateResults) && ((iter%this->PrintIntermediateFrequency) == 0)){
	cout << "vtkImageEMAlgorithm: Print intermediate result " << endl;
	PrintMatlabGraphResults(iter,this->PrintIntermediateSlice,1, imgXY, imgY, imgX, w_m, b_m);
	cout << "vtkImageEMAlgorithm: Return to Algorithm " << endl;
    } 

    if (iter < this->NumIter) {
      cout << "vtkImageEMAlgorithm: Mstep " << endl;
      // -----------------------------------------------------------
      // M-step
      // -----------------------------------------------------------
      // compute weighted residuals 
      // r_m  = (w_m.*(repmat(cY_M,[1 num_classes]) - repmat(mu,[prod(imS) 1])))*(ivar)';
      // iv_m = w_m * ivar';
      for (k = 0; k< NumSlices;k++){
	for (j = 0; j<imgX;j++){
	  for (i = 0; i<imgY;i++){
	    r_m[k][i][j] = 0;
	    iv_m[k][i][j] =0;
	    for (l=0; l<this->NumClasses; l++) {
	      r_m[k][i][j]  += (*w_m)[l]*(*cY_M - *MuLog++)*(*ivar);
	      iv_m[k][i][j] += (*w_m)[l] * (*ivar++);
	    }
	    MuLog = MuLogPtr;ivar = ivarPtr;
            cY_M++;w_m++;
	  }
	}
      }
      cY_M = cY_MPtr;w_m = w_mPtr;
      //  smooth residuals and inv covariances - 3D
      // w(k) = sum(u(j)*v(k+1-j))
      this->smoothConv(r_m,NumSlices,imgY,imgX,skern,this->SmoothingWidth);
      this->smoothConv(iv_m,NumSlices,imgY,imgX,skern,this->SmoothingWidth);

      // estimate the smoothed bias
      // transform r (smoothed weighted residuals) by iv (smoother inv covariances)
      // b_m = r_m./iv_m ;
      for (k = 0; k<NumSlices;k++){
	for (j = 0; j<imgX;j++){
	  for (i = 0; i<imgY;i++){
	    if (!iv_m[k][i][j]) iv_m[k][i][j]= 2.2204e-14;
	    (*b_m++) = r_m[k][i][j] / iv_m[k][i][j];
	  }
	}      
      }
      b_m = b_mPtr;
    } else {
      // -----------------------------------------------------------
      // Copy results to Image
      // -----------------------------------------------------------
      // Find out the maximum propability assigned to a certain class 
      // and assign that pixel to that class
      this->DeterminLabelMap(InputOutputVector, w_m, imgX,imgY, NumSlices,imgXY);
    }    
  }
  delete []wxp;delete []wxn;delete []wyn;delete []wyp;delete []wzn;delete []wzp;
  delete []cY_M;
  delete []MuLog;  
  delete []SigmaLog;
  delete []ProbSum;
  delete []LogPlus;
  delete []ivar;
  for (k=0; k< imgProd;k++) delete[] w_m[k];
  delete []w_m;
  for (k=0; k< this->NumClasses;k++) delete[] ProbMatrix[k];
  delete []ProbMatrix;
  delete []b_m;

  for (k=0; k< NumSlices; k ++) { 
    for (i=0; i < imgY; i++) {
      delete[] r_m[k][i];
      delete[] iv_m[k][i];
    }
    delete[] r_m[k];
    delete[] iv_m[k];
  }
  delete[] r_m;
  delete[] iv_m;

  cout << "vtkImageEMAlgorithm: Finished " << endl;

}
 
// -----------------------------------------------------------
// Print out results into a file "EMSegmResult<name>.m"
// Results can be display with Matlab Program listed below 
// -----------------------------------------------------------
void vtkImageEMSegmenter::PrintMatlabGraphResults(int iter,int slice,int FullProgram,int imgXY, int imgY, int imgX, double **w_m,double *b_m) {
  char filename[40];
  char weightname[40];
  int appendFlag = 0;
  int c;

  double *weight = new double[imgXY];
  double *ind_Vector = new double [imgXY];
  w_m += imgXY*(slice-1); 
  this->DeterminLabelMap(ind_Vector, w_m, imgX,imgY, 1,imgXY);
  sprintf(filename,"EMSegmResult%dImage.m",iter);
  this->WriteVectorToFile(filename,"Image",ind_Vector, imgXY);  

  c = 0;
  while (c < this->NumClasses) { 
    // Retrieve the vector with certain slice and class from w_m
    for (int i = 0; i < imgXY; i++) weight[i] = w_m [i][c];
    c++;
    sprintf(weightname,"weight(%d,:)",c);
    sprintf(filename,"EMSegmResult%dWeight%d.m",iter,c);
    this->WriteVectorToFile(filename,weightname,weight,imgXY);
  }

  sprintf(filename,"EMSegmResult%db_m.m",iter);
  b_m += imgXY*(slice-1); 
  this->WriteVectorToFile(filename,"b_m",b_m,imgXY);

  if (FullProgram) {
    sprintf(filename,"EMSegmResult%d.m",iter);
    FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
    if ( f == NULL ) {
      cerr << "Could not open file " << filename << "\n";
      return;
    }

    fprintf(f,"%% (c) Copyright 2002 Massachusetts Institute of Technology\n");
    fprintf(f,"%% Result of the %d iteration of the EM Algorithm\n",iter);
    fprintf(f,"EMSegmResult%dImage\n",iter);
    fprintf(f,"figure(%d1)\n",iter);
    fprintf(f,"set(%d1, 'position', [10 445 300 300]);\n",iter);
    fprintf(f,"imagesc([reshape(Image(:),[%d %d])]);\n",imgY, imgX);
    fprintf(f,"title('Label Map - Iteration %d');\n", iter);
    // fprintf(f,"colormap([0 0 1; 0 1 0; 1 0 0; 1 1 0;]);\n");
    fprintf(f,"colormap('default');\n");
    fprintf(f,"\n");

    fprintf(f,"figure(%d2);\n",iter);
    fprintf(f,"set(%d2, 'position', [10 10 1210 360]);\n",iter);
    fprintf(f,"colormap(gray); \n");
    fprintf(f,"dwm=[];\n");
    for (c = 1; c <= this->NumClasses ; c++ ) 
      fprintf(f,"EMSegmResult%dWeight%d\n",iter,c); 
    fprintf(f,"for t=1:%d\n",this->NumClasses);
    fprintf(f,"  temp_m=reshape(weight(t,:),[%d %d]);\n",imgY, imgX); 
    fprintf(f,"  dwm = [dwm,  temp_m(:,:)];\n");
    fprintf(f,"end\n");
    fprintf(f,"imagesc([dwm]);\n");
    fprintf(f,"title('Weights - Iteration %d');\n", iter);
    fprintf(f,"\n");

    fprintf(f,"EMSegmResult%db_m\n",iter);
    fprintf(f,"figure(%d3);\n",iter);
    fprintf(f,"set(%d3, 'position', [600 445 300 300]);\n",iter);
    fprintf(f,"colormap(gray);\n"); 
    fprintf(f,"imagesc([reshape(b_m(:),[%d %d])]);\n",imgY, imgX);
    fprintf(f,"title('Bias - Iteration %d');\n",iter);
    fflush(f);
    fclose(f);
  }
  delete[] weight;
  delete[] ind_Vector;
}

void vtkImageEMSegmenter::DeterminLabelMap(double* LabelMap, double **w_m, int imgX,int imgY, int NumSlices,int imgXY) { 
  int idxZ,idxY, idxR,l,MaxProbIndex;
  int index = 0;
  for (idxZ = 0; idxZ < NumSlices ; idxZ++) {
    for (idxY = 0; idxY < imgY; idxY++) {
      for (idxR = 0; idxR < imgX; idxR++) {
	MaxProbIndex = 0;
	for (l=1; l<this->NumClasses; l++) if (w_m[index][l] > w_m[index][MaxProbIndex]) MaxProbIndex = l;
	*(LabelMap++) = (double) this->Label[MaxProbIndex];
	index++;
      }
    }
  }
}

// Special Vector and Matrix functions
void vtkImageEMSegmenter::DeleteVariables() {
  int z,y;
  if (this->NumClasses > 0 ){
    delete[] this->Mu; 
    delete[] this->Sigma;
    delete[] this->Prob;
    delete[] this->Label;

    for (z=0; z< 6; z ++) { 
      for (y=0; y < this->NumClasses; y++)
	delete[] this->MrfParams[z][y];
      delete[] this->MrfParams[z];
    }
    delete[] this->MrfParams;

    this->Mu = NULL;this->Sigma = NULL;this->Prob = NULL;this->Label = NULL;this->MrfParams = NULL;
  } 
}

// Opens up a new file and writes down result in the file
void vtkImageEMSegmenter::WriteVectorToFile (char *filename, char *varname,double *vec, int xMax) const {
  int appendFlag = 0;
  FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
  if ( f == NULL ) {
    cerr << "Could not open file " << filename << "\n";
    return;
  }
  this->WriteVectorToFile(f,varname,vec,xMax);
  fflush(f);
  fclose(f);
}

// Writes Vector to file in Matlab format if name is specified otherwise just 
// writes the values in the file
void vtkImageEMSegmenter::WriteVectorToFile (FILE *f,char *name, double *vec, int xMax) const {
  if (name != NULL) fprintf(f,"%s = [", name);
  for (int x = 0; x < xMax; x++ )
      fprintf(f,"%10.6f ", vec[x]);
  if (name != NULL) fprintf(f,"];\n");
}

// Opens up a new file and writes down result in the file
void vtkImageEMSegmenter::WriteMatrixToFile (char *filename,char *varname, double **mat, int imgY, int imgX) const {
  int appendFlag = 0;
  FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
  if ( f == NULL ) {
    cerr << "Could not open file " << filename << "\n";
    return;
  }
  this->WriteMatrixToFile(f,varname,mat,imgY,imgX);
  fflush(f);
  fclose(f);
}

// Writes Matrix to file in Matlab format if name is specified otherwise just 
// writes the values in the file
void vtkImageEMSegmenter::WriteMatrixToFile (FILE *f,char *name,double **mat, int imgY, int imgX) const
{
  if (name != NULL) fprintf(f,"%s = [", name);
  for (int y = 0; y < imgY; y++ ) {
    this->WriteVectorToFile(f,NULL,mat[y],imgX);
    if (y < (imgY-1)) fprintf(f,";\n");
  }
  if (name != NULL) fprintf(f,"];\n");
  fprintf(f,"\n");
}

//  Smoothes  3D-Matrix
// w(k) = sum(u(j)*v(k+1-j))
// returns Matrix of size r_m
void vtkImageEMSegmenter::smoothConv(double ***mat3D, int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen) {
  int i,k;
  
  double *** resultY  = new double**[mat3DZlen]; 
  double *** resultX  = new double**[mat3DZlen];
  for (k=0; k < mat3DZlen; k++) {
      resultY[k]  = new double*[mat3DYlen];
      resultX[k]  = new double*[mat3DYlen];
      for (i=0;i < mat3DYlen; i ++) {
	resultY[k][i]  = new double[mat3DXlen];
	resultX[k][i]  = new double[mat3DXlen];
      }
  }  

  // First: convolut in Y Direction 
  for (i = 0; i < mat3DZlen; i++){
    this->convMatrix(resultY[i],mat3D[i],mat3DYlen,mat3DXlen,v,vLen); 
  }
  // Second: convolut in X Direction 
  for (i = 0; i < mat3DZlen; i++){
    this->convMatrixT(resultX[i],resultY[i],mat3DYlen,mat3DXlen,v,vLen); 
  }
  // Third: in Z direction
  this->convMatrix3D(mat3D,resultX,mat3DZlen,mat3DYlen,mat3DXlen,v,vLen);

  for (k=0; k< mat3DZlen; k ++) { 
    for (i=0; i < mat3DYlen; i++) {
      delete[] resultY[k][i];
      delete[] resultX[k][i];
    }
    delete[] resultY[k];
    delete[] resultX[k];
  }
  delete[] resultY;
  delete[] resultX;
}

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
void vtkImageEMSegmenter::convMatrix3D(double*** mat3D, double*** U,int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen) {
  int stump = vLen /2;
  int k,j,jMin,jMax,x,y;
  int kMax = mat3DZlen + stump;
  double ***USta = U;
  double *vSta = v;

  for (k = stump; k <  kMax; k++) {
    for (y = 0; y < mat3DYlen; y++) {
	for (x = 0; x < mat3DXlen; x++) 
	  (*mat3D)[y][x] = 0;
    }
    jMin = (0 > (k+1 - vLen) ? 0 : (k+1  - vLen));     //  max(0,k+1-vLen):
    jMax = ((k+1) < mat3DZlen ? (k+1) : mat3DZlen);     //  min(k+1,mat3DZlen) 
    // this->mat3D[k-stump] += U[j]*v[k-j];
    U = USta + jMin;  v = vSta + k-jMin; 
    for (j=jMin; j < jMax; j++) {
      for (y = 0; y < mat3DYlen; y++) {
	for (x = 0; x < mat3DXlen; x++)
	  (*mat3D)[y][x] += (*U)[y][x] * (*v);
      }
      v--;
      U++;
    }
    mat3D++ ;
  }  
}


// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
void vtkImageEMSegmenter::convMatrix(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen) {
  int i,j;

  // => Utrans[i] represents the i column of U;    
  double * Utrans  = new double[matYlen], *UtransPtr = Utrans; 
  double * result  = new double[matYlen], *resultPtr = result;

  for (i = 0; i < matXlen; i++) {
     for (j = 0; j < matYlen; j++) *(Utrans++) = U[j][i]; 
     Utrans = UtransPtr; 
     this->convVector(result,Utrans,matYlen,v,vLen); // Use the i-th Rows of Utrans; 
     for (j=0; j < matYlen; j++) mat[j][i] = *(result ++); // Write result to this->mat as i-th column
     result = resultPtr;
  }
  delete[] Utrans;
  delete[] result;
} 

// Same just v is a row vector instead of column one
// We use the following equation :
// conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
void vtkImageEMSegmenter::convMatrixT(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen) {
  int i;

  // Use the i-th Rows of U = ith Column of U';
  // write it to the i-th Row of 'this' => Transpose again
  for (i = 0; i < matYlen; i++) {
     this->convVector(mat[i],U[i],matXlen,v,vLen);  
  }
} 

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimensio
void vtkImageEMSegmenter::convVector(double vec[], double u[], int uLen, double v[], int vLen){
  int stump = vLen /2;
  int k,j,jMin,jMax;
  int kMax = uLen + stump;
  double *uSta =u, *vSta=v;  

  for (k = stump; k <  kMax; k++) {
    *vec = 0;
    jMin = (0 > (k - vLen +1) ? 0 : (k - vLen+1) ); // max(0,k-vLen+1):
    jMax = ((k+1) < uLen ? (k+1) : uLen) ;               // min(k,uLen)+1 
    u = uSta + jMin; v = vSta + k-jMin;
    for (j=jMin; j < jMax; j++) 
      *vec += (*u++) * (*v--);
    vec ++;
  }  
}


void vtkImageEMSegmenter::TestConv() { 
  int x=5,y=5,z=5;
  int i,k,l;

  double ***result = new double**[z]; // weighted inverse covariances 
  double ***resultY = new double**[z]; // weighted inverse covariances 
  double ***resultX = new double**[z]; // weighted inverse covariances 
  for (i=0; i < z ; i++) {
      result[i]  = new double*[y];
      resultY[i]  = new double*[y];
      resultX[i]  = new double*[y];
      for (k=0;k < y; k ++) {
	result[i][k] = new double[x];
	resultY[i][k] = new double[x];
	resultX[i][k] = new double[x];
	for (l=0;l < x; l ++) {
	  resultY[i][k][l] = 0;
	  resultX[i][k][l] = 0;
	}
      }
      
  }  

  for (k=0; k <z; k++) {
    for (i=0; i <y; i++)
      for (l=0; l <x; l++)
      result[k][i][l] = 1.0; 
  }

  double skern[3];
  for (i=0; i <3; i++)
    skern[i] = i+1;
  cout <<"Vector " << endl;

  //  this->convVector(resultY[0][0],result[0][0],x,skern,3);
  // for (i = 0; i < x; i++) cout << resultY[0][0][i] << " ";
  // cout << endl;

  // First: convolut in Y Direction 
  cout << "Y Convolution" << endl;
  for (i = 0; i < z; i++){
    this->convMatrix(resultY[i],result[i],y,x,skern,3); 
    this->PrintMatrix(resultY[i],y,x);
  }
  cout << "X convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 0; i < z; i++) {
    this->convMatrixT(resultX[i],resultY[i],y,x,skern,3); 
    this->PrintMatrix(resultX[i],y,x);
  }
  // Third: in Z direction
  this->convMatrix3D(resultY,resultX,z,y,x,skern,3);
  cout << "Z convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 0; i < z; i++) {
    this->PrintMatrix(resultY[i],y,x);
  }
  this->smoothConv(result,z,y,x,skern,3);
  cout << "Alltogether convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 0; i < z; i++) {
    this->PrintMatrix(result[i],y,x);
  }

  for (k=0; k< z; k ++) { 
    for (i=0; i < y; i++) {
      delete[] result[k][i];
      delete[] resultY[k][i];
      delete[] resultX[k][i];

    }
    delete[] result[k];
    delete[] resultY[k];
    delete[] resultX[k];
  }
  delete[] result;
  delete[] resultY;
  delete[] resultX;
}

void vtkImageEMSegmenter::PrintMatrix(double **mat, int yMax,int xMax) {
  int i;
  for (int y = 0; y < yMax; y++) {
      for (i = 0; i < xMax; i++)
	cout << mat[y][i] << " ";
      cout << endl;
  }
  cout << endl;
}

