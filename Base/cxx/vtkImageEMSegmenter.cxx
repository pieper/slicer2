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
  this->PrintIntermediateResults = 0 ; // Print intermediate results in a Matlab File (No = 0, Yes = 1)
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
  int StartSlice = self->get_StartSlice();
  int EndSlice = self->get_EndSlice();
  int StartEndSlice; 
  

  // find the region to loop over
  rowLength = (outExt[1] - outExt[0]+1)*in1Data->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
  maxZ = outExt[5] - outExt[4] + 1; // outExt[5/4] = Relative Maximum/Minimum Slice index  

  // Making sure values are ste correctly
  if (1 > StartSlice) StartSlice = 1;
  if (maxZ < EndSlice) EndSlice = maxZ; 
  else if (StartSlice > EndSlice) EndSlice = StartSlice -1;  
  StartEndSlice = EndSlice - StartSlice + 1;

  // Get increments to march through data 
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // The Slices of the images 
  vtkImageEMMatrix3D Volume(StartEndSlice,maxY,rowLength);    
  
  // Loop through input pixels 

  // 1.) Read thorugh beginning slides we do not want to read through for input and output
  for (idxZ = 1; idxZ < StartSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = (T) (*in1Ptr);
	in1Ptr++;
	outPtr++;
      }
      in1Ptr += inIncY;
      outPtr += outIncY;
    }
    in1Ptr += inIncZ;
    outPtr += outIncZ; 
  }

  // 2.) Read thorugh the slides that should be segmented 
  StartEndSlice ++;
  rowLength ++;
  double Max = ((double)*in1Ptr);
  double Min = ((double)*in1Ptr);

  // I like to see my pictures the right way around !
  for (idxZ = 1; idxZ < StartEndSlice ; idxZ++) {
    for (idxY = maxY; idxY > 0; idxY--) {
      for (idxR = 1; idxR < rowLength; idxR++) {
	// Copied from  vtkMrmlSlicer::GetBackPixel
	// in1data->GetWholeExtent(ext);
	// in1Data->GetPointData()->GetScalars()->GetScalar(y*(ext[1]-ext[0]+1)+x);
	// Pixel operaton
	Volume[idxZ][idxY][idxR] = ((double) * in1Ptr);
    
        if (((double)*in1Ptr) > Max) Max = ((double)*in1Ptr);
	else if (((double)*in1Ptr) < Min) Min = ((double)*in1Ptr);
	in1Ptr++;
      }
      in1Ptr += inIncY;
    }
    in1Ptr += inIncZ;
  }
  // 3.) Run Algorith, 
  // Bullshit for testing 
  if (self->get_ImgTestNo() < 1) self->vtkImageEMAlgorithm(Volume,Max);
  else Volume.setMatrix3DTest(self->get_ImgTestNo(),self->get_ImgTestDivision (),self->get_ImgTestPixel());
  // 4.) Write result in outPtr
  for (idxZ = 1; idxZ < StartEndSlice; idxZ++) {
    for (idxY = maxY; idxY > 0; idxY--) {
      for (idxR = 1; idxR < rowLength; idxR++) {
	*outPtr = (T) ((int) Volume[idxZ][idxY][idxR]);
	// *outPtr = (T) (1);
	outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ;
  }
  // 5.) Run through the end slides that were not segmented
  StartEndSlice = maxZ - (StartEndSlice + StartSlice-2) + 1;
  for (idxZ = 1; idxZ < StartEndSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 1; idxR < rowLength; idxR++) {
	*outPtr = (T) (*in1Ptr);
	in1Ptr++;
	outPtr++;
      }
      in1Ptr += inIncY;
      outPtr += outIncY;
    }
    in1Ptr += inIncZ;
    outPtr += outIncZ; 
  }
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
  vtkImageEMMatrix Test(10,10);
  this->NumClasses = NumberOfClasses;
  // Delete Old instances if they existed
  this->Mu.Resize(this->NumClasses,-1.0);
  this->Sigma.Resize(this->NumClasses,-1.0); 
  this->Prob.Resize(this->NumClasses,-1.0);
  this->MrfParams.Resize(6,this->NumClasses,this->NumClasses,-1.0);
  this->Label.Resize(this->NumClasses,-1.0);
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

int vtkImageEMSegmenter::checkValues(vtkImageEMMatrix3D Volume)
{
  int i=0,j,k;
  if (Volume.get_dim(1) != (this->EndSlice - this->StartSlice + 1)) {
    cout << "vtkImageEMSegmenter:checkValues:  Volume does not have the right dimension !" << endl;
    return -1;
  }
  while (i < this->NumClasses) {
    i++;
    if (this->Mu[i] < 0) {
      cout << "vtkImageEMSegmenter:checkValues:  Mu[" << i <<"] = " << this->Mu[i] << " must be greater than 0!" << endl;
      return -2;
    }
    if (this->Sigma[i] < 0)  {
      cout << "vtkImageEMSegmenter:checkValues:  Sigma[" << i <<"] = " << this->Sigma[i] << " is not greater than 0!" << endl;
      return -3;
    }
    if ((this->Prob[i] < 0) || (this->Prob[i] > 1.0)) {
      cout << "vtkImageEMSegmenter:checkValues:  Prob[" << i <<"] = " << this->Prob[i] << " is not between 0 and 1!" << endl;
      return -4;
    }
    if (this->Label[i] < 0) {
      cout << "vtkImageEMSegmenter:checkValues:  Color[" << i <<"] = " << this->Label[i] << " is not defined" << endl;
      return -6;
    }
    for (j = 1; j <= this->NumClasses; j++) {
      for (k = 1; k <= 6; k++) {
	if ((this->MrfParams[k][j][i] < 0) || (this->MrfParams[k][j][i] > 1)) {
	  cout << "vtkImageEMSegmenter:checkValues:  MrfParams[" << k <<"] [" << j <<"] [" << i <<"] = " << this->MrfParams[k][j][i] 
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
void vtkImageEMSegmenter::SetProbability(double prob, int index) {
  if ((index<1) || (index > this->NumClasses) ||
      (prob < 0) || (prob > 1)) {
    vtkImageEMError("vtkImageEMSegmenter::SetProbability: Incorrect input");
    return;
  }
  this->Prob[index] = prob;
}

void vtkImageEMSegmenter::SetMu(double mu, int index){
  if ((index<1) || (index > this->NumClasses) ||
      (mu < 0)) {
    vtkImageEMError("vtkImageEMSegmenter::SetMu: Incorrect input");
    return;
  }
  this->Mu[index] = mu;
}

void vtkImageEMSegmenter::SetSigma(double sigma, int index){
  if ((index<1) || (index > this->NumClasses) || (sigma < 0)) {
    vtkImageEMError("vtkImageEMSegmenter::SetSigma: Incorrect input");
    return;
  }
  this->Sigma[index] = sigma;
}

void vtkImageEMSegmenter::SetLabel(int label, int index){
  if ((index<1) || (index > this->NumClasses) || (label < 0)) {
    vtkImageEMError("vtkImageEMSegmenter::SetColor: Incorrect input");
    return;
  }
  this->Label[index] = label;
}

void vtkImageEMSegmenter::SetMarkovMatrix(double value, int k, int j, int i) {
  if ((j<1) || (j > this->NumClasses) ||
      (i<1) || (i > this->NumClasses) ||
      (k<1) || (k > 6) ||
      (value < 0) || (value > 1)) {
    vtkImageEMError("vtkImageEMSegmenter::SetMarkovMatrix: Incorrect input");
    return;
  }
  this->MrfParams[k][j][i] = value;
}; 

//----------------------------------------------------------------------------
// The EM Algorithm based on Tina Kapur PhD Thesis and the Matlab 
// implementation from Carl-Fredrik Westin, Mats Bjornemo and Anders Brun

void vtkImageEMSegmenter::vtkImageEMAlgorithm(vtkImageEMMatrix3D & Image,int ImageMax) {

  cout << "vtkImageEMAlgorithm: Initialize Variables" << endl;
  // Check consistency of all veluyes and start with output 
  if (this->checkValues(Image) < 1) return;

  int i,j,k,l,x,y,index,iter;
  // Image is actually a 3Dim Array im(Y,X,Slice) 
  // imS=[imgY,imgX,NumSlices] 
  int imgX = Image.get_dim(3), imgY = Image.get_dim(2); 
  int NumSlices = this->EndSlice - this->StartSlice + 1;
  int imgYPlus= imgY+1, imgXPlus= imgX+1, NumSlicesPlus= NumSlices +1;
  int imgXY   = imgX*imgY; 
  int imgXYPlus = imgXY +1;
  int imgProd = imgXY*NumSlices;
  int imgProdPlus = imgProd+1;
  int NumClassPlus = this->NumClasses+1;

  // ------------------------------------------------------
  // Calculate the mean of the log(greyvalue+1), 
  // currently we have the mean over the grey scale values !
  // ------------------------------------------------------
  vtkImageEMVector MuLog(this->NumClasses,0.0) ; 
  vtkImageEMVector SigmaLog(this->NumClasses,0.0) ; 
  vtkImageEMVector ProbSum(this->NumClasses,0.0);
  double Prob;
  int iMax = ImageMax+750;
  vtkImageEMVector LogPlus(iMax) ;
  vtkImageEMMatrix ProbMatrix(this->NumClasses,iMax);
  // The following is the same in Matlab as sum(p.*log(x+1)) with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 1; i < iMax; i++) {
    LogPlus[i] = log(i);
    for (k=1; k < NumClassPlus; k++) {
      ProbMatrix[k][i] = Gauss(i-1,this->Mu[k],this->Sigma[k]);
      MuLog[k] +=  ProbMatrix[k][i]*LogPlus[i];
      ProbSum[k] += ProbMatrix[k][i];
    }
  } 
  // Normalize Mu over psum  
  for (k=1; k < NumClassPlus; k++) {
      MuLog[k] /= ProbSum[k];
  }
  // Kilian : For Debugging Reasons 
  // MuLog[1] = 5.8199;MuLog[2]= 5.4476;MuLog[3] = 5.5954; MuLog[4] = 3.0651;

  // The following is the same in Matlab as sqrt(sum(p.*(log(x+1)-mulog).*(log(x+1)-mulog))/psum)
  // with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 1; i < iMax; i++) {
    for (k=1; k < NumClassPlus; k++)
      SigmaLog[k] +=  ProbMatrix[k][i]*pow(LogPlus[i]-MuLog[k],2);
  } 
  // Take the sqrt
  for (k=1; k < NumClassPlus; k++) {
      SigmaLog[k] =  sqrt(SigmaLog[k] / ProbSum[k]);
  }

  // Kilian : For Debugging Reasons 
  // SigmaLog[1] = 0.1348;SigmaLog[2]=0.2632;SigmaLog[3]= 0.1168; SigmaLog[4]=0.2386;


  // ------------------------------------------------------
  // Setting up more paramters!
  // ------------------------------------------------------

  // Calculate the inverse variance
  vtkImageEMVector ivar(this->NumClasses);
  for (i=1;i < NumClassPlus;i++) {
    ivar[i] = 1/(SigmaLog[i]*SigmaLog[i]); // inverse variance, needed in emseg
    // cout << ivar[i] << endl;
  }

  vtkImageEMVector NormVe(6);
  // ------------------------------------------------------------
  // iterative optimization 
  // ------------------------------------------------------------
  // X is image im as a vector
  vtkImageEMVector X(imgProd);
  X.Reshape(Image);

  vtkImageEMMatrix3D b_m(NumSlices,imgY,imgX,0.0);      // bias for emseg+mrf
  vtkImageEMVector   r_m(imgProd,0.0);                  // weighted residuals
  vtkImageEMVector   iv_m(imgProd,0.0);                 // weighted inverse covariances
  vtkImageEMMatrix   w_m(imgProd,this->NumClasses,0.0); // weights or posteriors at each iteration
  // define rank 1 kernels for faster convolution
  double lbound = (-(this->SmoothingWidth-1)/2); // upper bound = - lbound
  vtkImageEMVector skern(this->SmoothingWidth); 
  for (i=0; i < this->SmoothingWidth; i++) {
     skern[i+1] = Gauss(lbound + i ,0,this->SmoothingSigma);
  }

  // skern.PrintVector();
  // Sx Sy Sz are all skern 
  vtkImageEMVector Y(imgProd);
  for (i=1;i<imgProdPlus;i++) Y[i] = log(X[i]+1); 

  vtkImageEMVector cY_M(imgProd);                  // cY_M correct log intensity
  double normRow;                                  // Norm the rows  
  double max_orig;                                 // Maximum of w_m rows
  int    ind_orig;                                 // Indice of maximum
  vtkImageEMMatrix3D seg_orig(NumSlices,imgY,imgX,0.0);  // Maximum of the row of Matrix w_m 
  vtkImageEMMatrix fxp(imgProd, this->NumClasses),fxn(imgProd, this->NumClasses), fyp(imgProd, this->NumClasses),
         fyn(imgProd, this->NumClasses), fzp(imgProd, this->NumClasses), fzn(imgProd, this->NumClasses);
  vtkImageEMMatrix wxp(imgProd,this->NumClasses),wxn(imgProd,this->NumClasses),wyn(imgProd,this->NumClasses),
         wyp(imgProd,this->NumClasses),wzn(imgProd,this->NumClasses),wzp(imgProd,this->NumClasses);
  vtkImageEMMatrix mp(imgProd,this->NumClasses);  
  vtkImageEMMatrix3D r_m3D(NumSlices,imgY,imgX), r_m3Dsmooth(NumSlices,imgY,imgX),
           iv_m3D(NumSlices,imgY,imgX),iv_m3Dsmooth(NumSlices,imgY,imgX);
  for (iter=1; iter <= this->NumIter;iter++){
    cout << "vtkImageEMAlgorithm: "<< iter << ". Estep " << endl;
    // -----------------------------------------------------------
    // E-Step
    // -----------------------------------------------------------
    // cY_m = abs(Y - b_m(:));  // corrected log intensities
    for (k=1; k< NumSlicesPlus; k++) {
      index = (k-1)*imgXY;
      for (i=1; i < imgXPlus; i++) {
        for (j=1; j < imgYPlus; j++) {
	  cY_M[index+j] = abs(Y[index+j] - b_m[k][j][i]);
	  //  cout << k << " " << j << " " << i << " " << index+k << " " << cY_M[index+k] << " = abs(" << Y[index+k] << "-" << b_m[k][j][i] << ") " << endl;
        }
        index +=imgY;
      }
    }
    // Debug
    // cout << "cY_M " << endl;
    // cY_M.PrintVector(800);

    // Ininitialize first iteration
    // This is the EM Algorithm with out MF Part -> The Regulizing part is the part where 
    // the MF part is added
 
    if (iter == 1) { 
      index = 0;
      for (k=1; k < imgProdPlus; k++) {  
        normRow = 0;
        ind_orig = 1;
        max_orig = w_m[k][1];
        for (j=1; j < NumClassPlus; j++) {
	  w_m[k][j] = this->Prob[j]*(exp(cY_M[k])*Gauss(cY_M[k], MuLog[j],SigmaLog[j]));
          normRow += w_m[k][j];
          if (max_orig < w_m[k][j]) {max_orig = w_m[k][j]; ind_orig = j; }
	}
        // Normalize Rows and find Maxium of every Row and write it in Matrix
        for (j=1; j < NumClassPlus; j++) {w_m[k][j] /= normRow;}
        // Assign Indice of Maximum Value to seg_orig
        if (((k-1)%imgY) == 0) {
	   if (((k-1)%imgXY) == 0) {index += 1; x = 1;
	   } else {x += 1;}
	}
        y = (k-1)%imgY+1; 
        seg_orig[index][y][x]  = ind_orig;
	// cout << index << " " <<  y <<  " " << x <<  " " <<  seg_orig[index][y][x] << endl;
      }
    }
    // cout << "W_M " << endl;
    // w_m.PrintMatrix(600);
    // cout << "seg_orig " << endl;
    // seg_orig.PrintMatrix3D(-1,5,100);
    
    // -----------------------------------------------------------
    // regularize weights using a mean-field approximation
    // -----------------------------------------------------------
    for (k=1; k <= this->NumRegIter; k++) {
      cout << "vtkImageEMAlgorithm: "<< k << ". MFA Iteration" << endl;
      // -----------------------------------------------------------
      // neighbouring field values
      // -----------------------------------------------------------
      //  f = reshape(w_m, [imgY imgX NumSlices num_classes]);      
      for (i = 1; i< NumClassPlus; i++) {
        for(j = 0; j<  NumSlices; j++) {
          index = j*imgXY;
          for(l = 1 ; l< imgXYPlus; l++) {
	    // f(i,j+1,k)
  	    if (l < imgXYPlus-imgY) fxp[l+index][i] = w_m[l+imgY+index][i];  
            else                 fxp[l+index][i] = w_m[l+index][i];  
	    // f(i,j-1,k)
            if (l < imgYPlus)    fxn[l+index][i] = w_m[l+index][i]; 
            else                 fxn[l+index][i] = w_m[l-imgY+index][i]; 
	    // f(i+1,j,k)
	    if (l%imgY)          fyp[l+index][i] = w_m[l+1+index][i];
	    else                 fyp[l+index][i] = w_m[l+index][i];
	    // f(i-1,j,k)
	    if ((l-1)%imgY)      fyn[l+index][i] = w_m[l-1+index][i];
	    else                 fyn[l+index][i] = w_m[l+index][i];
            // f(i,j,k+1)
	    if (j < (NumSlices -1)) fzp[l+index][i] = w_m[l+index+imgXY][i];
            else                 fzp[l+index][i] = w_m[l+index][i];
	    // f(i,j,k-1)
	    if (j == 0)          fzn[l+index][i] = w_m[l+index][i];
            else                 fzn[l+index][i] = w_m[l+index-imgXY][i];
	  }
        } 
      }
      //fxp.WriteMatrixToFile("fxpem.m","fxpem");
      //fxn.WriteMatrixToFile("fxnem.m","fxnem");
      //fyp.WriteMatrixToFile("fypem.m","fypem"); 
      //fyn.WriteMatrixToFile("fynem.m","fynem");
      //fzp.WriteMatrixToFile("fzpem.m","fzpem");
      //fzn.WriteMatrixToFile("fznem.m","fznem");

      // v = v ./ repmat((sum(v,2)+eps),[1 size(v,2)]);
      for (i=1;i < imgProdPlus;i++){
        NormVe[1] =NormVe[2]= NormVe[3]=NormVe[4]=NormVe[5]=NormVe[6]=0;

	for (j=1;j<NumClassPlus ;j++){
          wxp[i][j]=wxn[i][j]=wyn[i][j]=wyp[i][j]=wzn[i][j]=wzp[i][j]=0;


	  // The different Matrisses are defined as follow:
	  // Matrix[i] = Neighbour in a certain direction
	  // In Tcl TK defined in this order: West North Up East South Down 
	  // this->MrfParams[i] :  
	  // i = 4 pixel and east neighbour
          // i = 1 pixel and west neighbour
          // i = 5 pixel and south neighbour
          // i = 2 pixel and north neighbour
          // i = 3 pixel and previous neighbour (up)
          // i = 6 pixel and next neighbour (down)


          for (l=1;l<NumClassPlus ;l++){
            wxn[i][j] += fxn[i][l]*this->MrfParams[4][l][j];
            wxp[i][j] += fxp[i][l]*this->MrfParams[1][l][j];
            wyn[i][j] += fyn[i][l]*this->MrfParams[5][l][j];  
	    wyp[i][j] += fyp[i][l]*this->MrfParams[2][l][j];
	    wzn[i][j] += fzn[i][l]*this->MrfParams[6][l][j];  
	    wzp[i][j] += fzp[i][l]*this->MrfParams[3][l][j];
            
	  }
	  NormVe[1] += wxp[i][j];
	  NormVe[2] += wxn[i][j];
	  NormVe[3] += wyn[i][j];
          NormVe[4] += wyp[i][j];
	  NormVe[5] += wzn[i][j];
	  NormVe[6] += wzp[i][j];
	}
          // Normalize 
        for (j=1;j<NumClassPlus ;j++){
	  wxp[i][j] /= NormVe[1];
	  wxn[i][j] /= NormVe[2];
	  wyn[i][j] /= NormVe[3];
	  wyp[i][j] /= NormVe[4];
	  wzn[i][j] /= NormVe[5];
	  wzp[i][j] /= NormVe[6];
	}
      }
      // wxp.WriteMatrixToFile("wxpem.m","wxpem");
      // wxn.WriteMatrixToFile("wxnem.m","wxnem");
      // wyp.WriteMatrixToFile("wypem.m","wypem"); 
      // wyn.WriteMatrixToFile("wynem.m","wynem");
      // wzp.WriteMatrixToFile("wzpem.m","wzpem");
      // wzn.WriteMatrixToFile("wznem.m","wznem");

      // mp = (ones(prod(imS),1)*p).*(1-alpha + alpha*wxn).*(1- alpha +alpha*wxp)...
      //.*(1- alpha + alpha*wyp).*(1-alpha + alpha*wyn).*(1- alpha + alpha*wzp).*(1-alpha + alpha*wzn); 
      for (i=1; i<imgProdPlus; i++){
        normRow = 0;
	for (j=1; j<NumClassPlus; j++) {
	  mp[i][j] = this->Prob[j]*(1-this->Alpha+this->Alpha*wxp[i][j])*(1-this->Alpha+this->Alpha*wxn[i][j])
	                    *(1-this->Alpha+this->Alpha*wyp[i][j])*(1-this->Alpha+this->Alpha*wyn[i][j])
	                    *(1-this->Alpha+this->Alpha*wzp[i][j])*(1-this->Alpha+this->Alpha*wzn[i][j]);
	  normRow += mp[i][j];
	}
        for (j=1; j<NumClassPlus; j++) {
          mp[i][j] /= normRow;
	}
      }
      // w_m(:,i) = mp(:,i).*(exp(cY_m).*Gauss(cY_m, mu(i)*ones(size(X)),sigma(i)));
      // w_m = w_m ./ repmat(eps +sum(w_m,2),[1,num_classes]);

      for (j=1; j<imgProdPlus; j++) {
        normRow = 0;
        for (i=1; i<NumClassPlus; i++) {
          w_m[j][i] = mp[j][i]*(exp(cY_M[j])*Gauss(cY_M[j], MuLog[i],SigmaLog[i]));
          normRow += w_m[j][i];
	}        
        for (i=1; i<NumClassPlus; i++) {
          w_m[j][i] /= normRow;
	}       
      }
    }

    if (this->PrintIntermediateResults) {
	cout << "vtkImageEMAlgorithm: Print intermediate result " << endl;
	this->DeterminLabelMap(Image, w_m, NumClassPlus, NumSlicesPlus, imgY, imgXY, imgXPlus, imgYPlus);
	PrintMatlabGraphResults(iter,1,1, imgXY, imgY, imgX,Image,w_m, b_m);
	cout << "vtkImageEMAlgorithm: Return to Algorithm " << endl;
    } 

    // mp.WriteMatrixToFile("mpem.m","mpem");
    // w_m.WriteMatrixToFile("w_mem.m","w_mem");

    if (iter < this->NumIter) {
      cout << "vtkImageEMAlgorithm: Mstep " << endl;
      // -----------------------------------------------------------
      // M-step
      // -----------------------------------------------------------

      // compute weighted residuals 
      // r_m  = (w_m.*(repmat(cY_M,[1 num_classes]) - repmat(mu,[prod(imS) 1])))*(ivar)';
      // iv_m = w_m * ivar';
      for (j=1; j<imgProdPlus; j++) {
	r_m[j] = 0;
	iv_m[j] =0;
	for (i=1; i<NumClassPlus; i++) {
	  r_m[j] += w_m[j][i]*(cY_M[j] - MuLog[i])*ivar[i];
	  iv_m[j] += w_m[j][i] * ivar[i];
	}
      }
  
      //  smooth residuals and inv covariances - 3D
      // w(k) = sum(u(j)*v(k+1-j))
      // returns Matrix of size r_m
      //cout << "r_m " << endl;
      //r_m.PrintVector(30);
      //r_m.WriteVectorToFile("r_mem.m","r_mem");
      //cout << "iv_m " << endl;
      //iv_m.PrintVector(30);
      // iv_m.WriteVectorToFile("iv_mem.m","iv_mem");
      r_m3D.Reshape(r_m);
      r_m3Dsmooth.smoothConv(r_m3D,skern);
      //cout << "rm_m3D " << endl;
      //r_m3Dsmooth[1].PrintMatrix(5,30);
      //r_m3Dsmooth[1].WriteMatrixToFile("r_m3Dem.m","r_m3Dmem");
      iv_m3D.Reshape(iv_m);
      iv_m3Dsmooth.smoothConv(iv_m3D,skern);
      //cout << "iv_m3D " << endl;
      //iv_m3Dsmooth[1].PrintMatrix(5,30);
      //iv_m3Dsmooth[1].WriteMatrixToFile("iv_m3Dem.m","iv_m3Dmem");
      // estimate the smoothed bias
      // transform r (smoothed weighted residuals) by iv (smoother inv covariances)
      // b_m = r_m./(iv_m + eps);
      for (k = 1; k<NumSlicesPlus;k++){
	for (i = 1; i<imgYPlus;i++){
	  for (j = 1; j<imgXPlus;j++){
	    b_m[k][i][j] = r_m3Dsmooth[k][i][j] / (iv_m3Dsmooth[k][i][j] + 2.2204e-14);
	  }
	}      
      }
      // cout << "b_m " << endl;
      // b_m[1].PrintMatrix(5,30);
      // b_m[1].WriteMatrixToFile("b_mem.m","b_mem");

    } else {
      // -----------------------------------------------------------
      // Copy results to Image
      // -----------------------------------------------------------
      // [foo ind_m] = max(w_m, [], 2);
      // Find out the maximum propability assigned to a certain class 
      // and assign that pixel to that class
      // Right now grey value is defind by Number of 256/ (classes)
      // Leter on it will be substiuted by colors
      if (!this->PrintIntermediateResults)
	this->DeterminLabelMap(Image, w_m, NumClassPlus, NumSlicesPlus, imgY, imgXY, imgXPlus, imgYPlus);
    }    
  }
  cout << "vtkImageEMAlgorithm: Finished " << endl;
}

double vtkImageEMSegmenter::Gauss(double x,double m,double s) { 
  return (1 / (sqrt(2*3.14159265358979)*s)*exp(-(x-m)*(x-m)/(2*pow(s,2))));
}
 
void vtkImageEMSegmenter::TestConv() { 
  double lbound = (-(this->SmoothingWidth-1)/2);
  // vtkImageEMVector skern(this->SmoothingWidth); 
  vtkImageEMVector skern(3); 
  int x=5,y=5,z=5;
  vtkImageEMMatrix3D r_m3D(z,y,x,0.0), r_m3Dsmooth(z,y,x,0.0), resultY(z,y,x,0.0), resultX(z,y,x,0.0); 

  int i,k,l;
  for (k=1; k <=z; k++) {
    for (i=1; i <=y; i++)
      for (l=1; l <=x; l++)
      r_m3D[k][i][l] = 1.0; 
  }
  for (i=1; i <4; i++)
    skern[i] = i;
  
  //for (i=0; i < this->SmoothingWidth; i++) {
  //   skern[i+1] = Gauss(lbound + i ,0,this->SmoothingSigma);
  //}

  // First: convolut in Y Direction 
  cout << "Y Convolution" << endl;
  for (i = 1; i <= z; i++){
    resultY[i].conv(r_m3D[i],skern); 
    resultY[i].PrintMatrix();
  }
  cout << "X convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 1; i <= z; i++) {
    resultX[i].convT(resultY[i],skern); 
    resultX[i].PrintMatrix();
  }
  // Third: in Z direction
  resultY.conv(resultX,skern);
  cout << "Z convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 1; i <= z; i++) {
    resultY[i].PrintMatrix();
  }
  r_m3Dsmooth.smoothConv(r_m3D,skern);
  cout << "Alltogether convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 1; i <= z; i++) {
    r_m3Dsmooth[i].PrintMatrix();
  }
}

// -----------------------------------------------------------
// Print out results into a file "EMSegmResult<name>.m"
// Results can be display with Matlab Program listed below 
// -----------------------------------------------------------
void vtkImageEMSegmenter::PrintMatlabGraphResults(int iter,int slice,int FullProgram,int ImgXY, int ImgY, int ImgX, vtkImageEMMatrix3D ind_m,vtkImageEMMatrix w_m,vtkImageEMMatrix3D b_m) {
  char filename[40];
  char weightname[40];
  int appendFlag = 0;
  int y,x,c;

  vtkImageEMVector weight(ImgXY);
  
  sprintf(filename,"EMSegmResult%dImage.m",iter);
  ind_m[slice].WriteMatrixToFile(filename,"Image");  

  for (c = 1; c <= w_m.get_dim(2); c++ ) {
    weight.setVector(w_m,ImgXY*(slice-1)+1,ImgXY*slice,c);
    sprintf(weightname,"weight(%d,:)",c);
    sprintf(filename,"EMSegmResult%dWeight%d.m",iter,c);
    weight.WriteVectorToFile(filename,weightname);
  }
  sprintf(filename,"EMSegmResult%db_m.m",iter);
  b_m[slice].WriteMatrixToFile(filename,"b_m");

  if (FullProgram) {
    sprintf(filename,"EMSegmResult%d.m",iter);
    FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
    if ( f == NULL ) {
      cerr << "Could not open file " << filename << "\n";
      return;
    }

    fprintf(f,"%% (c) Copyright 2001 Massachusetts Institute of Technology\n");
    fprintf(f,"%% Result of the %d iteration of the EM Algorithm\n",iter);
    fprintf(f,"EMSegmResult%dImage\n",iter);
    fprintf(f,"figure(%d1)\n",iter);
    fprintf(f,"set(%d1, 'position', [10 445 300 300]);\n",iter);
    fprintf(f,"imagesc([Image(:,:)]);\n");
    fprintf(f,"title('Label Map - Iteration %d');\n", iter);
    fprintf(f,"colormap([0 0 1; 0 1 0; 1 0 0; 1 1 0;]);\n");
    fprintf(f,"\n");

    fprintf(f,"figure(%d2);\n",iter);
    fprintf(f,"set(%d2, 'position', [10 10 1210 360]);\n",iter);
    fprintf(f,"colormap(gray); \n");
    fprintf(f,"dwm=[];\n");
    for (c = 1; c <= w_m.get_dim(2); c++ ) 
      fprintf(f,"EMSegmResult%dWeight%d\n",iter,c); 
    fprintf(f,"for t=1:%d\n",w_m.get_dim(2));
    fprintf(f,"  temp_m=reshape(weight(t,:),[%d %d]);\n",ImgY, ImgX); 
    fprintf(f,"  dwm = [dwm,  temp_m(:,:)];\n");
    fprintf(f,"end\n");
    fprintf(f,"imagesc([dwm]);\n");
    fprintf(f,"title('Weights - Iteration %d');\n", iter);
    fprintf(f,"\n");

    fprintf(f,"EMSegmResult%db_m\n",iter);
    fprintf(f,"figure(%d3);\n",iter);
    fprintf(f,"set(%d3, 'position', [600 445 300 300]);\n",iter);
    fprintf(f,"imagesc(b_m(:,:));\n");
    fprintf(f,"title('Bias - Iteration %d');\n",iter);
    fflush(f);
    fclose(f);
  }
}

void vtkImageEMSegmenter::DeterminLabelMap(vtkImageEMMatrix3D & Image, vtkImageEMMatrix w_m, int NumClassPlus, int NumSlicesPlus, int imgY, int imgXY, int imgXPlus, int imgYPlus) { 
  int k,i,j,l,MaxProbIndex,index;
  for (k=1; k<NumSlicesPlus ; k++) {
    index = (k-1)*imgXY;
    for (i=1; i < imgXPlus; i++) {
      for (j=1; j < imgYPlus; j++) {
	MaxProbIndex = 1;

	for (l=2; l<NumClassPlus; l++) 
	  if (w_m[index+j][l] > w_m[index+j][MaxProbIndex]) MaxProbIndex = l;
 
	Image[k][j][i] = this->Label[MaxProbIndex];
      }
      index +=imgY;
    }
  }
}

/*
EMSegmResult<iteration>
figure(1)
set(1, 'position', [10 445 300 300]);
seg_cur_m = reshape(ind_m, imS); %segmentation from em+mrf
imagesc([seg_cur_m(:,:,s_slice)]);
title('Label Map');
colormap([0 0 1; 0 1 0; 1 0 0; 1 1 0;]);

figure(2);
set(2, 'position', [10 10 600 150]);
title('Weights');
dwm=[];
for t=1:num_classes
 temp_m=reshape(w_m(:,t),imS); 
  dwm = [dwm,  temp_m(:,:,s_slice)];
end
imagesc([dwm]);
//
figure(3);
set(3, 'position', [600 445 300 300]);
imagesc(b_m(:,:,s_slice));
colormap(gray); 
title('Bias');

pause(0.2)
*/

